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
#include "lclist.h"
#include "esplog.h"

lc::lc(char *id, char *addr, bool vModePercent, int Vmax, int VRmax, int VSmax) 
: id(id), addr(addr), vModePercent(vModePercent), Vmax(VSmax), VRmax(VRmax), VSmax(VSmax)
{
    newSpeed = 0;
    V = 0;
}

lc::~lc()
{
    logger.vlogf(LOG_DEBUG, "Delete %s", id);
    if (id) {
        free(id);
        id = nullptr;
    }
    if (addr) {
        free(addr);
        addr = nullptr;
    }
}
void lc::setIdandAddr(const char *newId, const char *newAddr)
{
    if (id)
        free(id);
    if (addr)
        free(addr);
    id = (char *)malloc(strlen(newId) + 1);
    strcpy(id, newId);
    addr = (char *)malloc(strlen(newAddr) + 1);
    strcpy(addr, newAddr);
    initiated = true;
}

void lc::clear()
{
    if (id) {
        free(id);
        id = nullptr;
    }
    if (addr) {
        free(addr);
        addr = nullptr;
    }
    initiated = false;
}

lc *currentLc; // only id is filled
std::vector<lc *> locs;