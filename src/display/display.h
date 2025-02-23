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
#pragma once

#include "rritems/lclist.h"
#include "rritems/swlist.h"
#include "rritems/sensors.h"
#include <Arduino.h>
#include "menu.h"

class Display
{
    bool oldConnected;
    bool doneHostname;
    void sensor(char sensorPlace, sensorStates sensor);

  public:
    Display();
    int shiftGroup;
    void setInitial();
    void setup();
    void setupError();
    void setupConfig();
    void cls();
    void print(const char *message);
    void println(const char *message);
    void displayAll(menuT menu, subMenuT subMenu,uint8_t pbat, uint16_t vbat);
    void firmwaresetup(const char *type);
    void firmwareupdate(int percent);
    void firmwaremessage(const char *message);

    void updateClock(uint8_t hour, uint8_t minute);
    void updateSelection(const char *selection, uint32_t colour);
    void updateLc(lc *loc, int encVal);
    void clrLcSw();
    void updateLcRot(int encVal);
    void updateSw(swTypes swType, swStates swState);
    void updateCo(bool coState);
    void updateBk(bool hasSensors, sensorStates sensorPlus, sensorStates SensorMin);
    void updateSg(uint8_t aspect, uint8_t aspects);
};

extern Display display;