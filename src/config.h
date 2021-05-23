#pragma once

/*----------------------------------------------------------------------------------------
Settings for ota_updater
*/
#define WIFI_SSID "yourWifiName"
#define WIFI_PASS "yourWifiPassword"

#define HOSTNAME "my-vesc"

/*----------------------------------------------------------------------------------------
Settings for balance_beeper
*/
#define BEEPER_PIN 27

#define PLAY_STARTUP true
#define DUTY_CYCLE_ALERT 0.80          // 0 to disable
#define SWITCH_ERPM 1000               // 0 to disable
#define LOW_VOLTAGE 40.0               // 0 to disable
#define LOW_VOLTAGE_INTERVAL 30 * 1000 // every 30 seconds

/*----------------------------------------------------------------------------------------
Settings for balance_display
*/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

/* Uncomment below if using I2C, otherwise use SPI.
i2c is very slow, using it might cause inconsistent beeper timings.
Long i2c wires are not recommended and may cause crashing. */

//#define USE_I2C
#ifdef USE_I2C
#include <Wire.h>
#define I2C_ADDR 0x3c
// i2c pins: SDA=D21, SCL=D22
#else
#include <SPI.h>
#define OLED_DC 16    // RX2
#define OLED_CS 5     // D5
#define OLED_RESET 17 // TX2
// hw spi pins: SCK=D18, DI(MOSI)=D23
#endif

#define REFRESH_INTERVAL 100 // 100ms = 0.1s = 10hz
#define PAGE_TIME 4000 // milliseconds, how long it displays each info page while stopped.

#define USE_MPH // comment for km/h

/*----------------------------------------------------------------------------------------
Settings for balance_leds
*/
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS 64

// Note: Forward in this instance means postive ERPM, which can actually be backwards if your motor direction is reversed.
#define LED_PIN_FORWARD 32
#define LED_PIN_BACKWARD 33
#define NUM_LEDS_FORWARD 1
#define NUM_LEDS_BACKWARD 1
#define STARTUP_FORWARD true

// Dims leds when switches are off, 1.0 is full brightness, 0.0 is off when switch is off
#define SWITCH_OFF_BRIGHTNESS_SCALE 1.0

// For full color list see http://fastled.io/docs/3.1/struct_c_r_g_b.html
#define COLOR_FORWARD CRGB::White
#define COLOR_BACKWARD CRGB::Red

/*----------------------------------------------------------------------------------------
Settings for esc
*/
#define ESC_CAN_ID 2
#define BALANCE_BUDDY_CAN_ID 3

#define CAN_RX_PIN 12
#define CAN_TX_PIN 13

// Copy these from your vesc motor config
#define MOTOR_POLES 30
#define WHEEL_DIAMETER_MM 280

// For BMS on canbus, else comment out
// #define BMS_CAN_ID 10
