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

/*
    GPIO36 ADC1   +--[47K]--GPIO36--[100K]-GND
    9 bits, max value = 512 = 3.3V
    434/512*3.3
*/
#include "AiEsp32RotaryEncoder.h"
#include "Button.h"
#include "Free_Fonts.h"
#include "config.h"
#include "display/display.h"
#include "esplog.h"
#include "fileconfig.h"
#include "firmwareupdate.h"
#include "mqtt/mqtt.h"
#include "rrbattery.h"
#include "rritems/bklist.h"
#include "rritems/colist.h"
#include "rritems/lclist.h"
#include "rritems/sglist.h"
#include "rritems/swlist.h"
#include <ArduinoOTA.h>

#include "menu.h"
#include <TFT_eSPI.h>
#include <WiFi.h>

#include "state.h"

// RotaryFullStep encoder(PIN_A, PIN_B);
// Button rotaryButton(BUTTON);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(PIN_B, PIN_A, BUTTON, -1, 4);
Button menuButtonTrains(PIN_MENU_1);
Button menuButtonOutput(PIN_MENU_2);
Button menuButtonEBreak(PIN_MENU_3);
Button menuButtonSwitch(PIN_MENU_4);

Button menuButtonStop(PIN_STOP);
Button menuButtonShift(PIN_SHIFT);
Button menuButtonFN0(PIN_FN0);
Button menuButtonFN1(PIN_FN1);
Button menuButtonFN2(PIN_FN2);
Button menuButtonFN3(PIN_FN3);
Button menuButtonFN4(PIN_FN4);

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void reconnect()
{
    logger.log(LOG_DEBUG, "Connecting to MQTT Broker...");
    char *willMessage;

    willMessage = (char *)malloc(100);
    snprintf(willMessage, 100, "<info msg=\"contoller_disconnected\" source=\"lastwill\" mc=\"%s\"/>", configuration.hostname);
    while (!mqttClient.connected()) {

        if (mqttClient.connect(configuration.hostname, MQTT_COMMAND, 0, true, willMessage)) {
            logger.log(LOG_DEBUG, "Connected.");
            mqttClient.subscribe(MQTT_INFO); // for now this should be enough
            mqttClient.subscribe(MQTT_COMMAND);
        }
    }
    free(willMessage);
}

void startFWUpdate(int size, const char *type)
{
    logger.vlogf(LOG_INFO, "Updating %s...", type);
    display.firmwaresetup(type);
}
void updateFWUpdate(int size, int currentSize)
{
    Serial.print(".");
    display.firmwareupdate(currentSize * 100 / size);
}

