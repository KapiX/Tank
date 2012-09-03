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

#ifndef _D3D8VIDEODRIVER_H_
#define _D3D8VIDEODRIVER_H_

#include "Singleton.h"
#include "Types.h"
#include "VideoDriver.h"
#include <d3d8.h>

class RenderList;
class Texture;

struct Vertex
{
    float fX, fY, fZ;
    u32 iColor;
    float fS, fT;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class D3D8VideoDriver :
    public VideoDriver,
    public Singleton<D3D8VideoDriver>
{
private:
    LPDIRECT3D8         m_pD3D;
    LPDIRECT3DDEVICE8   m_pD3DDevice;

public:
    D3D8VideoDriver(u32 iWidth, u32 iHeight, void *hWnd);
    ~D3D8VideoDriver(void);

    void Clear(u8 iR, u8 iG, u8 iB, u8 iA = 255);
    void BeginScene();
    void EndScene();

    void Zoom(f32 iZoom);

    RenderList *CreateRenderList(RENDERLIST_TYPE rlt, u32 iElements);
    Texture *CreateTexture(const char *szFilename);

    void PrintText(Texture *pFont, int iX, int iY, int iZ, const char *szText, float fScale = 1.0f, int iR = 255, int iG = 255, int iB = 255, int iA = 255, int iCharW = 7, int iCharH = 13);
    void DrawSprite(Texture *pSprite, int iX, int iY, int iZ, int iX2, int iY2, int iW = 0, int iH = 0, float fScale = 1.0f);

    void DrawLine(int iX, int iY, int iX2, int iY2, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255);
    void FillRectangle(int iX, int iY, int iW, int iH, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255);

private:
    void Resize(u32 iWidth, u32 iHeight);
};

#endif // _D3D8VIDEODRIVER_H_