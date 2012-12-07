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

#ifndef _TYPES_H_
#define _TYPES_H_

#if defined(_MSC_VER)
    typedef signed __int8       s8;
    typedef unsigned __int8     u8;
    typedef signed __int16      s16;
    typedef unsigned __int16    u16;
    typedef signed __int32      s32;
    typedef unsigned __int32    u32;
    typedef signed __int64      s64;
    typedef unsigned __int64    u64;
    typedef signed char         c8;
    typedef unsigned char       uc8;
    typedef float               f32;
    typedef double              f64;
    typedef wchar_t             uchar; // unicode char
#elif defined(__GNUC__)
    typedef signed char         s8;
    typedef unsigned char       u8;
    typedef signed short        s16;
    typedef unsigned short      u16;
    typedef signed int          s32;
    typedef unsigned int        u32;
    typedef signed long long    s64;
    typedef unsigned long long  u64;
    typedef signed char         c8;
    typedef unsigned char       uc8;
    typedef float               f32;
    typedef double              f64;
    typedef wchar_t             uchar; // unicode char
#endif

struct BonusArea
{
    u16 iX;
    u16 iY;
    u16 iW;
    u16 iH;
};

struct SpawnPoint
{
    u8 iX;
    u8 iY;
};

struct MapInfo
{
    char id[4];
    u8 iNameSize;
    char *strName;
    BonusArea bonusArea;
    SpawnPoint p1Spawn;
    SpawnPoint p2Spawn;
    SpawnPoint e1Spawn;
    SpawnPoint e2Spawn;
    SpawnPoint e3Spawn;
    SpawnPoint eagle;
};

// first byte - x, next one - y
enum BLOCK_TYPE
{
    BT_EMPTY = 48,
    BT_BRICK = 0,
    BT_BRICK_HBOT = 2048,
    BT_BRICK_HTOP = 2056,
    BT_BRICK_VLEFT = 4096,
    BT_BRICK_VRIGHT = 4104,
    BT_BRICK_LBOT = 6144,
    BT_BRICK_RTOP = 6152,
    BT_BRICK_RBOT = 6160,
    BT_BRICK_LTOP = 6168,
    BT_STEEL = 8,
    BT_SEA = 16,
    BT_JUNGLE = 2064,
    BT_ICE = 2072,
    BT_EAGLE = 32,
    BT_EAGLELTOP = 32,
    BT_EAGLELBOT = 40,
    BT_EAGLERTOP = 2080,
    BT_EAGLERBOT = 2088,
    BT_DEADEAGLE = 4128,
    BT_DEADEAGLELTOP = 4128,
    BT_DEADEAGLELBOT = 4136,
    BT_DEADEAGLERTOP = 6176,
    BT_DEADEAGLERBOT = 6184,
    BT_EDGE = 4112,
    BT_DUMMY = 49 // totally random, needed to clear BT_EMPTY space
};
    
enum DIRECTION
{
    DIR_UP = 1,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
};

enum TANKLEVEL
{
    TL_1 = 1,
    TL_2,
    TL_3,
    TL_4,
};
    
enum SHIELDLEVEL
{
    SL_1 = 1,
    SL_2,
    SL_3,
    SL_4,
};
    
enum SOUND
{
    SND_BONUS = 0,
    SND_BRICKHIT,
    SND_EEXPLOSION,
    SND_FEXPLOSION,
    SND_GAMEOVER,
    SND_ICE,
    SND_LEVELSTARTING,
    SND_LIFE,
    SND_MOVING,
    SND_NMOVING,
    SND_PAUSE,
    SND_SHIELDHIT,
    SND_SHOOT,
    SND_STEELHIT,
    SND_TBONUSHIT,
    
    SND_COUNT,
};

enum CHANNEL
{
    CNL_LEVELSTARTING = 0,
    CNL_EXPLOSIONS,
    CNL_MOVING,
    CNL_PAUSE,
    CNL_EFFECTS,
    
    // TODO additional channels
};

#endif // _TYPES_H_