void doRotaryButton()
{
    static bool doneSelectMenu = false;
    static int operationItem = 0;
    static unsigned long lastTimeChanged = 0;
    static bool valueChanged = false;
    static long encVal;
    // dont print anything unless value changed
    if (rotaryEncoder.encoderChanged()) {
        encVal = rotaryEncoder.readEncoder();
        logger.vlogf(LOG_DEBUG, "We have rotary, value %d", encVal);
        if (state.getSelectMenu()) {
            if (state.getMenu() == lcMenu) {
                logger.vlogf(LOG_DEBUG, "Loc enc value %d max: %d", encVal, locs.size());
                if (locs.size() > 0) {
                    if (encVal > locs.size())
                        encVal = locs.size() - 1;
                    logger.vlogf(LOG_DEBUG, "Loc enc value %d", encVal);
                    display.updateSelection(locs[encVal]->id, TFT_YELLOW);
                }
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == coSubMenu) {
                logger.vlogf(LOG_DEBUG, "Co enc value %d max: %d", encVal, outputs.size());
                if (outputs.size() > 0) {
                    if (encVal > outputs.size())
                        encVal = outputs.size() - 1;
                    logger.vlogf(LOG_DEBUG, "Co enc value %d", encVal);
                    display.updateSelection(outputs[encVal]->id, TFT_YELLOW);
                }
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == bkSubMenu) {
                logger.vlogf(LOG_DEBUG, "Bk enc value %d max: %d", encVal, blocks.size());
                if (blocks.size() > 0) {
                    if (encVal > blocks.size())
                        encVal = blocks.size() - 1;
                    logger.vlogf(LOG_DEBUG, "Bk enc value %d", encVal);
                    display.updateSelection(blocks[encVal]->id, TFT_YELLOW);
                }
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == sgSubMenu) {
                logger.vlogf(LOG_DEBUG, "Sg enc value %d max: %d", encVal, signals.size());
                if (signals.size() > 0) {
                    if (encVal > signals.size())
                        encVal = signals.size() - 1;
                    logger.vlogf(LOG_DEBUG, "Sg enc value %d", encVal);
                    display.updateSelection(signals[encVal]->id, TFT_YELLOW);
                }
            }
            if (state.getMenu() == swMenu) {
                logger.vlogf(LOG_DEBUG, "Switch enc value %d max: %d", encVal, switches.size());
                if (switches.size() > 0) {
                    if (encVal > switches.size())
                        encVal = switches.size() - 1;
                    logger.vlogf(LOG_DEBUG, "Switch enc value %d", encVal);
                    display.updateSelection(switches[encVal]->id, TFT_YELLOW);
                }
            }
        } else {
            // select a submenu when in coMenu
            if (menuButtonOutput.read() == 0) {
                switch (state.getSubMenu()) {
                case coSubMenu:
                    state.setSubMenu(bkSubMenu);
                    break;
                case bkSubMenu:
                    state.setSubMenu(sgSubMenu);
                    break;
                case sgSubMenu:
                    state.setSubMenu(coSubMenu);
                    break;
                default:
                    break;
                }
            }
            if (state.getMenu() == lcMenu && currentLc->id)
                display.updateLcRot(encVal); // show a small rotaty value when selecting the speed
            // got a rotaty change
            valueChanged = true;
            lastTimeChanged = millis();
        }
    }
    // no change, if we do loco's wait some milli's and send the update, don't do
    // at the change, we get a feedbackloop
    if (valueChanged) {
        if (currentLc->id && state.getMenu() == lcMenu && millis() - lastTimeChanged > 200) {
            currentLc->newSpeed = encVal;
            publishLocSpeed(encVal);
            logger.vlogf(LOG_DEBUG, "value %d", encVal);
            valueChanged = false;
        }
    }

    if (rotaryEncoder.isEncoderButtonClicked()) {
        static unsigned long lastTimePressed = 0;
        // ignore multiple press in that time milliseconds
        if (millis() - lastTimePressed < 500) {
            return;
        }
        lastTimePressed = millis();
        // toggle between selectig a loco or switch and operating it.
        state.invertMenuSelection();
        logger.vlogf(LOG_DEBUG, "Button pressed inmenu: %d menu: %d (%d) rotval: %d", state.getSelectMenu(), state.getMenu(), state.getSubMenu(), operationItem);
        if (state.getSelectMenu()) {
            // go into the select menu
            if (state.getMenu() == lcMenu && locs.size() > 0) {
                // setup for loco's
                rotaryEncoder.setBoundaries(0, locs.size() - 1, true);
                if (currentLc->id) {
                    // find the one in the list
                    std::vector<lc *>::iterator itr = std::find(locs.begin(), locs.end(), currentLc);

                    if (itr != locs.end()) { // Found the item
                        operationItem = std::distance(locs.begin(), itr);
                    } else {
                        operationItem = 0;
                    }
                } else {
                    operationItem = 0;
                }
                rotaryEncoder.setEncoderValue(operationItem);
                display.clrLcSw();
                display.updateSelection(locs[operationItem]->id, TFT_YELLOW);
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == coSubMenu && outputs.size() > 0) {
                // if (menu == coMenu && outputs.size() > 0) {
                //  setup for outputs
                rotaryEncoder.setBoundaries(0, outputs.size() - 1, true);
                if (currentCo->id) {
                    // find the one in the list
                    std::vector<co *>::iterator itr = std::find(outputs.begin(), outputs.end(), currentCo);

                    if (itr != outputs.end()) { // Found the item
                        operationItem = std::distance(outputs.begin(), itr);
                    } else {
                        operationItem = 0;
                    }
                } else {
                    operationItem = 0;
                }
                logger.vlogf(LOG_DEBUG, "Co uper value %d max: %d", operationItem, outputs.size());
                rotaryEncoder.setEncoderValue(operationItem);
                display.clrLcSw();
                display.updateSelection(outputs[operationItem]->id, TFT_YELLOW);
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == bkSubMenu && blocks.size() > 0) {
                // if (menu == coMenu && outputs.size() > 0) {
                //  setup for outputs
                rotaryEncoder.setBoundaries(0, blocks.size() - 1, true);
                if (currentBk->id) {
                    // find the one in the list
                    std::vector<bk *>::iterator itr = std::find(blocks.begin(), blocks.end(), currentBk);

                    if (itr != blocks.end()) { // Found the item
                        operationItem = std::distance(blocks.begin(), itr);
                    } else {
                        operationItem = 0;
                    }
                } else {
                    operationItem = 0;
                }
                logger.vlogf(LOG_DEBUG, "Block uper value %d max: %d", operationItem, blocks.size());
                rotaryEncoder.setEncoderValue(operationItem);
                display.clrLcSw();
                display.updateSelection(blocks[operationItem]->id, TFT_YELLOW);
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == sgSubMenu && signals.size() > 0) {
                // if (menu == coMenu && outputs.size() > 0) {
                //  setup for outputs
                rotaryEncoder.setBoundaries(0, signals.size() - 1, true);
                if (currentSg->id) {
                    // find the one in the list
                    std::vector<sg *>::iterator itr = std::find(signals.begin(), signals.end(), currentSg);

                    if (itr != signals.end()) { // Found the item
                        operationItem = std::distance(signals.begin(), itr);
                    } else {
                        operationItem = 0;
                    }
                } else {
                    operationItem = 0;
                }
                logger.vlogf(LOG_DEBUG, "Block uper value %d max: %d", operationItem, signals.size());
                rotaryEncoder.setEncoderValue(operationItem);
                display.clrLcSw();
                display.updateSelection(signals[operationItem]->id, TFT_YELLOW);
            }

            if (state.getMenu() == swMenu && switches.size() > 0) {
                // setup for switches
                rotaryEncoder.setBoundaries(0, switches.size() - 1, true);
                if (currentSw->id) {
                    // find the one in the list
                    std::vector<sw *>::iterator itr = std::find(switches.begin(), switches.end(), currentSw);

                    if (itr != switches.end()) { // Found the item
                        operationItem = std::distance(switches.begin(), itr);
                    } else {
                        operationItem = 0;
                    }
                } else {
                    operationItem = 0;
                }
                logger.vlogf(LOG_DEBUG, "Switch uper value %d max: %d", operationItem, switches.size());
                rotaryEncoder.setEncoderValue(operationItem);
                display.clrLcSw();
                display.updateSelection(switches[operationItem]->id, TFT_YELLOW);
            }
        } else {
            // we have selected a loc or switch
            operationItem = rotaryEncoder.readEncoder();
            logger.vlogf(LOG_DEBUG, "Rotval: %d", operationItem);
            if (state.getMenu() == lcMenu && locs.size() > 0) {
                if (operationItem > locs.size()) {
                    operationItem = 0;
                }
                currentLc->setIdandAddr(locs[operationItem]->id, locs[operationItem]->addr);
                currentSw->clear();
                currentCo->clear();
                currentBk->clear();
                currentSg->clear();
                display.updateSelection(currentLc->id, TFT_WHITE);
                publishGetLocInfo(currentLc->id);
                publishLoc(); // get fns from this loc
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == coSubMenu && outputs.size() > 0) {
                if (operationItem > outputs.size()) {
                    operationItem = 0;
                }
                currentCo->setId(outputs[operationItem]->id);
                currentLc->clear();
                currentSw->clear();
                currentBk->clear();
                currentSg->clear();
                display.updateSelection(currentCo->id, TFT_WHITE);
                display.clrLcSw();
                publishGetOutputInfo(currentCo->id);
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == bkSubMenu && blocks.size() > 0) {
                if (operationItem > blocks.size()) {
                    operationItem = 0;
                }
                currentBk->setId(blocks[operationItem]->id);
                currentLc->clear();
                currentSw->clear();
                currentCo->clear();
                currentSg->clear();
                display.updateSelection(currentBk->id, TFT_WHITE);
                display.clrLcSw();
                publishGetBlockInfo(currentBk->id);
            }
            if (state.getMenu() == inpMenu && state.getSubMenu() == sgSubMenu && signals.size() > 0) {
                if (operationItem > signals.size()) {
                    operationItem = 0;
                }
                currentSg->setId(signals[operationItem]->id);
                currentLc->clear();
                currentSw->clear();
                currentCo->clear();
                currentBk->clear();
                display.updateSelection(currentSg->id, TFT_WHITE);
                display.clrLcSw();
                publishGetSignalInfo(currentSg->id);
            }
            if (state.getMenu() == swMenu && switches.size() > 0) {
                if (operationItem > switches.size()) {
                    operationItem = 0;
                }
                currentSw->setId(switches[operationItem]->id);
                currentLc->clear();
                currentCo->clear();
                currentBk->clear();
                currentSg->clear();
                display.updateSelection(currentSw->id, TFT_WHITE);
                display.clrLcSw();
                publishGetSwitchInfo(currentSw->id);
            }
        }
    }
}

