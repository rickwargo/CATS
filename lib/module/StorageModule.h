#pragma once

#include <Arduino.h>
#include "SPIModule.h"
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
    // File file;

    void sdError(const char* msg)
    {
        say("error: %s", msg);

        // if (file.isOpen()) file.close();

        tryToRecoverFromError();
    }

    bool initSdCard()
    {
        // Initialize SD card. Default SPI pins are used: SCK=18, MISO=19, MOSI=23, CS=5
        // if (!SD.begin(SD_CS)) {
        //     Serial.println("Card Mount Failed");
        //     return false;
        // }

        return true;
    }

    void initPins()
    {
        pinMode(SD_CS, OUTPUT);
    }

    char filename[32];

    bool initFile()
    {
        // snprintf(filename, sizeof(filename), "%s-%02d%02d%02d%02d%02d.csv", FILE_BASE_NAME, year() % 100, month(), day(), hour(), minute());

        // Open the file for writing
        // if (!file.open(filename, O_WRONLY | O_CREAT))
        // {
        //     sdError("File open failed.");
        //     return false;
        // }
        // say("Logging to: %s", filename);
        return true;
    }

    bool writeHeader()
    {
        // Write CSV header
        // file.printf("Time,%s\n", "Dummy");

        // Force data to SD and update the directory entry to avoid data loss.
        // if (!file.sync() || file.getWriteError())
        // {
        //     sdError("header write error");
        //     return false;
        // }
        // file.close();
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
            time_t timeNow = millis();

            // if (file.open(filename, O_WRONLY | O_APPEND | O_SYNC))
            // {
            //     file.printf("%lld,%s\n", timeNow, currentExportData);
            //     file.close();
            // }
            // else
            // {
            //     sdError("File open failed attempting to append export data.");
            // }
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
