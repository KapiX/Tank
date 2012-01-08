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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Types.h"

class Texture
{
protected:
    u32 m_iID;

public:
    Texture(const char *szFilename) {}
    virtual ~Texture() {}

    u32 GetID() { return m_iID; }

    virtual u32 GetWidth() = 0;
    virtual u32 GetHeight() = 0;
};

#endif // _TEXTURE_H_