void setup(void)
{
    Serial.begin(115200);
    delay(100);                   // Wait a moment to start (so we don't miss Serial output).
    logger.setup(true, LOG_INFO); // for nor LOG

    logger.vlogf(LOG_INFO, "RRMaus version %s", AUTO_VERSION);
    logger.log(LOG_INFO, "Started setup.");
#ifdef HAVE240X240
    pinMode(TFT__BLK, OUTPUT);    // set pin to input
    digitalWrite(TFT__BLK, HIGH); // turn on pullup resistors
#endif
    display.setup();

    liionbat.setup();

    if (!configuration.readConfig()) {
        display.setupError();
    } else {
        display.setupConfig();
        logger.setLoglevel(configuration.logLevel);
    }

    WiFi.setHostname(configuration.hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(configuration.wifiSSID, configuration.wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        display.print(".");
        delay(1000);
    }
    logger.vlogf(LOG_INFO, "Connected to wifi (%d).", (WiFi.status() == WL_CONNECTED));
    display.print("WIFI.");
    mqttClient.setServer(configuration.mqttServer, configuration.mqttPort);
    mqttClient.setBufferSize(16384); // the <lclist command can be quite large!
    mqttClient.setKeepAlive(MQTT_KEEP_ALIVE_INTERVAL);
    reconnect();
    display.print("MQTT.");

    ArduinoOTA.onStart([]() {
        Serial.println("Start");
        const char *type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "firmware";
        else { // U_SPIFFS
            type = "filesystem";
            ESPFS.end();
        }
        startFWUpdate(0, type);
    });

    if (configuration.OTAPassword) {
        ArduinoOTA.setPassword(configuration.OTAPassword);
    }

    ArduinoOTA.onEnd([]() {
        Serial.println("\nSuccess!");
        display.firmwaremessage("Success!");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        updateFWUpdate(total, progress);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
            display.firmwaremessage("Auth Failed!");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
            display.firmwaremessage("Begin Failed!");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
            display.firmwaremessage("Connect Failed!");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
            display.firmwaremessage("Receive Failed!");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
            display.firmwaremessage("End Failed!");
        }
        delay(1000); // Wait a second
        display.cls();
    });
    ArduinoOTA.begin();
    display.print("Ck");
    // check for new config file, only if there is an url AND firmwaretype
    if (configuration.firmwareURL != NULL && configuration.firmwareType != NULL) {
        fwupdate.setup(configuration.firmwareURL, configuration.firmwareType, AUTO_VERSION);
        // check for firmware update
        version_t fwVersion = fwupdate.checkversion();
        if (fwVersion.major != 0 || fwVersion.minor != 0 || fwVersion.commit != 0) {
            logger.vlogf(LOG_INFO, "Found new frimware version %d.%d-%d!", fwVersion.major, fwVersion.minor, fwVersion.commit);
            fwupdate.setCallbackFunctions(&startFWUpdate, &updateFWUpdate);
            fwupdate.updateFirmware();
        }
    }
    // check for a new config file, only the configURL and hostname are set
    if (configuration.configURL != NULL && configuration.hostname != NULL) {
        if (fwupdate.updateConfigFile(configuration.configURL, configuration.hostname, CONFIG_VERSION, "config")) {
            // reload new config
            if (!configuration.readConfig()) {
                display.setupError();
            } else {
                display.setupConfig();
                logger.setLoglevel(configuration.logLevel);
            }
        }
    }
    display.println(".done.");
    display.print("Inits.");
    menuButtonTrains.init();
    menuButtonOutput.init();
    menuButtonEBreak.init();
    menuButtonSwitch.init();

    menuButtonStop.init();
    menuButtonShift.init();
    menuButtonFN0.init();
    menuButtonFN1.init();
    menuButtonFN2.init();
    menuButtonFN3.init();
    menuButtonFN4.init();
    display.print("B");
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    rotaryEncoder.setBoundaries(0, 10, true); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    rotaryEncoder.disableAcceleration();      // acceleration is now enabled by default - disable if you dont need it
    display.print("R");
    locs.reserve(10);
    switches.reserve(10);
    outputs.reserve(10);
    display.print("V");
    mqttClient.setCallback(mqttCallback);
    display.print("C");
    logger.log(LOG_INFO, "Ended setup.");
    display.println("Ended setup.");
    currentLc = new lc(nullptr, nullptr, false, 0, 0, 0);
    currentSw = new sw(nullptr);
    currentCo = new co(nullptr);
    currentBk = new bk(nullptr);
    currentSg = new sg(nullptr);

    logger.log(LOG_INFO, "Ended mqtt calls setup.");
    delay(500); // Leave the half a second to get some info

    publishGetPowerState();
    publishGetLocList();
    display.cls();
}

