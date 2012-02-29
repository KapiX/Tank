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

#ifndef _D3D8TEXTURE_H_
#define _D3D8TEXTURE_H_

#include "Texture.h"

#include <d3d8.h>
#include <d3dx8.h>

class D3D8Texture :
    public Texture
{
private:
    u32 m_iWidth;
    u32 m_iHeight;
    LPDIRECT3DTEXTURE8 m_pkTexture;

public:
    D3D8Texture(const char *szFilename, LPDIRECT3DDEVICE8 pD3DDevice);
    ~D3D8Texture();

    u32 GetWidth() { return m_iWidth; }
    u32 GetHeight() { return m_iHeight; }

    void Set(LPDIRECT3DDEVICE8 pD3DDevice) { pD3DDevice->SetTexture(0, m_pkTexture); }
};

#endif // _D3D8TEXTURE_H_