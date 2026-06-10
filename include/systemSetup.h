#pragma once

#define SHIFTER_UPDATES_PER_SECOND  60

#define VSPI_CLK                    14
#define VSPI_MISO                   32
#define VSPI_MOSI                   27
#define VSPI_FREQUENCY              10000000

#define VSPI_TFT_CS                 13
#define VSPI_TFT_DC                 12
#define VSPI_TFT_RST                0

#define FILE_BASE_NAME              "cats"
#define SD_CS                       15
#define SD_CLK                      VSPI_CLK
#define SD_MISO                     VSPI_MISO
#define SD_MOSI                     VSPI_MOSI

#define IMU_INTERRUPT_PIN           19

#define ROTARY_BUTTON_PIN           36
#define ENCODER_A                   39 // Encoder CLK
#define ENCODER_B                   34 // Encoder DT
#define ENCODER_DEFAULT_STEPS       4

#define HAPTIC_PIN_1                33
#define HAPTIC_PIN_2                25
#define SHIFTER_PIN                 26

#define BUZZER_PIN                  2

#define I2C_SDA                     22
#define I2C_SCL                     23
#define I2C_BUS_FREQUENCY           400000
