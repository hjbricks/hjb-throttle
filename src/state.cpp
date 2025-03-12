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

#include "state.h"
programState::programState()
{
    setInitial();
}

void programState::setInitial()
{
    menu = lcMenu;
    subMenu = coSubMenu;
    inSelectMenu = false;
}

bool programState::getSelectMenu()
{
    return inSelectMenu;
}

void programState::setSelectMenu(bool newSelectMenu)
{
    inSelectMenu = newSelectMenu;
}
void programState::invertMenuSelection()
{
    inSelectMenu = !inSelectMenu;
}

menuT programState::getMenu()
{
    return menu;
}
void programState::setMenu(menuT newMenu)
{
    menu = newMenu;
}

subMenuT programState::getSubMenu()
{
    return subMenu;
}
void programState::setSubMenu(subMenuT newSubMenu)
{
    subMenu = newSubMenu;
}
programState state;