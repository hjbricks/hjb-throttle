#pragma once

/*
    This files holds all configuration stuff, pins and defines
*/

// PINS
// V_BAT_PIN 36 -> moved to platformio.ini
// TFT PINS, defined in platformio.ini
// TFT_MOSI     23
// TFT_SCLK     18
// TFT_DC       2
// TFT_RST      4

// for esp 32 use gpio25, 26,27 for the rotary encoder
#define PIN_A 25        // yellow    clk pin
#define PIN_B 26        // blue      dt  pin
#define BUTTON 27       // white     sw  pin
// menu buttons

#define PIN_MENU_1 32   // white
#define PIN_MENU_2 33   // blue
#define PIN_MENU_3 34   // yelow
#define PIN_MENU_4 35   // green

#define PIN_STOP 5
#define PIN_SHIFT 15
#define PIN_FN0   13
#define PIN_FN1   14
#define PIN_FN2   19
#define PIN_FN3   17
#define PIN_FN4   16

// mqtt defines
#define MQTT_CONNECTION_INTERVAL 3000
#define MQTT_KEEP_ALIVE_INTERVAL 10