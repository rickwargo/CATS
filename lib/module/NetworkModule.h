#pragma once
#include <WiFi.h>
#include <WiFiMulti.h>
#include "Module.h"
#include "passwords.h"
#include "say.h"

#define	MAX_WIFI_NETWORKS	4

class NetworkModule : public Module
{
public:
    NetworkModule(std::string name) : Module(std::move(name)) {}

protected:
    bool setup() override
    {
        if (!wifiConnect()) return false;

#if CORE_DEBUG_LEVEL >= 4
        IPAddress octet = WiFi.localIP();
        say("IP address: %d.%d.%d.%d", octet[0], octet[1], octet[2], octet[3]);
        say("WiFi: %s", WiFi.SSID().c_str());
        say("Channel: %d, RSSI: %d", WiFi.channel(), WiFi.RSSI());
#endif
        return true;
    }

private:
    const char *knownWifiNetworks[MAX_WIFI_NETWORKS] = {
        WIFI1_SSID,
        WIFI2_SSID,
        WIFI3_SSID,
        WIFI4_SSID,
    };

    const char *wifiPasswords[MAX_WIFI_NETWORKS] = {
        WIFI1_PASSWORD,
        WIFI2_PASSWORD,
        WIFI3_PASSWORD,
        WIFI4_PASSWORD,
    };

    // Wi-Fi connect timeout per AP. Increase when connecting takes longer.
    const unsigned long connectTimeoutMs = 10000;
    WiFiMulti wifiMulti;

    bool wifiConnect()
    {
        unsigned short status;

        // WiFi.mode(WIFI_STA);
        // WiFi.begin(WIFI3_SSID, WIFI3_PASSWORD);
        for (int wifiIndex = 0; wifiIndex < MAX_WIFI_NETWORKS; wifiIndex++)
        {
            wifiMulti.addAP(knownWifiNetworks[wifiIndex], wifiPasswords[wifiIndex]);
        }
        status = wifiMulti.run();
        if (status != WL_CONNECTED) {
            say("WiFi Multi connection status: %d", (int)status);
            return false;
        }

        return true;
    }

    void wifiDisconnect()
    {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
};
