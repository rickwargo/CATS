#pragma once

#include <bsec2.h>

#include "I2CModule.h"
#include "Event.h"

// Sample Rate defines the frequency an environment is sensed by the BSEC chip
// The BSEC (Bosch Software Environmental Cluster) library for the BME680 primarily supports
// two power and sampling modes:
// - Low Power (LP): Samples every 3 seconds (0.33 Hz).
// - Ultra-Low Power (ULP): Samples every 5 minutes (1/300 Hz).
//
// Key Details & Configurations
// - Gas Sensors (IAQ, eCO₂, bVOC): These calculations require the built-in heater and
//     rely exclusively on the LP or ULP modes.
// - Temperature, Pressure & Humidity (T/P/H): Standard environmental readings can be decoupled
//     from the gas sensor if needed, allowing you to sample non-gas metrics in LP while keeping
//     the gas sensor in ULP to save power.
// - Continuous Mode: While the raw hardware supports faster 1 Hz readings, the
//     BSEC algorithm restricts output to the rates above, though the library does allow
//     for manual configurations in some integrations.
//
// For example, when using popular integrations like ESPHome or standard Arduino BSEC,
// you define your rate via the configuration object or initialization function.

#define SAMPLE_RATE		 BSEC_SAMPLE_RATE_LP
// #define SAMPLE_RATE		 BSEC_SAMPLE_RATE_ULP


inline const char* accuracyStatus[] = {
    "Unreliable",
    "Low Accuracy",
    "Medium Accuracy",
    "High Accuracy",
};
inline const char* iaqAccuracyIndicator[] = {
    "Stabilizing",
    "Uncertain",
    "Calibrating",
    "Calibrated",
};
inline const char* iaqIndexStatus[] = {
    "Excellent",
    "Good",
    "Lightly polluted",
    "Moderately polluted",
    "Heavily polluted",
    "Severely polluted",
    "Extremely polluted",
};

class EnvironData
{
public:
    float iaqRaw = 0;
    short staticIaq = 0;
    unsigned short iaqAccuracy = 0;
    float co2Equivalents = 0;
    float bVocEquivalents = 0;
    float temperature = 0;
    float pressure = 0;
    float humidity = 0;
    float gasSensorResistance = 0;
    float rawTemperature = 0;
    float rawPressure = 0;
    float rawHumidity = 0;
    float rawGasSensorResistance = 0;
    int initialStabilization = 0;
    int gasReadingStabilized = 0;
    float gasPercentage = 0;
    int64_t time_stamp = 0;

    void update(const bsecOutputs& outputs);
    void outputData();
    // report a good value, but only when it has stabilized
    [[nodiscard]] short iaq() const { return gasReadingStabilized ? staticIaq : -1; }

    // Convert from hPa (millibars) to inHg
    static float to_inHg(float p) { return p * 0.02952998057228; }
    [[nodiscard]] float pressure_inHg() const { return to_inHg(pressure); }

    // convert to degrees Fahrenheit
    static float to_degF(float t) { return t * 9.0 / 5.0 + 32.0; }
    [[nodiscard]] float temperature_degF() const { return to_degF(temperature); }

    // convert to kOhms
    [[nodiscard]] float gasSensorResistance_kOhms() const
    {
        return gasReadingStabilized ? pow(10, gasSensorResistance) / 1000 : -1;
    }

private:
    unsigned long headerLastPrinted = 0;
    short windowDuration = 1; /* seconds */
    short headerWindowDuration = 60; /* seconds */
};

class EnvironmentModule : public I2CModule
{
    float iaqRaw = 0;
    short staticIaq = 0;
    unsigned short iaqAccuracy = 0;
    float co2Equivalents = 0;
    float bVocEquivalents = 0;
    float temperature = 0;
    float pressure = 0;
    float humidity = 0;
    float gasSensorResistance = 0;
    float rawTemperature = 0;
    float rawPressure = 0;
    float rawHumidity = 0;
    float rawGasSensorResistance = 0;
    int initialStabilization = 0;
    int gasReadingStabilized = 0;
    float gasPercentage = 0;
    int64_t time_stamp = 0;
    // report a good value, but only when it has stabilized
    [[nodiscard]] short stabilizedIaq() const { return gasReadingStabilized ? staticIaq : -1; }
    // Convert from hPa (millibars) to inHg
    static float to_inHg(float p) { return p * 0.02952998057228; }
    [[nodiscard]] float pressure_inHg() const { return to_inHg(pressure); }
    // convert to degrees Fahrenheit
    static float to_degF(float t) { return t * 9.0 / 5.0 + 32.0; }
    [[nodiscard]] float temperature_degF() const { return to_degF(temperature); }
    // convert to kOhms
    [[nodiscard]] float gasSensorResistance_kOhms() const
    {
        return gasReadingStabilized ? pow(10, gasSensorResistance) / 1000 : -1;
    }

public:
    EnvironmentModule(std::string name, unsigned int cycleCheckTime) : I2CModule(std::move(name), cycleCheckTime)
    {
    }

protected:
    bool setup() override
    {
        if (!bme680Setup()) return false;
        return true;
    }

