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

#include "D3D8Texture.h"
#include <d3dx8.h>

D3D8Texture::D3D8Texture(const char *szFilename, LPDIRECT3DDEVICE8 pD3DDevice)
    : Texture(szFilename)
{
    D3DXCreateTextureFromFile(pD3DDevice, szFilename, &m_pkTexture);

    D3DSURFACE_DESC kDesc;
    m_pkTexture->GetLevelDesc(0, &kDesc);
    m_iWidth = kDesc.Width;
    m_iHeight = kDesc.Height;

    m_iID = 0; // not used
}

D3D8Texture::~D3D8Texture()
{
    if(m_pkTexture != NULL)
    {
        m_pkTexture->Release();
        m_pkTexture = NULL;
    }
}