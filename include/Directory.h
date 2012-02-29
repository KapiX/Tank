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

#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

#include "Singleton.h"
#include "Types.h"
#include <string>
#include <vector>
#ifdef _WIN32
#    include <Windows.h>
#elif __unix
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <dirent.h>
#endif

class Directory :
    public Singleton<Directory>
{
private:
#ifdef _WIN32
    WIN32_FIND_DATA m_kFindData;
    HANDLE m_hFind;
#elif __unix
    DIR *m_pkDir;
    struct dirent *m_pkFile;
#endif

public:
    u32 ListFiles(std::string strDir, std::vector<std::string> *pastrFiles);
};

#endif // _DIRECTORY_H_