    void cycle() override
    {
        if (!environmentSensor.run())
            checkBsecStatus(environmentSensor);

        int iaq = stabilizedIaq();
        if (previousIAQ <= 200 && iaq > 200)
            onVeryUnhealthy(iaq);
        else if (previousIAQ > 200 && iaq <= 100)
            onModerate(iaq);
        if (iaq != previousIAQ)
        {
            onIAQChanged(iaq);
            previousIAQ = iaq;
        }
        // look for 1°F change in temp, but keep values in °C
        if (abs(to_degF(temperature)-to_degF(previousTemperature)) >= 1) // limit to a decimal digit precision for temp change
        {
            previousTemperature = temperature;
            onTemperatureChanged(temperature_degF());
        }
        if (abs(pressure-previousPressure) >= 1) // limit to a decimal digit precision for temp change
        {
            previousPressure = pressure;
            onPressureChanged(pressure);
        }
        if (abs(humidity-previousHumidity) >= 1) // limit to a decimal digit precision for temp change
        {
            previousHumidity = humidity;
            onHumidityChanged(humidity);
        }
    }

    virtual void onVeryUnhealthy(int iaq)
    {
        ctx.bus->publish({Event::IAQVeryUnhealthy, this, iaq});
    }

    virtual void onModerate(int iaq)
    {
        ctx.bus->publish({Event::IAQModerate, this, iaq});
    }

    virtual void onTemperatureChanged(float newTemperature)
    {
        ctx.bus->publish({Event::TemperatureChanged, this, (int)(newTemperature*10)});
    }

    virtual void onIAQChanged(int newIaq)
    {
        ctx.bus->publish({Event::IAQChanged, this, newIaq});
    }

    virtual void onPressureChanged(float newPressure)
    {
        ctx.bus->publish({Event::PressureChanged, this, (int)(newPressure*10)});
    }

    virtual void onHumidityChanged(float newHumidity)
    {
        ctx.bus->publish({Event::HumidityChanged, this, (int)(newHumidity*10)});
    }

private:
    Bsec2 environmentSensor; // Interface with the BME-680
    int previousIAQ = 0;
    float previousTemperature = 0.0;
    float previousPressure = 0.0;
    float previousHumidity = 0.0;

    static void newDataCallback(const bme68xData, const bsecOutputs outputs, Bsec2)
    {
        // handleNewData(outputs);
    }

    void handleNewData(const bsecOutputs& outputs)
    {
        // envData.update(outputs);
    }

    bool bme680Setup()
    {
        /* Desired subscription list of BSEC2 outputs */
        bsecSensor sensorList[] = {
            BSEC_OUTPUT_IAQ,
            BSEC_OUTPUT_STATIC_IAQ,
            BSEC_OUTPUT_CO2_EQUIVALENT,
            BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
            BSEC_OUTPUT_RAW_TEMPERATURE,
            BSEC_OUTPUT_RAW_PRESSURE,
            BSEC_OUTPUT_RAW_HUMIDITY,
            BSEC_OUTPUT_RAW_GAS,
            BSEC_OUTPUT_STABILIZATION_STATUS,
            BSEC_OUTPUT_RUN_IN_STATUS,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
            BSEC_OUTPUT_COMPENSATED_GAS,
            BSEC_OUTPUT_GAS_PERCENTAGE
        };

        if (!environmentSensor.begin(BME68X_I2C_ADDR_HIGH, Wire))
        {
            checkBsecStatus(environmentSensor);
            return false;
        }
        /*
         *,The default offset provided has been determined by testing the sensor in LP and ULP mode on application board 3.0
         *,Please update the offset value after testing this on your product
         */
        if (SAMPLE_RATE == BSEC_SAMPLE_RATE_ULP)
        {
            environmentSensor.setTemperatureOffset(TEMP_OFFSET_ULP);
        }
        else if (SAMPLE_RATE == BSEC_SAMPLE_RATE_LP)
        {
            environmentSensor.setTemperatureOffset(TEMP_OFFSET_LP);
        }

        /* Subscribe to the desired BSEC2 outputs */
        if (!environmentSensor.updateSubscription(sensorList, ARRAY_LEN(sensorList), SAMPLE_RATE))
            checkBsecStatus(environmentSensor);

        /* Whenever new data is available, call the newDataCallback function */
        environmentSensor.attachCallback(newDataCallback);

        return true;
    };

