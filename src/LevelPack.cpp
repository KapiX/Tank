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

#include "LevelPack.h"
#include <cstring>

void LevelPack::Init(const char *strFilename)
{
    ResetCurrentLevel();
    m_iLevelCount = m_kArchive.GetFilesCount();
    m_kArchive.Open(strFilename);
}

void LevelPack::Free()
{
    m_kArchive.Close();
}

void LevelPack::GetLevelData(int iLevel, Byte **data, unsigned int *size)
{
    char *name = new char[16];
    itoa(iLevel, name, 10);
    name = strcat(name, ".tlv");
    m_kArchive.Extract(name, data, size);
    delete [] name;
}