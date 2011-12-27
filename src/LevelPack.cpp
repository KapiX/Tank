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

	allocImp.Alloc = ::SzAlloc;
	allocImp.Free = ::SzFree;

	allocTempImp.Alloc = ::SzAllocTemp;
	allocTempImp.Free = ::SzFreeTemp;

	int error = InFile_Open(&packStream.file, strFilename);

	FileInStream_CreateVTable(&packStream);
	LookToRead_CreateVTable(&lookStream, False);

	lookStream.realStream = &packStream.s;
	LookToRead_Init(&lookStream);

	CrcGenerateTable();

	SzArEx_Init(&db);
	SRes res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);

	m_piIndex = new int[db.db.NumFiles];
	m_iLevelCount = db.db.NumFiles;

	for(int i = 0; i < db.db.NumFiles; i++)
	{
		UInt16 *temp = NULL;
		size_t tempSize = 0;

		const CSzFileItem *f = db.db.Files + i;
		size_t len = SzArEx_GetFileNameUtf16(&db, i, NULL);

		if(len > tempSize)
		{
			::SzFree(NULL, temp);
			tempSize = len;
			temp = (UInt16 *) ::SzAlloc(NULL, tempSize * sizeof(temp[0]));
			if(temp == 0)
			{
				res = SZ_ERROR_MEM;
				break;
			}
		}

		SzArEx_GetFileNameUtf16(&db, i, temp);

		char *name = ReturnChar(temp);
		char *levelNum = new char[strlen(name) - 4];
		strncpy(levelNum, name, strlen(name) - 4); // - .tlv
		int iLevelNum = atoi(levelNum);
		
		m_piIndex[iLevelNum] = i;

		::SzFree(NULL, temp);
	}
}

void LevelPack::Free()
{
	SzArEx_Free(&db, &allocImp);

	File_Close(&packStream.file);
}

void LevelPack::GetLevelData(int iLevel, Byte **data, unsigned int *size)
{
	static UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
    static Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
    static size_t outBufferSize = 0;
	size_t offset = 0;
    size_t outSizeProcessed = 0;

	SzArEx_Extract(&db, &lookStream.s, m_piIndex[iLevel], &blockIndex, &outBuffer, &outBufferSize, &offset, &outSizeProcessed, &allocImp, &allocTempImp);

	*size = outSizeProcessed;
	*data = new Byte[outSizeProcessed];
	memcpy(*data, (void *) &outBuffer[offset], outSizeProcessed);
}

int LevelPack::Buf_EnsureSize(CBuf *dest, size_t size)
{
	if (dest->size >= size)
		return 1;
	Buf_Free(dest, &g_Alloc);
	return Buf_Create(dest, size, &g_Alloc);
}

#ifndef _WIN32
Bool LevelPack::Utf16_To_Utf8(Byte *dest, size_t *destLen, const UInt16 *src, size_t srcLen)
{
	size_t destPos = 0, srcPos = 0;
	for (;;)
	{
		unsigned numAdds;
		UInt32 value;
		if (srcPos == srcLen)
		{
			*destLen = destPos;
			return True;
		}
		value = src[srcPos++];
		if (value < 0x80)
		{
			if (dest)
				dest[destPos] = (char)value;
			destPos++;
			continue;
		}
		if (value >= 0xD800 && value < 0xE000)
		{
			UInt32 c2;
			if (value >= 0xDC00 || srcPos == srcLen)
				break;
			c2 = src[srcPos++];
			if (c2 < 0xDC00 || c2 >= 0xE000)
				break;
			value = (((value - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;
		}
		for (numAdds = 1; numAdds < 5; numAdds++)
			if (value < (((UInt32)1) << (numAdds * 5 + 6)))
				break;
		if (dest)
			dest[destPos] = (char)(kUtf8Limits[numAdds - 1] + (value >> (6 * numAdds)));
		destPos++;
		do
		{
			numAdds--;
			if (dest)
				dest[destPos] = (char)(0x80 + ((value >> (6 * numAdds)) & 0x3F));
			destPos++;
		}
		while (numAdds != 0);
	}
	*destLen = destPos;
	return False;
}

SRes LevelPack::Utf16_To_Utf8Buf(CBuf *dest, const UInt16 *src, size_t srcLen)
{
	size_t destLen = 0;
	Bool res;
	Utf16_To_Utf8(NULL, &destLen, src, srcLen);
	destLen += 1;
	if (!Buf_EnsureSize(dest, destLen))
		return SZ_ERROR_MEM;
	res = Utf16_To_Utf8(dest->data, &destLen, src, srcLen);
	dest->data[destLen] = 0;
	return res ? SZ_OK : SZ_ERROR_FAIL;
}
#endif

SRes LevelPack::Utf16_To_Char(CBuf *buf, const UInt16 *s, int fileMode)
{
	int len = 0;
	for (len = 0; s[len] != '\0'; len++);

#ifdef _WIN32
	{
		int size = len * 3 + 100;
		if (!Buf_EnsureSize(buf, size))
			return SZ_ERROR_MEM;
		{
			char defaultChar = '_';
			BOOL defUsed;
			int numChars = WideCharToMultiByte(fileMode ?
				(
#ifdef UNDER_CE
				CP_ACP
#else
				AreFileApisANSI() ? CP_ACP : CP_OEMCP
#endif
				) : CP_OEMCP,
				0, (LPCWSTR) s, len, (char *)buf->data, size, &defaultChar, &defUsed);
			if (numChars == 0 || numChars >= size)
				return SZ_ERROR_FAIL;
			buf->data[numChars] = 0;
			return SZ_OK;
		}
	}
#else
	fileMode = fileMode;
	return Utf16_To_Utf8Buf(buf, s, len);
#endif
}

char *LevelPack::ReturnChar(const UInt16 *s)
{
	CBuf buf;
	SRes res;
	Buf_Init(&buf);
	res = Utf16_To_Char(&buf, s, 0);
	char *ret = new char[strlen((const char *) buf.data)];
	if(res == SZ_OK)
		strcpy(ret, (char *) buf.data);
	else
	{
		delete [] ret;
		ret = NULL;
	}
	Buf_Free(&buf, &g_Alloc);
	return ret;
}