    void checkBsecStatus(Bsec2 bsec)
    {
        if (bsec.status < BSEC_OK)
        {
            say("BSEC error code: %d", bsec.status);
        }
        else if (bsec.status > BSEC_OK)
        {
            // BSEC warning code 100 (BSEC_W_SC_CALL_TIMING_VIOLATION) indicates a timing mismatch in your sensor's data collection.
            // It means the difference between the actual and defined sampling intervals for the Bosch BSEC driver is greater than allowed.
            if (bsec.status != BSEC_W_SC_CALL_TIMING_VIOLATION) say("BSEC warning code: %d", bsec.status);
        }

        if (bsec.sensor.status < BME68X_OK)
        {
            say("BME68X error code: %d", bsec.sensor.status);
        }
        else if (bsec.sensor.status > BME68X_OK)
        {
            if (bsec.status != BSEC_W_SC_CALL_TIMING_VIOLATION) say("BME68X warning code: %d", bsec.sensor.status);
        }
    }
};

inline void EnvironData::update(const bsecOutputs& outputs)
{
    for (unsigned short i = 0; i < outputs.nOutputs; i++)
    {
        const bsecData output = outputs.output[i];
        time_stamp = output.time_stamp;

        switch (output.sensor_id)
        {
        case BSEC_OUTPUT_IAQ:
            if (output.signal != iaq())
            {
                say("[IAQ]\t\t%d", (unsigned short)output.signal);
                say("[IAQ Accuracy]\t%s", iaqAccuracyIndicator[output.accuracy]);
            }
            time_stamp = output.time_stamp;
            iaqAccuracy = output.accuracy;
            iaqRaw = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_STATIC_IAQ:
            if ((unsigned short)output.signal != staticIaq)
                say("[s-IAQ]\t\t%f", output.signal);
            staticIaq = gasReadingStabilized ? (unsigned short)output.signal : -1;
            break;
        case BSEC_OUTPUT_CO2_EQUIVALENT:
            if (output.signal != co2Equivalents)
                say("[CO2]\t\t%f", output.signal);
            co2Equivalents = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
            if (output.signal != bVocEquivalents)
                say("[b-VOC]\t\t%f", output.signal);
            bVocEquivalents = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_RAW_HUMIDITY:
            if (output.signal != rawHumidity)
                say("[Humidity]\t%f", output.signal);
            rawHumidity = output.signal;
            break;
        case BSEC_OUTPUT_STABILIZATION_STATUS:
            if (output.signal != initialStabilization)
                say("[Initialized]\t%f", output.signal);
            initialStabilization = output.signal;
            break;
        case BSEC_OUTPUT_RUN_IN_STATUS:
            if (output.signal != gasReadingStabilized)
                say("[Stable]\t\t%f", output.signal);
            gasReadingStabilized = output.signal;
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
            if (output.signal != humidity)
                say("[adjHumidity]\t%f", output.signal);
            humidity = output.signal;
            break;
        case BSEC_OUTPUT_COMPENSATED_GAS:
            if (output.signal != gasSensorResistance)
                say("[adjGasRes]\t%f", output.signal);
            gasSensorResistance = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_RAW_TEMPERATURE:
            if (output.signal != rawTemperature)
                say("[Temp °C]\t%f", output.signal);
            rawTemperature = output.signal;
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
            if (output.signal != temperature)
                say("[adjTemp °C]\t%f", output.signal);
            temperature = output.signal;
            break;
        case BSEC_OUTPUT_RAW_PRESSURE:
            if (output.signal != rawPressure)
                say("[Pressure]\t%f", output.signal);
            rawPressure = output.signal;
            pressure = output.signal; // in hectoPascals (hPa), 1 hPa = 0.02952998057228 inHg
            break;
        case BSEC_OUTPUT_RAW_GAS:
            if (output.signal != rawGasSensorResistance)
                say("[GasRes]\t\t%f", output.signal);
            rawGasSensorResistance = output.signal;
            break;
        case BSEC_OUTPUT_GAS_PERCENTAGE:
            if (output.signal != gasPercentage)
                say("[gasPct]\t\t%f", output.signal);
            gasPercentage = gasReadingStabilized ? output.signal : -1;
            break;
        default:
            break;
        }
    }
}
