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

#include "7ZipArchive.h"
#include "LevelPack.h"
#include <cstdio>
#include <cstring>

LevelPack::LevelPack()
{
    m_pkArchive = new SevenZipArchive();
}

LevelPack::~LevelPack()
{
    if(m_pkArchive != NULL)
    {
        m_pkArchive->Close();
        delete m_pkArchive;
        m_pkArchive = NULL;
    }
}

void LevelPack::Open(const char *strFilename)
{
    ResetCurrentLevel();
    if(m_pkArchive->Opened())
    {
        m_pkArchive->Close();
    }
    m_pkArchive->Open(strFilename);
    m_iLevelCount = m_pkArchive->GetFilesCount();
}

void LevelPack::GetLevelData(u32 iLevel, u8 **data, size_t *size)
{
    char *name = new char[16];
    sprintf(name, "%d", iLevel);
    name = strcat(name, ".tlv");
    m_pkArchive->Extract(name, data, size);
    delete [] name;
}