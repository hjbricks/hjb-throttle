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
#include "sglist.h"
#include "esplog.h"

sg::sg(char *id) : id(id)
{
}

sg::~sg()
{
    logger.vlogf(LOG_DEBUG, "Delete %s", id);
    if (id) {
        free(id);
        id = nullptr;
        initiated = false;
    }
}
void sg::setId(const char *newId)
{
    if (id)
        free(id);
    id = (char *)malloc(strlen(newId) + 1);
    strcpy(id, newId);
    initiated = true;
}

void sg::clear()
{
    if (id) {
        free(id);
        id = nullptr;
    }
    initiated = false;
}

void sg::setAspect(uint8_t newAspect)
{
    aspect = newAspect;
}
void sg::setAspects(uint8_t newAspects)
{
    aspects = newAspects;
}
uint8_t sg::getAspects()
{
    return aspects;
}
sg *currentSg; // only id is filled
std::vector<sg *> signals;