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

#include "OGLTexture.h"
#include "SDL_image.h"
#include "SDL_opengl.h"

OGLTexture::OGLTexture(const char *szFilename)
    : Texture(szFilename)
{
    m_pTextureData = IMG_Load(szFilename);

    glGenTextures(1, &m_iID);
    glBindTexture(GL_TEXTURE_2D, m_iID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, m_pTextureData->format->BytesPerPixel,
        m_pTextureData->w, m_pTextureData->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        m_pTextureData->pixels);
}

OGLTexture::~OGLTexture()
{
    glDeleteTextures(1, &m_iID);
    if(m_pTextureData != NULL)
    {
        SDL_FreeSurface(m_pTextureData);
        m_pTextureData = NULL;
    }
}