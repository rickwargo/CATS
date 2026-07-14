#pragma once

#define SHIFTER_UPDATES_PER_SECOND  30

#define VSPI_CLK                    2
#define VSPI_MISO                   4
#define VSPI_MOSI                   16
#define VSPI_FREQUENCY              10000000

#define VSPI_TFT_CS                 14
#define VSPI_TFT_DC                 27
#define VSPI_TFT_RST                12

#define FILE_BASE_NAME              "cats"
#define SD_CS                       15
#define SD_CLK                      VSPI_CLK
#define SD_MISO                     VSPI_MISO
#define SD_MOSI                     VSPI_MOSI

// #define IMU_INTERRUPT_PIN           19

#define ROTARY_BUTTON_PIN           36
#define ENCODER_A                   34 // Encoder CLK
#define ENCODER_B                   39 // Encoder DT
#define ENCODER_DEFAULT_STEPS       4

#define HAPTIC_PIN_1                32
#define HAPTIC_PIN_2                33
#define SHIFTER_PIN                 25

#define BUZZER_PIN                  17

#define CAN_TX_PIN                  ((gpio_num_t)5)
#define CAN_RX_PIN                  ((gpio_num_t)18)
#define CANBUS_SPEED                500

#define I2C_SDA                     22
#define I2C_SCL                     23
#define I2C_BUS_FREQUENCY           400000
