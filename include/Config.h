/*
    Copyright 2011, 2012 Kacper Kasper <kacperkasper@gmail.com>

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

#include "Singleton.h"
#include "Types.h"
#include <fstream>

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

struct Controls
{
    u32 iUp;
    u32 iDown;
    u32 iLeft;
    u32 iRight;
    u32 iShoot;
};

enum CONFIGITEM_TYPE
{
    CIT_INT = 1,
    CIT_BOOL,
    CIT_STRING,
};

struct ConfigItem
{
    const char *strIdentifier;
    void *pValue;
    CONFIGITEM_TYPE kType;
};

class Config :
    public Singleton<Config>
{
private:
    CONTROLLER m_kP1Controller;
    CONTROLLER m_kP2Controller;
    Controls m_kP1Controls;
    Controls m_kP2Controls;
    bool m_bFullscreen;
    u32 m_iVideoDriver;

    ConfigItem *m_akItems;
    u32 m_iItemCount;

public:
    Config();
    ~Config();

    void ReadFromFile(const char *strFilename);
    void SaveToFile(const char *strFilename);

    CONTROLLER GetP1Controller() const { return m_kP1Controller; }
    CONTROLLER GetP2Controller() const { return m_kP2Controller; }
    Controls *GetP1Controls() { return &m_kP1Controls; }
    Controls *GetP2Controls() { return &m_kP2Controls; }
    bool GetFullscreen() const { return m_bFullscreen; }
    u32 GetVideoDriver() const { return m_iVideoDriver; }
};

#endif // _CONFIG_H_