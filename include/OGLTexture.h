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

#ifndef _OGLTEXTURE_H_
#define _OGLTEXTURE_H_

#include "Texture.h"

#include "SDL.h"
#include "SDL_image.h"
#include <GL/glew.h>

class OGLTexture :
    public Texture
{
private:
    SDL_Surface *m_pTextureData;

public:
    OGLTexture(const char *szFilename);
    ~OGLTexture();

    u32 GetWidth() { return m_pTextureData->w; }
    u32 GetHeight() { return m_pTextureData->h; }
};

#endif // _OGLTEXTURE_H_