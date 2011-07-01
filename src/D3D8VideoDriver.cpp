#include "D3D8VideoDriver.h"
#include "D3D8RenderList.h"

D3D8VideoDriver::D3D8VideoDriver(u32 iWidth, u32 iHeight, void *hWnd)
{
    m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

    D3DDISPLAYMODE d3ddm;
    m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;

    m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                         (HWND) hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                         &d3dpp, &m_pD3DDevice);

    m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

D3D8VideoDriver::~D3D8VideoDriver(void)
{
    if(m_pD3D != NULL)
    {
        m_pD3D->Release();
    }
    if(m_pD3DDevice != NULL)
    {
        m_pD3DDevice->Release();
    }
}

void D3D8VideoDriver::Clear(u8 iR, u8 iG, u8 iB, u8 iA)
{
    m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(iA, iR, iG, iB), 1.0f, 0);
}

void D3D8VideoDriver::BeginScene()
{
    m_pD3DDevice->BeginScene();
}

void D3D8VideoDriver::EndScene()
{
    m_pD3DDevice->EndScene();
    m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void D3D8VideoDriver::Zoom(f32 iZoom)
{
}

RenderList *D3D8VideoDriver::CreateRenderList(RENDERLIST_TYPE rlt, u32 iElements)
{
    return new D3D8RenderList(rlt, iElements, m_pD3DDevice);
}

void D3D8VideoDriver::Resize(u32 iWidth, u32 iHeight)
{
}