#pragma once

#include "VideoDriver.h"
#include "Singleton.h"
#include "RenderList.h"

#include <d3d8.h>

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")

struct Vertex
{
    float fX, fY, fZ, fRHW;
    float fS, fT;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)

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

private:
    void Resize(u32 iWidth, u32 iHeight);
};

