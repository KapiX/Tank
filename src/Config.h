/*
	Copyright 2011 Kacper Kasper

	This file is part of Tank.

    Tank is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tank is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tank.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <fstream>
#include "Singleton.h"
#include "Types.h"

/*
  In case of more than one gamepad/joystick use these values as follows:
  X360GAMEPAD + 0 - for gamepad 1
  X360GAMEPAD + 1 - for gamepad 2
  X360GAMEPAD + 2 - for gamepad 3
  ... etc.
*/
enum CONTROLLER
{
    KEYBOARD = 1,
    X360GAMEPAD = 2, // xinput supports up to 4 x360 gamepads
    JOYSTICK = 6,
};

struct CONTROLS
{
    u32 iUp;
    u32 iDown;
    u32 iLeft;
    u32 iRight;
    u32 iShoot;
};

struct CONFIG_ITEM
{
    char strIdentifier[64];
    char strValue[64];
};

class Config :
    public Singleton<Config>
{
private:
    CONTROLLER m_kP1Controller;
    CONTROLLER m_kP2Controller;
    CONTROLS m_kP1Controls;
    CONTROLS m_kP2Controls;
    bool m_bFullscreen;

    CONFIG_ITEM ParseLine(const char *strLine);

public:
    void ReadFromFile(const char *strFilename);
    void SaveToFile(const char *strFilename);

    CONTROLLER GetP1Controller() const { return m_kP1Controller; }
    CONTROLLER GetP2Controller() const { return m_kP2Controller; }
    CONTROLS *GetP1Controls() { return &m_kP1Controls; }
    CONTROLS *GetP2Controls() { return &m_kP2Controls; }
    bool GetFullscreen() const { return m_bFullscreen; }
};

#endif