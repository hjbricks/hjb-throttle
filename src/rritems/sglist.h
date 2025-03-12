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

#include <string.h>
#include <vector>


class sg
{
  public:
    sg(char *id);
    ~sg();
    bool operator==(const sg &rhs) const { return strcmp(this->id, rhs.id) == 0; }
    void setId(const char *newId);
    void clear();
    bool isSelected() { return id != nullptr; }

    void setAspect(uint8_t newAspect);
    void setAspects(uint8_t newAspects);
    uint8_t getAspects();
    char *id;
    uint8_t aspect;
    bool initiated;
    protected:
    uint8_t aspects;
};

extern sg *currentSg; // only id is filled
extern std::vector<sg *> signals;