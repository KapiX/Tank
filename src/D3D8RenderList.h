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

#ifndef _D3D8RENDERLIST_H_
#define _D3D8RENDERLIST_H_

#include "RenderList.h"
#include "D3D8VideoDriver.h"

#include <d3d8.h>
#include <d3dx8.h>

class D3D8RenderList :
    public RenderList
{
private:
    LPDIRECT3DDEVICE8       m_pD3DDevice;
    LPDIRECT3DVERTEXBUFFER8 m_pVB;

    Vertex *m_aVerts;

    u32 m_iCount;

public:
    D3D8RenderList(RENDERLIST_TYPE rlt, u32 iElements, LPDIRECT3DDEVICE8 pD3DDevice);
    ~D3D8RenderList(void);

    void AddElement(Sprite *pSprite, Color *pColor = 0);

    void FillBuffer();
    void UpdateBuffer(u32 iCount, UpdateData *aUpdateData, Color *pUpdateColor = 0);

    void Render();
};

#endif // _D3D8RENDERLIST_H_