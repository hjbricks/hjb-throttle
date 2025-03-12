/*
MIT License

Copyright (c) 2023 Hilbert Barelds

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "display.h"

#include "DejaVu_Sans.h"
#include "DejaVu_Sans_Mono.h"
#include "esplog.h"
#include "fileconfig.h"
#include "icons/block-24.h"
#include "icons/block-invert-24.h"
#include "icons/power-ebreak-24.h"
#include "icons/signal-24.h"
#include "icons/signal-invert-24.h"
#include "icons/sun-24.h"
#include "icons/sun-invert-24.h"
#include "icons/switch-24.h"
#include "icons/switch-invert-24.h"
#include "icons/train-24.h"
#include "icons/train-invert-24.h"
#include <PubSubClient.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFi.h>

#define menuYSel 22
#define menuYPos 240 - 24
#ifdef HAVE240X240
#define menuSpeedline 22 + 40 + 42
#define menuYRest 22 + 40
#define swXoffet 0
#else
#define menuSpeedline 22 + 40 + 42 + 50
#define menuYRest 22 + 40 + 15
#define swXoffet 30
#endif

void do3aspects(uint8_t aspect);
void do4aspects(uint8_t aspect);
void do5aspects(uint8_t aspect);
void doAspectsBlack(bool top, bool middle, bool bottom);

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

// Yes I know dirty, but it is working (only read from the objects)
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;

Display::Display()
{
    setInitial();
}

void Display::print(const char *message)
{
    tft.print(message);
}
void Display::println(const char *message)
{
    tft.println(message);
}
void Display::cls()
{
    doneHostname = false;
    tft.fillScreen(TFT_BLACK);
}
void Display::setInitial()
{
    doneHostname = false;
}

void Display::setup()
{
    // init tft
    tft.init();
#ifdef HAVE240X320
    tft.setRotation(1);
#else
    tft.setRotation(2);
#endif
    tft.fillScreen(TFT_BLACK);

    // do some init screen logging
    tft.drawRect(0, 0, TFT_HEIGHT - 1, 239, TFT_GREEN);
    tft.setCursor(0, 4, 4);
    tft.setTextColor(TFT_WHITE);
    tft.println(" Initialising...\n");
    tft.setFreeFont(&FreeSans9pt7b);
    tft.println(AUTO_VERSION);
    oldConnected = false;
    shiftGroup = 0;
    setInitial();
}
void Display::firmwaresetup(const char *type)
{
    cls();
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.drawRect(0, 0, TFT_HEIGHT - 1, 239, TFT_GREEN);
    tft.setCursor(0, 30, 4);
    tft.setTextColor(TFT_WHITE);
    tft.print(" Update ");
    tft.println(type);
}

void Display::firmwaremessage(const char *message)
{
    tft.setCursor(10, 160, 4);
    tft.fillRect(10, 140, TFT_HEIGHT - 40, 30, TFT_BLACK);
    tft.print(message);
}
void Display::firmwareupdate(int percent)
{
    tft.drawRect(10, 100, TFT_HEIGHT - 20, 30, TFT_WHITE);
    tft.fillRect(10, 100, (TFT_HEIGHT - 20) * percent / 100, 30, TFT_WHITE);
}

void Display::setupError()
{
    tft.println(configuration.getErrorMessage());
}
void Display::setupConfig()
{
    // Show on splash screen
    tft.print(configuration.hostname);
    tft.print(" : ");
    tft.println(configuration.logLevel);
    // tft.println(configuration.wifiSSID);
    tft.print(configuration.mqttServer);
    tft.print(" : ");
    tft.println(configuration.mqttPort);
    // no mqtt username or password yet
    // tft.println(configuration.firmwareURL);
    tft.println(configuration.firmwareType);
    // tft.println(configuration.configURL);
}

void Display::updateClock(uint8_t hour, uint8_t minute)
{
    static uint8_t oldHour = 0, oldMinute = 0;
    if ((oldHour != hour) || (oldMinute != minute)) {
        // clock
        tft.setFreeFont(&DejaVu_Sans_Mono_20);
        char number[20];
        snprintf(number, 19, "%02d:%02d", hour, minute);

        tft.fillRect(26, menuYPos, (TFT_HEIGHT / 2) + 26, 24, TFT_BLACK); // off centre to the right
        tft.setTextDatum(TR_DATUM);
        tft.setTextColor(TFT_SKYBLUE);
        tft.drawString(number, (TFT_HEIGHT / 2) + 26, menuYPos + 1);
        tft.setTextColor(TFT_BLACK);
        tft.setTextDatum(TL_DATUM);
    }
    oldHour = hour;
    oldMinute = minute;
}

void Display::updateSelection(const char *selection, uint32_t colour)
{
    logger.vlogf(LOG_DEBUG, "Selection: '%s'", selection);
    tft.setFreeFont(&FreeSans18pt7b);
    tft.fillRect(26, menuYSel, TFT_HEIGHT - 10 - 24, 40, TFT_BLACK); // off centre to the right
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(colour);
    tft.drawString(selection, TFT_HEIGHT / 2, menuYSel + 1);
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
}

void Display::displayAll(menuT menu, subMenuT subMenu, uint8_t pbat, uint16_t vbat)
{
    static bool oldConnected = false; // for shur not true ;-)
    bool connected = mqttClient.connected();
    // connection status
    if (oldConnected != connected) {
        tft.drawRect(0, 0, 18, 18, TFT_WHITE);
        tft.drawRect(1, 1, 16, 16, TFT_WHITE);
        if (connected) {
            tft.fillRect(5, 5, 8, 8, TFT_WHITE);
        } else {
            tft.fillRect(5, 5, 8, 8, TFT_BLACK);
        }
    }
    oldConnected = connected;
    // controller name
    if (!doneHostname) {
        tft.setFreeFont(&FreeSansBold12pt7b);
        tft.drawString(configuration.hostname, 22, 0);
        doneHostname = true;
    }
    static int oldCalcRssi = 1; // for shure not true
    int calcRssi;
    if (WiFi.isConnected()) {
        long wifiRSSI = WiFi.RSSI();
        // logger.vlogf(LOG_DEBUG, "RSSI: %d", wifiRSSI);
        if (wifiRSSI >= -55) {
            // All full
            calcRssi = 5;
        } else if (wifiRSSI < -55 && wifiRSSI > -65) {
            // All but 1
            calcRssi = 4;
        } else if (wifiRSSI < -65 && wifiRSSI > -75) {
            // All but 2
            calcRssi = 3;
        } else if (wifiRSSI < -75 && wifiRSSI > -85) {
            // All but 3
            calcRssi = 2;
        } else if (wifiRSSI < -85 && wifiRSSI > -96) {
            // All but 4
            calcRssi = 1;
        } else {
            // None
            calcRssi = 0;
        }

    } else {
        calcRssi = 0;
    }
    if (calcRssi != oldCalcRssi) {
        tft.fillRect(130 + TFT_HEIGHT - 240, 0, 44, 20, TFT_BLACK);
        for (int i = 0; i < 5; i++) {
            if (i < calcRssi) {
                tft.fillRect(130 + TFT_HEIGHT - 240 + i * 9, 14 - i * 3, 8, (i + 1) * 3, TFT_WHITE);
            } else {
                tft.drawRect(130 + TFT_HEIGHT - 240 + i * 9, 14 - i * 3, 8, (i + 1) * 3, TFT_WHITE);
            }
        }
    }
    oldCalcRssi = calcRssi;

    static uint8_t oldPbat = -1;
    // do battery
    if (oldPbat != pbat) {
        // offset = 320-240
        tft.drawRect(177 + TFT_HEIGHT - 240, 0, 58, 18, TFT_WHITE);
        tft.drawRect(178 + TFT_HEIGHT - 240, 1, 56, 16, TFT_WHITE);
        tft.fillRect(235 + TFT_HEIGHT - 240, 6, 6, 8, TFT_WHITE);
        uint32_t colours[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN};
        for (int i = 0; i < 7; i++) {
            if (i * 15 >= pbat) {
                tft.fillRect(179 + TFT_HEIGHT - 240 + i * 8, 2, 7, 14, TFT_BLACK);
            } else {
                tft.fillRect(179 + TFT_HEIGHT - 240 + i * 8, 2, 7, 14, colours[i]);
            }
        }
    }
    oldPbat = pbat;

    // bottom menu (y =22, for now, moving down the more we get)
    if (menu == lcMenu) {
        tft.pushImage(0, menuYPos, 24, 24, image_data_traininvert24);
    } else {
        tft.pushImage(0, menuYPos, 24, 24, image_data_train24);
    }
    if (menu == swMenu) {
        tft.pushImage(240 - 26 + TFT_HEIGHT - 240, menuYPos, 24, 24, image_data_switchinvert24);
    } else {
        tft.pushImage(240 - 26 + TFT_HEIGHT - 240, menuYPos, 24, 24, image_data_switch24);
    }
    switch (subMenu) {
    case sgSubMenu:
        if (menu == inpMenu) {
            tft.pushImage(40 + (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_data_signalinvert24);
        } else {
            tft.pushImage(40 + (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_data_signal24);
        }
        break;
    case bkSubMenu:
        if (menu == inpMenu) {
            tft.pushImage(40 + (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_data_blockinvert24);
        } else {
            tft.pushImage(40 + (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_data_block24);
        }
        break;
    case coSubMenu:
    default:
        if (menu == inpMenu) {
            tft.pushImage(40 + (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_data_sunoff);
        } else {
            tft.pushImage(40 + (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_data_sunon);
        }
        break;
    }

    tft.pushImage(175 + TFT_HEIGHT - 240 - (TFT_HEIGHT - 240) / 3, menuYPos, 24, 24, image_power_ebreak24);
    // updateClock(0, 0);

    // restore DATUM
}

void Display::updateLcRot(int encVal)
{
    char str[15];
    tft.setFreeFont(&FreeSans12pt7b);
    tft.fillRect(140 - 60 + (TFT_HEIGHT - 240) / 2, menuSpeedline, 60, 22, TFT_BLACK); // off centre to the right
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TR_DATUM);
    snprintf(str, 10, "%d", encVal);
    tft.drawString(str, 140 + (TFT_HEIGHT - 240) / 2, menuSpeedline);
}

void Display::clrLcSw()
{
    tft.fillRect(0, menuYRest - 15, TFT_HEIGHT, 145, TFT_BLACK); // off centre to the right
}

void Display::updateLc(lc *loc, int encVal)
{
    char str[15];
    clrLcSw();
#ifdef HAVE240X240
    tft.setFreeFont(&FreeSans18pt7b);
#else
    tft.setFreeFont(&DejaVu_Sans_54);
#endif
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_WHITE);
    snprintf(str, 10, "%d", abs(loc->V));
    tft.drawString(str, 140 + (TFT_HEIGHT - 240) / 2, menuYRest + 1);
    tft.setTextDatum(TL_DATUM);
    if (loc->V < 0)
        tft.drawString("<", 0, menuYRest + 1);
    tft.setTextDatum(TR_DATUM);
    if (loc->V > 0)
        tft.drawString(">", TFT_HEIGHT, menuYRest + 1);

    updateLcRot(encVal);
    snprintf(str, 14, "%d %s", loc->Vmax, (loc->vModePercent ? "%" : "kmh"));
    tft.drawString(str, TFT_HEIGHT, menuSpeedline);

    tft.setTextDatum(TL_DATUM);
    tft.drawString(loc->addr, 0, menuSpeedline);

    int gap = 0;
    int offset;
#ifdef HAVE240X240
    tft.fillCircle(220, menuYSel + 20, 10, TFT_BLACK);
    if (loc->fn[0]) {
        tft.fillCircle(220, menuYSel + 20, 10, TFT_YELLOW);
    } else {
        tft.drawCircle(220, menuYSel + 20, 10, TFT_YELLOW);
    }
    uint32_t colour;
    for (int i = 1; i < 9; i++) {
        if (i > 4) {
            gap = 17;
        }
        offset = 240 / 10 * i + 5;
        offset += gap;
        if ((shiftGroup == 0 && i < 5) || (shiftGroup == 1 && i > 4)) {
            colour = TFT_GREEN;
        } else {
            colour = TFT_BLUE;
        }
        if (loc->fn[i]) {
            tft.fillCircle(offset, menuYRest + 82, 10, colour);
        } else {
            tft.drawCircle(offset, menuYRest + 82, 10, colour);
        }
        if ((shiftGroup == 2 && i < 5) || (shiftGroup == 3 && i > 4)) {
            colour = TFT_GREEN;
        } else {
            colour = TFT_BLUE;
        }
        if (loc->fn[i + 8]) {
            tft.fillCircle(offset, menuYRest + 82 + 32, 10, colour);
        } else {
            tft.drawCircle(offset, menuYRest + 82 + 32, 10, colour);
        }
    }
#else
    uint32_t colour;
    div_t result;
    tft.fillCircle(8, menuYRest + 82 + 40, 8, TFT_BLACK);
    if (loc->fn[0].fn) {
        tft.fillCircle(8, menuYRest + 82 + 40, 8, TFT_YELLOW);
    } else {
        tft.drawCircle(8, menuYRest + 82 + 40, 8, TFT_YELLOW);
    }
    for (int i = 1; i < 17; i++) {
        result = div(i - 1, 4);
        gap = 17 * result.quot;
        offset = 15 + (TFT_HEIGHT - 20) / 20 * i;
        offset += gap;
        if (shiftGroup == result.quot) {
            colour = TFT_GREEN;
        } else {
            colour = TFT_BLUE;
        }
        if (loc->fn[i].fn) {
            tft.fillCircle(offset, menuYRest + 82 + 40, 8, colour);
        } else {
            tft.drawCircle(offset, menuYRest + 82 + 40, 8, colour);
        }
    }
#endif
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
}

void drawTripple(swStates swState)
{
    uint32_t colour;
    tft.fillRect(60 + swXoffet, menuYRest + 60, 120 + swXoffet, 20, TFT_WHITE);
    // straight
    if (swState == stStraight) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillRect(90 + swXoffet, menuYRest + 62, 60 + swXoffet, 16, colour);

    tft.fillTriangle(150 + swXoffet, menuYRest + 60, 165 + swXoffet, menuYRest + 45, 180 + swXoffet, menuYRest + 60, TFT_WHITE);
    tft.fillTriangle(120 + swXoffet, menuYRest + 30, 105 + swXoffet, menuYRest + 15, 135 + swXoffet, menuYRest + 15, TFT_WHITE);

    // right
    if (swState == stRight) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillTriangle(120 + swXoffet, menuYRest + 30, 135 + swXoffet, menuYRest + 15, 150 + swXoffet, menuYRest + 60, colour);
    tft.fillTriangle(150 + swXoffet, menuYRest + 60, 135 + swXoffet, menuYRest + 15, 165 + swXoffet, menuYRest + 45, colour);

    tft.drawLine(120 + swXoffet, menuYRest + 30, 150 + swXoffet, menuYRest + 60, TFT_WHITE);
    tft.drawLine(120 + swXoffet, menuYRest + 29, 150 + swXoffet, menuYRest + 59, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 15, 165 + swXoffet, menuYRest + 45, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 16, 165 + swXoffet, menuYRest + 46, TFT_WHITE);

    tft.fillTriangle(150 + swXoffet, menuYRest + 80, 165 + swXoffet, menuYRest + 95, 180 + swXoffet, menuYRest + 80, TFT_WHITE);
    tft.fillTriangle(120 + swXoffet, menuYRest + 110, 105 + swXoffet, menuYRest + 125, 135 + swXoffet, menuYRest + 125, TFT_WHITE);

    // left
    if (swState == stLeft) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillTriangle(150 + swXoffet, menuYRest + 80, 165 + swXoffet, menuYRest + 95, 135 + swXoffet, menuYRest + 125, colour);
    tft.fillTriangle(150 + swXoffet, menuYRest + 80, 120 + swXoffet, menuYRest + 110, 135 + swXoffet, menuYRest + 125, colour);

    tft.drawLine(120 + swXoffet, menuYRest + 110, 150 + swXoffet, menuYRest + 80, TFT_WHITE);
    tft.drawLine(120 + swXoffet, menuYRest + 111, 150 + swXoffet, menuYRest + 81, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 125, 165 + swXoffet, menuYRest + 95, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 126, 165 + swXoffet, menuYRest + 94, TFT_WHITE);
}
void drawSwRight(swStates swState)
{
    uint32_t colour;
    tft.fillRect(60 + swXoffet, menuYRest + 60, 120 + swXoffet, 20, TFT_WHITE);
    if (swState == stStraight) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillRect(90 + swXoffet, menuYRest + 62, 60 + swXoffet, 16, colour);

    tft.fillTriangle(150 + swXoffet, menuYRest + 60, 165 + swXoffet, menuYRest + 45, 180 + swXoffet, menuYRest + 60, TFT_WHITE);
    tft.fillTriangle(120 + swXoffet, menuYRest + 30, 105 + swXoffet, menuYRest + 15, 135 + swXoffet, menuYRest + 15, TFT_WHITE);

    // upper branch
    if (swState == stTurnout) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillTriangle(120 + swXoffet, menuYRest + 30, 135 + swXoffet, menuYRest + 15, 150 + swXoffet, menuYRest + 60, colour);
    tft.fillTriangle(150 + swXoffet, menuYRest + 60, 135 + swXoffet, menuYRest + 15, 165 + swXoffet, menuYRest + 45, colour);

    tft.drawLine(120 + swXoffet, menuYRest + 30, 150 + swXoffet, menuYRest + 60, TFT_WHITE);
    tft.drawLine(120 + swXoffet, menuYRest + 29, 150 + swXoffet, menuYRest + 59, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 15, 165 + swXoffet, menuYRest + 45, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 16, 165 + swXoffet, menuYRest + 46, TFT_WHITE);
}
void drawSwLeft(swStates swState)
{
    uint32_t colour;
    tft.fillRect(60 + swXoffet, menuYRest + 60, 120 + swXoffet, 20, TFT_WHITE);
    if (swState == stStraight) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillRect(90 + swXoffet, menuYRest + 62, 60 + swXoffet, 16, colour);

    tft.fillTriangle(60 + swXoffet, menuYRest + 60, 75 + swXoffet, menuYRest + 45, 90 + swXoffet, menuYRest + 60, TFT_WHITE);
    tft.fillTriangle(120 + swXoffet, menuYRest + 30, 105 + swXoffet, menuYRest + 15, 135 + swXoffet, menuYRest + 15, TFT_WHITE);
    if (swState == stTurnout) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillTriangle(75 + swXoffet, menuYRest + 45, 105 + swXoffet, menuYRest + 15, 90 + swXoffet, menuYRest + 60, colour);
    tft.fillTriangle(105 + swXoffet, menuYRest + 15, 120 + swXoffet, menuYRest + 30, 90 + swXoffet, menuYRest + 60, colour);

    tft.drawLine(75 + swXoffet, menuYRest + 45, 105 + swXoffet, menuYRest + 15, TFT_WHITE);
    tft.drawLine(75 + swXoffet, menuYRest + 44, 105 + swXoffet, menuYRest + 14, TFT_WHITE);
    tft.drawLine(90 + swXoffet, menuYRest + 60, 120 + swXoffet, menuYRest + 30, TFT_WHITE);
    tft.drawLine(90 + swXoffet, menuYRest + 59, 120 + swXoffet, menuYRest + 29, TFT_WHITE);
}
void drawSwDouble(swStates swState)
{
    uint32_t colour;
    tft.fillRect(00 + swXoffet, menuYRest + 60, 120 + swXoffet, 20, TFT_WHITE);
    tft.fillRect(120 + swXoffet, menuYRest + 60, 120 + swXoffet, 20, TFT_WHITE);
    if (swState == stStraight || swState == stRight) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillRect(30 + swXoffet, menuYRest + 62, 60 + swXoffet, 16, colour);
    if (swState == stStraight || swState == stLeft) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillRect(150 + swXoffet, menuYRest + 62, 60 + swXoffet, 16, colour);

    tft.fillTriangle(120 + swXoffet, menuYRest + 60, 135 + swXoffet, menuYRest + 45, 150 + swXoffet, menuYRest + 60, TFT_WHITE);
    tft.fillTriangle(165 + swXoffet, menuYRest + 15, 180 + swXoffet, menuYRest + 30, 195 + swXoffet, menuYRest + 15, TFT_WHITE);
    tft.fillTriangle(90 + swXoffet, menuYRest + 80, 120 + swXoffet, menuYRest + 80, 105 + swXoffet, menuYRest + 95, TFT_WHITE);
    tft.fillTriangle(60 + swXoffet, menuYRest + 110, 45 + swXoffet, menuYRest + 125, 75 + swXoffet, menuYRest + 125, TFT_WHITE);
    if (swState == stTurnout || swState == stRight) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillTriangle(135 + swXoffet, menuYRest + 45, 165 + swXoffet, menuYRest + 15, 150 + swXoffet, menuYRest + 60, colour);
    tft.fillTriangle(165 + swXoffet, menuYRest + 15, 180 + swXoffet, menuYRest + 30, 150 + swXoffet, menuYRest + 60, colour);

    tft.drawLine(135 + swXoffet, menuYRest + 45, 165 + swXoffet, menuYRest + 15, TFT_WHITE);
    tft.drawLine(135 + swXoffet, menuYRest + 44, 165 + swXoffet, menuYRest + 14, TFT_WHITE);
    tft.drawLine(150 + swXoffet, menuYRest + 60, 180 + swXoffet, menuYRest + 30, TFT_WHITE);
    tft.drawLine(150 + swXoffet, menuYRest + 59, 180 + swXoffet, menuYRest + 29, TFT_WHITE);

    if (swState == stTurnout || swState == stLeft) {
        colour = TFT_ORANGE;
    } else {
        colour = TFT_DARKGREY;
    }
    tft.fillTriangle(90 + swXoffet, menuYRest + 80, 105 + swXoffet, menuYRest + 95, 60 + swXoffet, menuYRest + 110, colour);
    tft.fillTriangle(105 + swXoffet, menuYRest + 95, 60 + swXoffet, menuYRest + 110, 75 + swXoffet, menuYRest + 125, colour);
    tft.drawLine(60 + swXoffet, menuYRest + 110, 90 + swXoffet, menuYRest + 80, TFT_WHITE);
    tft.drawLine(60 + swXoffet, menuYRest + 111, 90 + swXoffet, menuYRest + 81, TFT_WHITE);
    tft.drawLine(75 + swXoffet, menuYRest + 125, 105 + swXoffet, menuYRest + 95, TFT_WHITE);
    tft.drawLine(75 + swXoffet, menuYRest + 124, 105 + swXoffet, menuYRest + 94, TFT_WHITE);
}
void drawDecoupler(swStates swState)
{
    uint32_t colour;
    tft.fillRect(60 + swXoffet, menuYRest + 60, 120 + swXoffet, 20, TFT_WHITE);
    tft.fillRect(90 + swXoffet, menuYRest + 62, 60 + swXoffet, 16, TFT_DARKGREY);
    if (swState == stTurnout) {
        colour = TFT_CYAN;
    } else {
        colour = TFT_RED;
    }
    tft.fillRect(90 + swXoffet, menuYRest + 50, 60 + swXoffet, 10, colour);
    tft.fillRect(90 + swXoffet, menuYRest + 81, 60 + swXoffet, 10, colour);

    tft.drawRect(90 + swXoffet, menuYRest + 48, 60 + swXoffet, 14, TFT_WHITE);
    tft.drawRect(91 + swXoffet, menuYRest + 49, 58 + swXoffet, 12, TFT_WHITE);
    tft.drawRect(90 + swXoffet, menuYRest + 79, 60 + swXoffet, 14, TFT_WHITE);
    tft.drawRect(91 + swXoffet, menuYRest + 80, 58 + swXoffet, 12, TFT_WHITE);
}

void Display::updateSw(swTypes swType, swStates swState)
{
    uint32_t colour;
    switch (swType) {
    case swTriple: // three way switch
        drawTripple(swState);
        break;
    case swRight:
        drawSwRight(swState);
        break;
    case swLeft:
        drawSwLeft(swState);
        break;
    case swDouble:
        drawSwDouble(swState);
        break;
    case swDecoupler:
        drawDecoupler(swState);
    }
}
void Display::updateCo(bool coState)
{
    uint32_t colour;
    tft.drawRect(TFT_HEIGHT / 2 - 50, menuYRest, 100, 100, TFT_WHITE);
    if (coState) {
        colour = TFT_GREEN;
    } else {
        colour = TFT_RED;
    }
    tft.fillRect(TFT_HEIGHT / 2 - 30, menuYRest + 20, 60, 60, colour);
}

void Display::sensor(char sensorPlace, sensorStates sensor)
{
    int x_pos = 0;
    x_pos = (sensorPlace == '+') ? (swXoffet) : (TFT_HEIGHT / 2 + 50);
    switch (sensor) {
    case sensor_on:
        tft.fillCircle(x_pos + 40, menuYRest + 50, 15, TFT_RED);
        break;
    case sensor_off:
        tft.fillCircle(x_pos + 40, menuYRest + 50, 15, TFT_GREEN);
        break;
    case sensor_undefined: // FALL THROUGH
    default:
        break;
    }
}
void Display::updateBk(bool hasSensors, sensorStates sensorPlus, sensorStates sensorMinus)
{
    uint32_t colour;
    tft.drawRect(TFT_HEIGHT / 2 - 50, menuYRest, 100, 100, TFT_WHITE);
    tft.drawLine(swXoffet, menuYRest + 50, TFT_HEIGHT / 2 - 50, menuYRest + 50, TFT_WHITE);
    tft.drawLine(TFT_HEIGHT / 2 + 50, menuYRest + 50, TFT_HEIGHT - swXoffet, menuYRest + 50, TFT_WHITE);

    tft.setFreeFont(&DejaVu_Sans_Mono_20);
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("+", (TFT_HEIGHT / 2) - 50 + 20, menuYRest + 10);

    // + sensor side: assumprion, first name is +, second is - side of the block
    if (hasSensors) {
        tft.drawCircle(swXoffet + 40, menuYRest + 50, 20, TFT_WHITE);
        tft.drawCircle(TFT_HEIGHT / 2 + 50 + 40, menuYRest + 50, 20, TFT_WHITE);

        tft.drawLine(swXoffet + 40 - 19, menuYRest + 50, swXoffet + 40 + 19, menuYRest + 50, TFT_BLACK);
        sensor('+', sensorPlus);

        // - sensor side
        tft.drawLine(TFT_HEIGHT / 2 + 50 + 40 - 19, menuYRest + 50, TFT_HEIGHT / 2 + 50 + 40 + 19, menuYRest + 50, TFT_BLACK);
        sensor('-', sensorMinus);
    }
}
void Display::updateSg(uint8_t aspect, uint8_t aspects)
{
    tft.drawRoundRect(TFT_HEIGHT / 2 - 45, menuYRest - 15, 90, 140, 45, TFT_WHITE);
    tft.drawRoundRect(TFT_HEIGHT / 2 - 44, menuYRest - 14, 88, 138, 44, TFT_WHITE);

    if (aspects == 2) {
        /*
        2 aspects
        2:
        red
        green
        */
        tft.drawCircle(TFT_HEIGHT / 2, menuYRest + 80, 15, TFT_WHITE);
        tft.drawCircle(TFT_HEIGHT / 2, menuYRest + 30, 15, TFT_WHITE);
        if (aspect == 0) {
            // bottom red
            tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 30, 12, TFT_BLACK); // top
            tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 80, 12, TFT_RED);   // bottom
        } else {
            // top green
            tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 30, 12, TFT_GREEN); // top
            tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 80, 12, TFT_BLACK); // bottom
        }
    } else {
        tft.drawCircle(TFT_HEIGHT / 2, menuYRest + 90, 15, TFT_WHITE);
        tft.drawCircle(TFT_HEIGHT / 2, menuYRest + 52, 15, TFT_WHITE);
        tft.drawCircle(TFT_HEIGHT / 2, menuYRest + 15, 15, TFT_WHITE);
        // 3 or more aspects
        switch (aspects) {
        case 3:
            do3aspects(aspect);
            break;
        case 4:
            do4aspects(aspect);
            break;
        case 5:
            do5aspects(aspect);
            break;
        }
    }
}
/*
3:
         0 red     -> middle
         1 green   -> top
         2 yellow  -> bottom
*/
void do3aspects(uint8_t aspect)
{
    switch (aspect) {
    case 0:
        doAspectsBlack(true, false, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 52, 12, TFT_RED); // middle
        break;
    case 1:
        doAspectsBlack(false, true, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_GREEN); // top
        break;
    case 2:
        doAspectsBlack(true, true, false);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 90, 12, TFT_YELLOW); // bottom
        break;
    }
}
/*
        4:
         0 red             -> middle
         1 green           -> top
         2 yellow          -> bottom
         3 green yellow    -> top, bottom
*/
void do4aspects(uint8_t aspect)
{
    switch (aspect) {
    case 0:
        doAspectsBlack(true, false, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 52, 12, TFT_RED); // middle
        break;
    case 1:
        doAspectsBlack(false, true, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_GREEN); // top
        break;
    case 3:
        doAspectsBlack(false, true, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_GREEN);  // top
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 90, 12, TFT_YELLOW); // bottom
        break;
    case 2:
        doAspectsBlack(true, true, false);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 90, 12, TFT_YELLOW); // bottom
        break;
    }
}
/*
        5:
         0 red             -> middle
         1 green           -> top
         2 green yellow    -> top, bottom
         3 red white white -> top, middle, bottom
         4 white           -> middle
         */
void do5aspects(uint8_t aspect)
{
    switch (aspect) {
    case 0:
        doAspectsBlack(true, false, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 52, 12, TFT_RED); // middle
        break;
    case 1:
        doAspectsBlack(false, true, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_GREEN); // top
        break;
    case 2:
        doAspectsBlack(false, true, false);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_GREEN);  // top
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 90, 12, TFT_YELLOW); // bottom
        break;
    case 3:
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_RED);   // top
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 52, 12, TFT_WHITE); // middle
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 90, 12, TFT_WHITE); // bottom
        break;
    case 4:
        doAspectsBlack(true, false, true);
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 52, 12, TFT_WHITE); // middle
        break;
    }
}
void doAspectsBlack(bool top, bool middle, bool bottom)
{
    if (top)
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 15, 12, TFT_BLACK); // top
    if (middle)
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 52, 12, TFT_BLACK); // middle
    if (bottom)
        tft.fillCircle(TFT_HEIGHT / 2, menuYRest + 90, 12, TFT_BLACK); // bottom
}

Display display = Display();