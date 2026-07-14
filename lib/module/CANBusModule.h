#pragma once

#include <Arduino.h>
#include <string>
#include <driver/twai.h>
#include <utility>
#include "Module.h"
#include "Event.h"
#include "ISubscriber.h"
#include "say.h"

class CANBusModule : public Module, public ISubscriber
{
public:
    CANBusModule(std::string name, gpio_num_t txPin, gpio_num_t rxPin, short busSpeed = 500)
        : Module(std::move(name)), txPin(txPin), rxPin(rxPin), busSpeed(busSpeed) {}

    void onEvent(Event& e) override
    {
        switch (e.type) {
        // case Event::CANBusFeedbackRequest:
        //     onFeedbackRequest((short)e.data);
        //     break;
        default:
            break;
        }
    }

protected:
    gpio_num_t txPin, rxPin;
    short busSpeed;

    bool setup() override
    {
        ctx.bus->subscribe(this);

        pinMode(txPin, OUTPUT);
        pinMode(rxPin, INPUT);

        // 1. General Configuration (Pins and Mode)
        // TWAI_MODE_NORMAL allows both reading and writing to the bus
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(txPin, rxPin, TWAI_MODE_NORMAL);

        // 2. Timing Configuration (Set your Bitrate here, e.g., 500 kbps)
        twai_timing_config_t t_config;
        switch (busSpeed)
        {
            case 100:
                t_config = TWAI_TIMING_CONFIG_100KBITS();
                break;
            case 250:
                t_config = TWAI_TIMING_CONFIG_250KBITS();
                break;
            case 500:
                t_config = TWAI_TIMING_CONFIG_500KBITS();
                break;
            case 800:
                t_config = TWAI_TIMING_CONFIG_800KBITS();
                break;
            case 1000:
                t_config = TWAI_TIMING_CONFIG_1MBITS();
                break;
            default:
                t_config = TWAI_TIMING_CONFIG_500KBITS();
                break;
        }

        // 3. Filter Configuration (Accept all incoming messages)
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

        // Install the driver
        if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
            say("[CANBus] ERROR: Failed to install TWAI/CAN driver");
            return false;
        }

        // Start the driver
        if (twai_start() != ESP_OK) {
            say("[CANBus] ERROR: Failed to start TWAI/CAN controller");
            return false;
        }

        say("[CANBus] CAN Controller successfully started at %dkbps.", busSpeed);

        return true;
    }

    void cycle() override
    {
        // Non-blocking transmission interval
        // if (millis() - lastTransmitTime >= transmitInterval) {
        //     sendCANMessage();
        //     lastTransmitTime = millis();
        // }

        // Check for incoming CAN messages continuously
        receiveCANMessage();
    }

private:
    // Timing configurations
    unsigned long lastTransmitTime = 0;
    const unsigned long transmitInterval = 10000;

    void sendCANMessage() {
        twai_message_t tx_msg;

        tx_msg.identifier = 0x0AF;           // CAN ID (Standard 11-bit ID)
        tx_msg.extd = 0;                     // 0 = Standard Frame, 1 = Extended Frame (29-bit)
        tx_msg.rtr = 0;                      // 0 = Data Frame, 1 = Remote Transmission Request
        tx_msg.data_length_code = 4;         // Number of data bytes (0 to 8)

        // Fill payload data
        tx_msg.data[0] = 0x00;
        tx_msg.data[1] = 0x80;
        tx_msg.data[2] = 0xFF;
        tx_msg.data[3] = 0x80;

        // Queue the message for transmission (Non-blocking with a small wait window)
        esp_err_t result = twai_transmit(&tx_msg, pdMS_TO_TICKS(10));

        if (result == ESP_OK) {
            say("CAN message sent successfully!");
        } else {
            say("Failed to send message. Error code: 0x%X", result);
        }
    }

    void receiveCANMessage() {
        twai_message_t rx_msg;

        // Check if a message is available in the buffer (Wait up to 1ms)
        // Changing the wait time to 0 makes this completely non-blocking
        if (twai_receive(&rx_msg, pdMS_TO_TICKS(1)) == ESP_OK) {

            // Print message meta details
            say("Received %sFrame! ID: 0x%03X", rx_msg.extd ? "Extended " : "", rx_msg.identifier);

            say(" DLC: %d Data: ", rx_msg.data_length_code);

            // Loop through and print payload data bytes
            for (int i = 0; i < rx_msg.data_length_code; i++) {
                Serial.printf("0x%02X ", rx_msg.data[i]);
            }
            Serial.println();
        }
    }
};
