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
#include "bklist.h"
#include "esplog.h"

bk::bk(char *id) : id(id)
{
}

bk::~bk()
{
    logger.vlogf(LOG_DEBUG, "Delete %s", id);
    if (id) {
        free(id);
        id = nullptr;
        initiated = false;
    }
    clearSensors();
}
void bk::setId(const char *newId)
{
    if (id)
        free(id);
    id = (char *)malloc(strlen(newId) + 1);
    strcpy(id, newId);
    initiated = true;
}

void bk::clearSensors()
{
    for (auto p : sensorIds) {
        if (p->name)
            free(p->name);
    }
    sensorIds.clear();
}

void bk::clear()
{
    logger.vlogf(LOG_DEBUG, "Start block clear");
    if (id) {
        free(id);
        id = nullptr;
    }
    clearSensors();
    initiated = false;
    logger.vlogf(LOG_DEBUG, "Done block clear");
}

void bk::addSensor(char *newSensorName)
{
    // code to add the sensor id
    sensorT *foundSensor = nullptr;
    uint8_t count;
    foundSensor = findSensor(newSensorName, &count);

    if (!foundSensor) {
        sensorT *newSensor = new sensorT();
        newSensor->name = newSensorName;
        newSensor->state = sensor_undefined;
        sensorIds.push_back(newSensor);
    } else {
        free(newSensorName);
    }
}
sensorT *bk::findSensor(char *sensorName, uint8_t *count)
{
    sensorT *foundSensor = nullptr;
    *count = 0;
    for (auto p : sensorIds) {
        if (strcmp(p->name, sensorName) == 0) {
            foundSensor = p;
            break;
        }
        (*count) ++;
    }
    return foundSensor;
}

// for now we do hardcode max two sensors per block
void bk::getSensorstates(sensorStates results[]){
    uint8_t count = 0;
    for (auto p : sensorIds) {
        results[count] = p->state;
        count++;
        if (count >= 2)
            break; // hard exit
    }
}


bk *currentBk; // only id is filled
std::vector<bk *> blocks;