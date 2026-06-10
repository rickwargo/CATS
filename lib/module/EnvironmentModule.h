#pragma once

#include <Arduino.h>
#include <bsec2.h>
#include <utility>

#include "module/I2CModule.h"
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
    float iaqRaw;
    short staticIaq;
    unsigned short iaqAccuracy;
    float co2Equivalents;
    float bVocEquivalents;
    float temperature;
    float pressure;
    float humidity;
    float gasSensorResistance;
    float rawTemperature;
    float rawPressure;
    float rawHumidity;
    float rawGasSensorResistance;
    int initialStabilization;
    int gasReadingStabilized;
    float gasPercentage;
    int64_t time_stamp;

    void update(const bsecOutputs& outputs);
    void outputData();

    // report a good value, but only when it has stabilized
    short iaq() const { return gasReadingStabilized ? staticIaq : -1; }

    // Convert from hPa (millibars) to inHg
    float pressure_inHg() const { return pressure * 0.02952998057228; }
    float pressure_mmHg() const { return pressure * 0.02952998057228 * 25.4; }

    // convert to degrees Fahrenheit
    float temperature_degF() const { return temperature * 9.0 / 5.0 + 32.0; }

    // convert to kOhms
    float gasSensorResistance_kOhms() const
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
public:
    EnvironmentModule(std::string name, unsigned int cycleCheckTime) : I2CModule(std::move(name), cycleCheckTime), envData()
    {
    }

