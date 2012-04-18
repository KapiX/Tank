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
#include <cstring>

bool SevenZipArchive::m_sbInitialized = false;
ISzAlloc SevenZipArchive::m_skAlloc;

void SevenZipArchive::Init()
{
    if(m_sbInitialized)
    {
        return;
    }

    ::CrcGenerateTable();

    m_skAlloc.Alloc = ::SzAlloc;
    m_skAlloc.Free = ::SzFree;

    m_sbInitialized = true;
}

void SevenZipArchive::Open(const char *strFilename)
{
    if(m_bOpened)
    {
        return;
    }

    // Make sure all is ready to go
    SevenZipArchive::Init();

    SRes result;
    // Open file
    result = InFile_Open(&m_kInStream.file, strFilename);
    if(result != SZ_OK)
    {
        return;
    }

    // Prepare tables
    FileInStream_CreateVTable(&m_kInStream);
    LookToRead_CreateVTable(&m_kLookStream, false);

    m_kLookStream.realStream = &m_kInStream.s;
    LookToRead_Init(&m_kLookStream);

    // Open archive
    SzArEx_Init(&m_kDB);
    result = SzArEx_Open(&m_kDB, &m_kLookStream.s, &m_skAlloc, &m_skAlloc);

    if(result != SZ_OK)
    {
        SzArEx_Free(&m_kDB, &m_skAlloc);
        File_Close(&m_kInStream.file);
    }

    // Prepare list containing file names
    // Initialize array
    m_astrFiles = new uchar *[m_kDB.db.NumFiles];
    m_iFilesCount = m_kDB.db.NumFiles;
    
    for(size_t i = 0; i < m_iFilesCount; i++)
    {
        size_t iLen;
        GetFileName(i, NULL, &iLen);

        m_astrFiles[i] = new uchar[iLen];
        GetFileName(i, m_astrFiles[i], &iLen);
    }

    m_bOpened = true;
}

void SevenZipArchive::Close()
{
    if(!m_bOpened)
    {
        return;
    }

    for(u32 i = 0; i < m_iFilesCount; i++)
    {
        if(m_astrFiles[i])
        {
            delete [] m_astrFiles[i];
            m_astrFiles[i] = NULL;
        }
    }

    if(m_astrFiles)
    {
        delete [] m_astrFiles;
        m_astrFiles = NULL;
    }

    if(m_pOutBuffer)
    {
        IAlloc_Free(&m_skAlloc, m_pOutBuffer);
        m_pOutBuffer = NULL;
    }

    // Free archive
    SzArEx_Free(&m_kDB, &m_skAlloc);
    // Close file
    File_Close(&m_kInStream.file);

    m_bOpened = false;
}

bool SevenZipArchive::Extract(char *strFileName, u8 **ppData, size_t *piSize)
{
    if(!m_bOpened)
        return false;

    u32 iIndex = GetFileIndex((uchar *) strFileName);
    if(iIndex == -1)
    {
        return false;
    }

    size_t iOffset = 0;

    SRes result = GetFile(iIndex, iOffset, *piSize);
    if(result != SZ_OK)
    {
        return false;
    }

    *ppData = new u8[*piSize];
    memcpy(*ppData, m_pOutBuffer + iOffset, *piSize);

    return true;
}

bool SevenZipArchive::Utf16ToUtf8(u8 *pDest, size_t *pDestLen, const u16 *pSrc, size_t iSrcLen)
{
    static const u8 saUtf8Limits[5] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    size_t destPos = 0, srcPos = 0;
    for (;;)
    {
        u32 numAdds;
        u32 value;
        if (srcPos == iSrcLen)
        {
            *pDestLen = destPos;
            return true;
        }
        value = pSrc[srcPos++];
        if (value < 0x80)
        {
            if (pDest)
                pDest[destPos] = (char)value;
            destPos++;
            continue;
        }
        if (value >= 0xD800 && value < 0xE000)
        {
            UInt32 c2;
            if (value >= 0xDC00 || srcPos == iSrcLen)
                break;
            c2 = pSrc[srcPos++];
            if (c2 < 0xDC00 || c2 >= 0xE000)
                break;
            value = (((value - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;
        }
        for (numAdds = 1; numAdds < 5; numAdds++)
            if (value < (((u32)1) << (numAdds * 5 + 6)))
                break;
        if (pDest)
            pDest[destPos] = (char)(saUtf8Limits[numAdds - 1] + (value >> (6 * numAdds)));
        destPos++;
        do
        {
            numAdds--;
            if (pDest)
                pDest[destPos] = (char)(0x80 + ((value >> (6 * numAdds)) & 0x3F));
            destPos++;
        }
        while (numAdds != 0);
    }
    *pDestLen = destPos;
    return false;
}

void SevenZipArchive::GetFileName(u32 iIndex, uchar *strFileName, size_t *piLength)
{
    // Get name length
    size_t iLen = SzArEx_GetFileNameUtf16(&m_kDB, iIndex, NULL);

    // Alloc buffer
    u16 *strTmp = new u16[iLen];
    SzArEx_GetFileNameUtf16(&m_kDB, iIndex, strTmp);

    SevenZipArchive::Utf16ToUtf8(strFileName, piLength, strTmp, iLen);

    delete [] strTmp;
}

u32 SevenZipArchive::GetFileIndex(uchar *strFileName)
{
    for(u32 i = 0; i < m_iFilesCount; i++)
    {
        if(strcmp((char *) m_astrFiles[i], (char *) strFileName) == 0)
            return i;
    }

    return -1;
}

SRes SevenZipArchive::GetFile(u32 iIndex, size_t &riOffset, size_t &riOutSizeProcessed)
{
    return SzArEx_Extract(&m_kDB, &m_kLookStream.s, iIndex, &m_iBlockIndex,
        &m_pOutBuffer, &m_iOutBufferSize, &riOffset, &riOutSizeProcessed,
        &m_skAlloc, &m_skAlloc);
}