void loop()
{
    static long resetTime = millis();
    //   static menuT menu = lcMenu;
    //   static subMenuT subMenu = coSubMenu;
    //   static bool inSelectMenu = false;
    bool trainsButton = menuButtonTrains.onPress();
    bool trainsSwitch = menuButtonSwitch.onPress();
    bool trainsOutput = menuButtonOutput.onPress();
    bool powerEBrk = menuButtonEBreak.onPress();
    bool stopButton = menuButtonStop.onPress();
    bool shiftButton = menuButtonShift.onPress();
    bool FN0Button = menuButtonFN0.onPress();
    bool FN1Button = menuButtonFN1.onPress();
    bool FN2Button = menuButtonFN2.onPress();
    bool FN3Button = menuButtonFN3.onPress();
    bool FN4Button = menuButtonFN4.onPress();

    static bool wifiWasConnected = true; // we start the loop connected

    if (WiFi.status() != WL_CONNECTED && wifiWasConnected) {
        wifiWasConnected = false;
        logger.log(LOG_WARN, "Wifi connection lost.");
    } else if (WiFi.status() == WL_CONNECTED && !wifiWasConnected) {
        wifiWasConnected = true;
        reconnect();
        ArduinoOTA.begin();
    }
    if (!mqttClient.connected())
        reconnect();
    mqttClient.loop();
    ArduinoOTA.handle();

    if (!state.getSelectMenu()) {
        // only allowed to change if not in select menu!
        // Reboot after 4 seconds of pressing both menuButtonTrains and menuButtonSwitch
        if (menuButtonTrains.read() == 0 && menuButtonSwitch.read() == 0) {
            if (millis() - resetTime > 4000) { // restart ESP
                ESP.restart();
            }
        } else {
            resetTime = millis();

            // DONE: if the inpMenu button is pressed, there shouldn't be
            //       other actions when turning the rotary button,
            //       so inpMenu should get active the moment it is pressed ie
            //       menuButtonOutput.read() == 0
            if (menuButtonOutput.read() == 0) {
                state.setMenu(inpMenu);
                // change min max to 3 -1
                rotaryEncoder.setBoundaries(0, 2, true);
            }
            if (trainsButton) {
                state.setMenu(lcMenu);
                publishGetLocList();
            } else if (trainsSwitch) {
                state.setMenu(swMenu);
                publishGetSwitchList();
            } else if (trainsOutput && menuButtonOutput.read() != 0) {
                // DONE: has to change when the rotatry button is used
                state.setMenu(inpMenu);
                logger.vlogf(LOG_DEBUG, "submenu '%d'", state.getSubMenu());
                switch (state.getSubMenu()) {
                case coSubMenu:
                    publishGetOutputsList();
                    break;
                case bkSubMenu:
                    publishGetBlocksList();
                    break;
                case sgSubMenu:
                    publishGetSignalsList();
                    break;
                }
            }
        }
        if (currentLc->isSelected()) {
            int fn = 0;
            if (stopButton) {
                currentLc->newSpeed = 0;
                rotaryEncoder.setEncoderValue(0);
                publishLocSpeed(0);
            }
            if (shiftButton) {
                display.shiftGroup = (display.shiftGroup + 1) % 4;
                display.updateLc(currentLc, rotaryEncoder.readEncoder());
            }
            if (FN0Button) {
                publishLocFn(0, !(currentLc->fn[0].fn));
                if (currentLc->fn[0].pushbutton)
                    publishLocFn(0, (currentLc->fn[0].fn));
            }
            if (FN1Button)
                fn = 1;
            if (FN2Button)
                fn = 2;
            if (FN3Button)
                fn = 3;
            if (FN4Button)
                fn = 4;
            if (fn) {
                fn = fn + 4 * display.shiftGroup;
                publishLocFn(fn, !(currentLc->fn[fn].fn));
                if (currentLc->fn[fn].pushbutton) {
                    publishLocFn(fn, (currentLc->fn[fn].fn));
                    // logger.vlogf(LOG_DEBUG, "pushbutton true '%d' '%d'", !(currentLc->fn[fn].fn),(currentLc->fn[fn].fn) );
                }
            }
        }
        if (currentSw->isSelected()) {
            if (stopButton) {
                publishSwToggle();
            }
        }
        if (currentCo->isSelected()) {
            if (stopButton) {
                currentCo->on = !(currentCo->on);
                logger.vlogf(LOG_INFO, "Co %s %d", currentCo->id, currentCo->on);
                publishCoState();
            }
        }
        if (currentSg->isSelected()) {
            if (stopButton) {
                //currentSg->on = !(currentCo->on); increment aspect
                logger.vlogf(LOG_INFO, "Sg %s %d", currentSg->id, currentSg->aspect);
                publishSgState();
            }
        }
    }
    if (powerEBrk) {
        publishSetPowerEBreak();
    }

    doRotaryButton();
    display.displayAll(state.getMenu(), state.getSubMenu(), liionbat.getPercentage(), liionbat.getVoltage());
    delay(50);
}