protected:
    bool setup() override
    {
        instance = this;

        if (!bme680Setup()) return false;
        return true;
    }

    void cycle() override
    {
        if (!environmentSensor.run())
            checkBsecStatus(environmentSensor);
        else
            envData.outputData();

        int iaq = envData.iaq();
        if (previousIAQ <= 200 && iaq > 200)
            onVeryUnhealthy(iaq);
        else if (previousIAQ > 200 && iaq <= 100)
            onModerate(iaq);
        if (iaq != previousIAQ)
        {
            onIAQChanged(iaq);
            previousIAQ = iaq;
        }
        float temperature = envData.temperature_degF();
        if ((int)(temperature*10) != (int)(previousTemperature*10)) // limit to a decimal digit precision for temp change
        {
            previousTemperature = temperature;
            onTemperatureChanged(temperature);
        }
        float pressure = envData.pressure;
        if ((int)(pressure*10) != (int)(previousPressure*10)) // limit to a decimal digit precision for temp change
        {
            previousPressure = pressure;
            onPressureChanged(pressure);
        }
        float humidity = envData.humidity;
        if ((int)(humidity*10) != (int)(previousHumidity*10)) // limit to a decimal digit precision for temp change
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

    virtual void onTemperatureChanged(float temperature)
    {
        ctx.bus->publish({Event::TemperatureChanged, this, (int)(temperature*10)});
    }

    virtual void onIAQChanged(int iaq)
    {
        ctx.bus->publish({Event::IAQChanged, this, iaq});
    }

    virtual void onPressureChanged(float pressure)
    {
        ctx.bus->publish({Event::PressureChanged, this, (int)(pressure*10)});
    }

    virtual void onHumidityChanged(float humidity)
    {
        ctx.bus->publish({Event::HumidityChanged, this, (int)(humidity*10)});
    }

private:
    Bsec2 environmentSensor; // Interface with the BME-680
    EnvironData envData;
    int previousIAQ = 0;
    float previousTemperature = 0.0;
    float previousPressure = 0.0;
    float previousHumidity = 0.0;
    static EnvironmentModule* instance;
    static void newDataCallback(const bme68xData, const bsecOutputs outputs, Bsec2)
    {
        if (instance) {
            instance->handleNewData(outputs);
        }
    }

    void handleNewData(const bsecOutputs& outputs)
    {
        envData.update(outputs);
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
EnvironmentModule* EnvironmentModule::instance = nullptr;

inline void EnvironData::update(const bsecOutputs& outputs)
{
    for (unsigned short i = 0; i < outputs.nOutputs; i++)
    {
        const bsecData output = outputs.output[i];
        time_stamp = output.time_stamp;

        switch (output.sensor_id)
        {
        case BSEC_OUTPUT_IAQ:
            // if (output.signal != iaq())
            // {
            //     say("[IAQ]\t\t%d", (unsigned short)output.signal);
            //     say("[IAQ Accuracy]\t%s", iaqAccuracyIndicator[output.accuracy]);
            // }
            time_stamp = output.time_stamp;
            iaqAccuracy = output.accuracy;
            iaqRaw = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_STATIC_IAQ:
            // if ((unsigned short)output.signal != staticIaq)
            //     say("[s-IAQ]\t\t%f", output.signal);
            staticIaq = gasReadingStabilized ? (unsigned short)output.signal : -1;
            break;
        case BSEC_OUTPUT_CO2_EQUIVALENT:
            // if (output.signal != co2Equivalents)
            //     say("[CO2]\t\t%f", output.signal);
            co2Equivalents = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
            // if (output.signal != bVocEquivalents)
            //     say("[b-VOC]\t\t%f", output.signal);
            bVocEquivalents = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_RAW_HUMIDITY:
            // if (output.signal != rawHumidity)
            //     say("[Humidity]\t%f", output.signal);
            rawHumidity = output.signal;
            break;
        case BSEC_OUTPUT_STABILIZATION_STATUS:
            // if (output.signal != initialStabilization)
            //     say("[Initialized]\t%f", output.signal);
            initialStabilization = output.signal;
            break;
        case BSEC_OUTPUT_RUN_IN_STATUS:
            // if (output.signal != gasReadingStabilized)
            //     say("[Stable]\t\t%f", output.signal);
            gasReadingStabilized = output.signal;
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
            // if (output.signal != humidity)
            //     say("[adjHumidity]\t%f", output.signal);
            humidity = output.signal;
            break;
        case BSEC_OUTPUT_COMPENSATED_GAS:
            // if (output.signal != gasSensorResistance)
            //     say("[adjGasRes]\t%f", output.signal);
            gasSensorResistance = gasReadingStabilized ? output.signal : -1;
            break;
        case BSEC_OUTPUT_RAW_TEMPERATURE:
            // if (output.signal != rawTemperature)
            //     say("[Temp °C]\t%f", output.signal);
            rawTemperature = output.signal;
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
            // if (output.signal != temperature)
            //     say("[adjTemp °C]\t%f", output.signal);
            temperature = output.signal;
            break;
        case BSEC_OUTPUT_RAW_PRESSURE:
            // if (output.signal != rawPressure)
            //     say("[Pressure]\t%f", output.signal);
            rawPressure = output.signal;
            pressure = output.signal; // in hectoPascals (hPa), 1 hPa = 0.02952998057228 inHg
            break;
        case BSEC_OUTPUT_RAW_GAS:
            // if (output.signal != rawGasSensorResistance)
            //     say("[GasRes]\t\t%f", output.signal);
            rawGasSensorResistance = output.signal;
            break;
        case BSEC_OUTPUT_GAS_PERCENTAGE:
            // if (output.signal != gasPercentage)
            //     say("[gasPct]\t\t%f", output.signal);
            gasPercentage = gasReadingStabilized ? output.signal : -1;
            break;
        default:
            break;
        }
    }
}

void EnvironData::outputData()
{
    static uint32_t targetTime = 0; // Time for next output of data

    if (millis() < targetTime || pressure == 0) // invalid reading if pressure is zero
        return;
    targetTime = millis() + 37000;

#ifdef WEATHER_NOW
    say("====== Weather Now ======");
    say("Offset:      %.1f sec", time_stamp / 1000000000.0);
    if (gasReadingStabilized)
        say("IAQ:         %d", iaq());
    say("Temperature: %.1f F", temperature_degF());
    say("Humidity:    %.1f%%", humidity);
    say("Pressure:    %.2f hPa", pressure);
    if (gasReadingStabilized)
        say("Gas:         %.3f kOhms", gasSensorResistance_kOhms());
    if (gasReadingStabilized)
        say("Gas %%:       %.1f%%", gasPercentage);
    if (gasReadingStabilized)
        say("CO2:         %.1f ppm", co2Equivalents);
    if (gasReadingStabilized)
        say("b-VOC:       %.2f", bVocEquivalents);

    say("[%.1f] IAQ %d: %.1fF / %.1f%% / %.1fhPa / %.1fkOhms (%.1f%%) / %.1fppm / bVOC %.2f",
                  time_stamp / 1000000000.0, // nanoseconds --> seconds
                  iaq(),
                  temperature_degF(),
                  humidity,
                  pressure,
                  gasSensorResistance_kOhms(),
                  gasPercentage,
                  co2Equivalents,
                  bVocEquivalents);
#endif
}

// char *EnvironData::bme680Header()
// {
//     return (char *)"IAQ_Raw,Static_IAQ,IAQ_Accuracy,CO2_Equiv,BVOC_Equiv,Temp_C,Pressure_hPa,Humidity_pct,Adj_Gas_Ohms,Raw_Temp_C,Raw_Pressure_hPa,Raw_Humidity_pct,Raw_Gas_Ohms,InitialStabilization,GasReadingStable,Gas_Pct";
// }
//
// char *EnvironData::bme680ExportData()
// {
//     static char buffer[512];
//     snprintf(buffer, sizeof(buffer),
//         "%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f",
//         iaqRaw,
//         staticIaq,
//         iaqAccuracy,
//         co2Equivalents,
//         bVocEquivalents,
//         temperature,
//         pressure,
//         humidity,
//         gasSensorResistance,
//         rawTemperature,
//         rawPressure,
//         rawHumidity,
//         rawGasSensorResistance,
//         initialStabilization,
//         gasReadingStabilized,
//         gasPercentage);
//
//     return buffer;
// }
