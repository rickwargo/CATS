#pragma once

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

using fs::FS;
#include <ElegantOTA.h>

#include "Event.h"
#include "Module.h"

class OTAUpdateModule : public Module, public ISubscriber
{
public:
    OTAUpdateModule(std::string name) : Module(std::move(name)) {}

    void onEvent(Event& e) override
    {
        switch (e.type) {
        case Event::OTAUpdateRequest:
            onUpdateRequest((short)e.data);
            break;
        default:
            break;
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/html", "<html><body><h1>OTA Update Web UI</h1></body></html>");
        });

        server.on("/health", [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", "OK");
        });
        ElegantOTA.setAutoReboot(true);
        ElegantOTA.onStart(onOTAStart);
        ElegantOTA.onProgress(onOTAProgress);
        ElegantOTA.onEnd(onOTAEnd);


        ElegantOTA.begin(&server);
        server.begin();

        return true;
    }

    void cycle() override
    {
        ElegantOTA.loop();
    }

    virtual void onUpdateRequest(short data) {
    }

private:
    AsyncWebServer server = AsyncWebServer(80);

    static void onOTAStart() {
        // Log when OTA has started
        Serial.println("OTA update started!");
        // <Add your own code here>
    }

    static void onOTAProgress(size_t current, size_t final) {
        static unsigned long ota_progress_millis = 0;

        // Log every 1 second
        if (millis() - ota_progress_millis > 1000) {
            ota_progress_millis = millis();
            Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
        }
    }

    static void onOTAEnd(bool success) {
        // Log when OTA has finished
        if (success) {
            Serial.println("OTA update finished successfully!");
        } else {
            Serial.println("There was an error during OTA update!");
        }
        // <Add your own code here>
    }

};
