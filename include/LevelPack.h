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

#ifndef _LEVELPACK_H_
#define _LEVELPACK_H_

#include "Types.h"

class SevenZipArchive;

class LevelPack
{
private:
    u32 m_iCurrentLevel;
    u32 m_iLevelCount;
    SevenZipArchive *m_pkArchive;

public:
    LevelPack();
    ~LevelPack();

    void Open(const char *strFilename);

    void GetLevelData(u32 iLevel, u8 **data, size_t *size);
    u32 GetCurrentLevel() { return m_iCurrentLevel; }
    u32 GetNextLevel() { return m_iCurrentLevel + 1; }
    u32 GetLevelCount() { return m_iLevelCount; }
    u32 NextLevel() { m_iCurrentLevel += 1; return m_iCurrentLevel; }
    void ResetCurrentLevel() { m_iCurrentLevel = 1; }
};

#endif // _LEVELPACK_H_