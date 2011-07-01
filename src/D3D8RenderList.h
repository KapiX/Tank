#pragma once

#include "RenderList.h"
#include "D3D8VideoDriver.h"

#include <d3d8.h>
#include <d3dx8.h>

class D3D8RenderList :
    public RenderList
{
private:
    LPDIRECT3DDEVICE8       m_pD3DDevice;
    LPDIRECT3DTEXTURE8      m_pTexture;
    LPDIRECT3DVERTEXBUFFER8 m_pVB;

    Vertex *m_aVerts;

    u32 m_iCount;

public:
    D3D8RenderList(RENDERLIST_TYPE rlt, u32 iElements, LPDIRECT3DDEVICE8 pD3DDevice);
    ~D3D8RenderList(void);

    void AddElement(Sprite *pSprite);

    void FillBuffer();
    void UpdateBuffer(u32 iCount, UpdateData *aUpdateData);

    void LoadTexture(const char *szFilename);
    void FreeTexture();

    void Render();
};