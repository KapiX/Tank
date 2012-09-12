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

#ifndef _7ZIPARCHIVE_H_
#define _7ZIPARCHIVE_H_

extern "C" {
#include "7z/7z.h"
#include "7z/7zAlloc.h"
#include "7z/7zCrc.h"
#include "7z/7zFile.h"
#include "7z/7zVersion.h"
}

#include "Types.h"

class SevenZipArchive
{
private:
    static bool m_sbInitialized;
    static ISzAlloc m_skAlloc;

    CFileInStream m_kInStream;
    CLookToRead m_kLookStream;
    CSzArEx m_kDB;

    u32 m_iBlockIndex;
    u8 *m_pOutBuffer;
    size_t m_iOutBufferSize;

    bool m_bOpened;

    uc8 **m_astrFiles;
    u32 m_iFilesCount;

public:
    static void Init();

    SevenZipArchive() :
        m_bOpened(false),
        m_iBlockIndex(0xFFFFFFFF),
        m_pOutBuffer(NULL),
        m_iOutBufferSize(0)
    {}
    ~SevenZipArchive() {}

    void Open(const char *strFilename);
    void Close();

    bool Exists(char *strFileName) { return GetFileIndex((uc8 *) strFileName) != -1; }
    u32 Find(char *strFileName) { return GetFileIndex((uc8 *) strFileName); }
    bool Extract(char *strFileName, u8 **ppData, size_t *piSize);

    u32 GetFilesCount() const { return m_iFilesCount; }

    bool Opened() const { return m_bOpened; }

protected:
    static bool Utf16ToUtf8(u8 *pDest, size_t *pDestLen, const u16 *pSrc, size_t iSrcLen);

    /*
    GetFileName(u32, uc8 *, u32 *)
    Returns:
        strFileName - file name
        piLength - file name length
    */
    void GetFileName(u32 iIndex, uc8 *strFileName, size_t *piLength);
    u32 GetFileSize(u32 iIndex) { return (m_kDB.db.Files + iIndex)->Size; }
    s32 GetFileIndex(uc8 *strFileName);
    bool IsDirectory(u32 iIndex) { return (m_kDB.db.Files + iIndex)->IsDir != 0; }
    SRes GetFile(u32 iIndex, size_t &riOffset, size_t &riOutSizeProcessed);
};

#endif // _7ZIPARCHIVE_H_
