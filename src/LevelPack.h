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