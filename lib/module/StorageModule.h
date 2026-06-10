#pragma once

#include <Arduino.h>
#include <TimeLib.h>
#include <SdFat.h>
#include "module/SPIModule.h"
#include "say.h"

class StorageModule : public SPIModule
{
public:
    StorageModule(std::string name) : SPIModule(std::move(name)) {}

protected:
    bool setup() override
    {
        return sdSetup();
    }

    void cycle() override
    {
        sdCycle();
    }

private:
    SdFat sd_fat; // SD filesystem object
    SdFile file; // File object

    void sdError(const char* msg)
    {
        say("error: %s", msg);

        if (file.isOpen()) file.close();
        // sd_fat.end();

        tryToRecoverFromError();
    }

    bool initSdCard()
    {
        // Initialize at the highest speed supported by the board that is
        // not over 50 MHz. Try a lower speed if SPI errors occur.
        SPIClass& spi = TFT_eSPI::getSPIinstance();  // Create a class variable to hold the SPI class instance

        if (!sd_fat.begin(SdSpiConfig(SD_CS, SHARED_SPI, VSPI_FREQUENCY, &spi)))
        {
            sdError("Failed to access SD card");
            return false;
        }
        return true;
    }

    void initPins()
    {
        pinMode(SD_CS, OUTPUT);
    }

    char filename[32];

    bool initFile()
    {
        snprintf(filename, sizeof(filename), "%s-%02d%02d%02d%02d%02d.csv", FILE_BASE_NAME, year() % 100, month(), day(), hour(), minute());

        // Open the file for writing
        if (!file.open(filename, O_WRONLY | O_CREAT))
        {
            sdError("File open failed.");
            return false;
        }
        say("Logging to: %s", filename);
        return true;
    }

    bool writeHeader()
    {
        // Write CSV header
        file.printf("Time,%s\n", "Dummy");

        // Force data to SD and update the directory entry to avoid data loss.
        if (!file.sync() || file.getWriteError())
        {
            sdError("header write error");
            return false;
        }
        file.close();
        return true;
    }

    bool sdSetup()
    {
        int result = false;

        initPins();
        try
        {
            if (initSdCard() && initFile() && writeHeader())
                result = true;
        }
        catch (const std::exception& e)
        {
            say("[sdSetup] Exception: %s", e.what());
        }
        catch (...)
        {
            say("[sdSetup] Unknown error occurred");
        }
        return result;
    }

    void sdCycle()
    {
        static char previousExportData[512] = "";
        char currentExportData[512] = "";
        if (strcmp(previousExportData, currentExportData) == 0)
        {
            // No change in data, skip writing to SD card
            return;
        }
        strcpy(previousExportData, currentExportData); // Update previous data

        try
        {
            time_t timeNow = now();

            if (file.open(filename, O_WRONLY | O_APPEND | O_SYNC))
            {
                file.printf("%lld,%s\n", timeNow, currentExportData);
                file.close();
            }
            else
            {
                sdError("File open failed attempting to append export data.");
            }
        }
        catch (const std::exception& e)
        {
            say("[sdCycle] Exception: %s", e.what());
        }
        catch (...)
        {
            say("[sdCycle] Unknown error occurred");
        }
    }
};
