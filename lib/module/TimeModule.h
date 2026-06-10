#pragma once

#include <Arduino.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <WiFi.h>
#include "Module.h"
#include "say.h"
#include "passwords.h"

#define TIMEZONE            usET
#define NTP_PACKET_SIZE     48       // NTP time stamp is in the first 48 bytes of the message

class TimeModule : public Module
{
public:
    TimeModule(std::string name) : Module(std::move(name))
    {
    }

protected:
    bool setup() override
    {
        // Request time from NTP server and synchronize the local clock
        // (clock may pause since this may take >100ms)
        syncTime();

        return true;
    }

    void cycle() override
    {
        time_secs += 1.0; // Adjust for time taken to render

        // Midnight roll-over
        if (time_secs >= (60 * 60 * 24))
            time_secs = 0;

        // Request time from NTP server and synchronize the local clock
        // (clock may pause since this may take >100ms)
        // occurs every midnight
        if (time_secs == 0)
            syncTime();
    }

private:
    // Try to use pool URL instead so the server IP address is looked up from those available
    // (use a pool server in your own country to improve response time and reliability)
    // const char* ntpServerName = "time.nist.gov";
    // const char* ntpServerName = "pool.ntp.org";
    const char* ntpServerName = "time.google.com";

    IPAddress timeServerIP; // Use server pool

    // A UDP instance to let us send and receive packets over UDP
    WiFiUDP udp;

    uint32_t targetTime = 0; // Time for next tick
    unsigned short h = 0, m = 0, s = 0; // Time h:m:s
    float time_secs = h * 3600 + m * 60 + s;

    //====================================================================================
    //                                  Variables
    //====================================================================================
    // TimeChangeRule *tz1_Code;   // Pointer to the time change rule, use to get the TZ abbrev, e.g. "GMT"

    time_t utc = 0;

    bool timeValid = false;

    unsigned int localUdpPort = 2390; // local port to listen for UDP packets

    byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

    unsigned short lastMinute = 0;

    uint32_t nextSendTime = 0;
    uint32_t newRecvTime = 0;
    uint32_t lastRecvTime = 0;

    uint32_t newTickTime = 0;
    uint32_t lastTickTime = 0;

    uint32_t no_packet_count = 0;

    void wifiDisconnect()
    {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }

    //====================================================================================
    //                                    Update Time
    //====================================================================================

    void syncTime()
    {
        // Don't send too often so we don't trigger Denial of Service
        if (nextSendTime < millis())
        {
            // Wait 1 hour for next sync
            nextSendTime = millis() + 60 * 60 * 1000;

            // Start the UDP connection
            // if (!wifiConnect()) // Ensure WiFi is connected
            //     return;
            udp.begin(localUdpPort);

            // Get a random server from the pool
            WiFi.hostByName(ntpServerName, timeServerIP);

            sendNTPpacket(timeServerIP); // send an NTP packet to a time server
            decodeNTP();

            udp.stop();
            wifiDisconnect(); // Disconnect to save power

            if (no_packet_count > 0)
            {
                // Wait 1 minute for next sync
                nextSendTime = millis() + 60 * 1000;
            }
            else
            {
                // Wait 1 hour for next sync
                nextSendTime = millis() + 60 * 60 * 1000;
            }
        }
    }

    //====================================================================================
    // Send an NTP request to the time server at the given address
    //====================================================================================
    void sendNTPpacket(IPAddress& address)
    {
        // tft->println("sending NTP packet...");
        // set all bytes in the buffer to 0
        memset(packetBuffer, 0, NTP_PACKET_SIZE);

        // Initialize values needed to form NTP request
        // (see URL above for details on the packets)
        packetBuffer[0] = 0b11100011; // LI, Version, Mode
        packetBuffer[1] = 0; // Stratum, or type of clock
        packetBuffer[2] = 6; // Polling Interval
        packetBuffer[3] = 0xEC; // Peer Clock Precision

        // 8 bytes of zero for Root Delay & Root Dispersion

        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;

        // all NTP fields have been given values, now
        // you can send a packet requesting a timestamp:
        udp.beginPacket(address, 123); // NTP requests are to port 123
        udp.write(packetBuffer, NTP_PACKET_SIZE);
        udp.endPacket();
    }

    void displayET(time_t utc)
    {
        time_t eastern;
        TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // UTC - 4 hours
        TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300}; // UTC - 5 hours
        Timezone usEastern(usEDT, usEST);
        eastern = usEastern.toLocal(utc);

        // Display the time on the screen
        say("Time is %2d:%02d:%02d %s", hourFormat12(eastern), minute(eastern), second(eastern),
            isPM(eastern) ? "PM" : "AM");
    }

    //====================================================================================
    // Decode the NTP message and print status to serial port
    //====================================================================================
    void decodeNTP()
    {
        timeValid = false;
        uint32_t waitTime = millis() + 500;

        while (millis() < waitTime && !timeValid)
        {
            yield();
            if (udp.parsePacket())
            {
                newRecvTime = millis();

                // We've received a packet, read the data from it
                udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

                say("NTP response time was %dms", 500 - (waitTime - newRecvTime));
                say("Time since last sync is %0.1fs", (newRecvTime - lastRecvTime) / 1000.0);

                lastRecvTime = newRecvTime;

                // The timestamp starts at byte 40 of the received packet and is four bytes,
                // or two words, long. First, extract the two words:
                unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
                unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

                // Combine the four bytes (two words) into a long integer
                // this is NTP time (seconds since Jan 1 1900):
                unsigned long secsSince1900 = highWord << 16 | lowWord;

                // Now convert NTP Unix time (Seconds since Jan 1 1900) into everyday time:
                // UTC time starts on Jan 1 1970. In seconds the difference is 2208988800:
                utc = secsSince1900 - 2208988800UL;

                setTime(utc); // Set system clock to utc time (not time zone compensated)

                timeValid = true;

                unsigned short hh = hour(utc);
                unsigned short mm = minute(utc);
                unsigned short ss = second(utc);

                time_secs = hh * 3600 + mm * 60 + ss; // Update the clock time
                displayET(utc); // Display the time in Eastern Time
            }
        }

        // Keep a count of missing or bad NTP replies

        if (timeValid)
        {
            no_packet_count = 0;
        }
        else
        {
            say("No NTP reply, trying again in 1 minute...");
            no_packet_count++;
        }

        if (no_packet_count >= 10)
        {
            no_packet_count = 0; // Reset to one hour to try later
            say("No NTP packet in last 10 minutes");
        }
    }
};
