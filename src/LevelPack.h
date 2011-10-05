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

#ifndef _LEVELPACK_H_
#define _LEVELPACK_H_

#include "7z/7z.h"
#include "7z/7zAlloc.h"
#include "7z/7zCrc.h"
#include "7z/7zFile.h"
#include "7z/7zVersion.h"

static void *Alloc(void *p, size_t size) {
	p = p;
	if (size == 0)
		return 0;
	return malloc(size);
}
static void Free(void *p, void *address)
{
	p = p;

	free(address);
}

class LevelPack
{
private:
	int *m_piIndex; // tablica z indeksami plików zawieraj¹cych kolejne poziomy
	int m_iCurrentLevel;
	int m_iLevelCount;

	CFileInStream packStream;
	CLookToRead lookStream;
	CSzArEx db;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	
	ISzAlloc g_Alloc;

	int Buf_EnsureSize(CBuf *dest, size_t size);
#ifndef _WIN32
	Byte kUtf8Limits[5] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC }; // TODO nie skompiluje sie

	Bool Utf16_To_Utf8(Byte *dest, size_t *destLen, const UInt16 *src, size_t srcLen);
	SRes Utf16_To_Utf8Buf(CBuf *dest, const UInt16 *src, size_t srcLen);
#endif
	SRes Utf16_To_Char(CBuf *buf, const UInt16 *s, int fileMode);
	char *ReturnChar(const UInt16 *s);
	

public:
	LevelPack()
	{
		g_Alloc.Alloc = ::Alloc;
		g_Alloc.Free = ::Free;
#ifndef _WIN32
		kUtf8Limits[0] = 0xC0;
		kUtf8Limits[1] = 0xE0;
		kUtf8Limits[2] = 0xF0;
		kUtf8Limits[3] = 0xF8;
		kUtf8Limits[4] = 0xFC;
#endif
	}

	void Init(const char *strFilename);
	void Free();

	void GetLevelData(int iLevel, Byte **data, unsigned int *size);
	inline int GetCurrentLevel() { return m_iCurrentLevel; }
	inline int GetNextLevel() { return m_iCurrentLevel + 1; }
	inline int GetLevelCount() { return m_iLevelCount; }
	inline int NextLevel() { m_iCurrentLevel += 1; return m_iCurrentLevel; }
	inline void ResetCurrentLevel() { m_iCurrentLevel = 1; }
};

#endif