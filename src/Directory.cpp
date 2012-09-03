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

#include "Directory.h"

u32 Directory::ListFiles(std::string strDir, std::vector<std::string> *pastrFileNames)
{
    u32 iCount = 0;
#if defined(WIN32)
    m_hFind = FindFirstFile((strDir + "\\*").c_str(), &m_kFindData);
    do
    {
        if(m_kFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }
        if(pastrFileNames != NULL)
        {
            pastrFileNames->push_back(m_kFindData.cFileName);
        }
        iCount++;
    }
    while(FindNextFile(m_hFind, &m_kFindData) != 0);
    FindClose(m_hFind);
    
    return iCount;
#elif defined(__unix)
    m_pkDir = opendir((strDir + "/").c_str());
    
    if(m_pkDir != NULL)
    {
        while(m_pkFile = readdir(m_pkDir))
        {
            struct stat s;
            stat((strDir + "/" + m_pkFile->d_name).c_str(), &s);
            if(S_ISDIR(s.st_mode))
            {
                continue;
            }
            if(pastrFileNames != NULL)
            {
                pastrFileNames->push_back(m_pkFile->d_name);
            }
            iCount++;
        }
        closedir(m_pkDir);
    }
    return iCount;
#else
    #error This code does not support your platform.
#endif
}

