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

#include "D3D8RenderList.h"
#include "D3D8Texture.h"
#include "D3D8VideoDriver.h"
#include <d3dx8.h>

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
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                         (HWND) hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                         &d3dpp, &m_pD3DDevice);

    m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x00000001);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    
    Resize(iWidth, iHeight);
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
    m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(iA, iR, iG, iB), 1.0f, 0);
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
    D3DXMATRIX kMatScale;

    D3DXMatrixScaling(&kMatScale, iZoom, iZoom, 1.0f);

    m_pD3DDevice->SetTransform(D3DTS_VIEW, &kMatScale);
}

RenderList *D3D8VideoDriver::CreateRenderList(RENDERLIST_TYPE rlt, u32 iElements)
{
    return new D3D8RenderList(rlt, iElements, m_pD3DDevice);
}

Texture *D3D8VideoDriver::CreateTexture(const char *szFilename)
{
    return new D3D8Texture(szFilename, m_pD3DDevice);
}

void D3D8VideoDriver::PrintText(Texture *pFont, int iX, int iY, int iZ, const char *szText, float fScale, int iR, int iG, int iB, int iA, int iCharW, int iCharH)
{
    static_cast<D3D8Texture *>(pFont)->Set(m_pD3DDevice);
    u32 iColor = D3DCOLOR_RGBA(iR, iG, iB, iA);
    for(u32 i = 0; i < strlen(szText); i++)
    {
        f32 iS1 = (f32) (((uchar) szText[i] % 16) * (iCharW + 1)) / pFont->GetWidth();
        f32 iT1 = (f32) (((uchar) szText[i] / 16) * (iCharH + 1)) / pFont->GetHeight();
        f32 iS2 = (f32) (((uchar) szText[i] % 16 + 1) * (iCharW + 1) - 1) / pFont->GetWidth();
        f32 iT2 = (f32) (((uchar) szText[i] / 16 + 1) * (iCharH + 1) - 1) / pFont->GetHeight();

        Vertex akVerts[] = {
            { iX + ((iCharW + 1) * i) * fScale, iY, iZ, iColor, iS1, iT1 },
            { iX + (((iCharW + 1) * i) + iCharW) * fScale, iY, iZ, iColor, iS2, iT1 },
            { iX + ((iCharW + 1) * i) * fScale, iY + iCharH * fScale, iZ, iColor, iS1, iT2 },
            { iX + (((iCharW + 1) * i) + iCharW) * fScale, iY + iCharH * fScale, iZ, iColor, iS2, iT2 },
        };

        m_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
        m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, akVerts, sizeof(Vertex));
    }
    m_pD3DDevice->SetTexture(0, NULL);
}

void D3D8VideoDriver::DrawSprite(Texture *pSprite, int iX, int iY, int iZ, int iX2, int iY2, int iW, int iH, float fScale)
{
    if(iW == 0)
    {
        iW = pSprite->GetWidth();
    }
    if(iH == 0)
    {
        iH = pSprite->GetHeight();
    }

    static_cast<D3D8Texture *>(pSprite)->Set(m_pD3DDevice);
    u32 iColor = 0xFFFFFFFF;
    f32 iS1 = (f32) iX2 / pSprite->GetWidth();
    f32 iT1 = (f32) iY2 / pSprite->GetHeight();
    f32 iS2 = (f32) (iX2 + iW) / pSprite->GetWidth();
    f32 iT2 = (f32) (iY2 + iH) / pSprite->GetHeight();

    Vertex akVerts[] = {
        { iX, iY, iZ, iColor, iS1, iT1 },
        { iX + iW * fScale, iY, iZ, iColor, iS2, iT1 },
        { iX, iY + iH * fScale, iZ, iColor, iS1, iT2 },
        { iX + iW * fScale, iY + iH * fScale, iZ, iColor, iS2, iT2 },
    };

    m_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, akVerts, sizeof(Vertex));
    m_pD3DDevice->SetTexture(0, NULL);
}

void D3D8VideoDriver::DrawLine(int iX, int iY, int iX2, int iY2, int iZ, int iR, int iG, int iB, int iA)
{
    u32 iColor = D3DCOLOR_RGBA(iR, iG, iB, iA);

    Vertex akVerts[] = {
        { iX, iY, iZ, iColor, 0.0f, 0.0f },
        { iX2, iY2, iZ, iColor, 0.0f, 0.0f },
    };

    m_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    m_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, akVerts, sizeof(Vertex));
}

void D3D8VideoDriver::FillRectangle(int iX, int iY, int iW, int iH, int iZ, int iR, int iG, int iB, int iA)
{
    u32 iColor = D3DCOLOR_RGBA(iR, iG, iB, iA);

    Vertex akVerts[] = {
        { iX, iY, iZ, iColor, 0.0f, 0.0f },
        { iX + iW, iY, iZ, iColor, 0.0f, 1.0f },
        { iX, iY + iH, iZ, iColor, 1.0f, 0.0f },
        { iX + iW, iY + iH, iZ, iColor, 1.0f, 1.0f },
    };

    m_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, akVerts, sizeof(Vertex));
}

void D3D8VideoDriver::Resize(u32 iWidth, u32 iHeight)
{
    // We're using right-handed coordinate system...
    D3DXMATRIX kMatProjection;
    D3DXMatrixOrthoOffCenterRH(&kMatProjection, 0.0f, iWidth, iHeight, 0.0f, 65.0f, 0.1f);
    m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &kMatProjection);

    // ...so we need to scale world space by -1 in the z-direction
    D3DXMATRIX kMatView;
    D3DXMatrixIdentity(&kMatView);
    kMatView._31 = -kMatView._31;
    kMatView._32 = -kMatView._32;
    kMatView._33 = -kMatView._33;
    kMatView._34 = -kMatView._34;
    m_pD3DDevice->SetTransform(D3DTS_VIEW, &kMatView);
}