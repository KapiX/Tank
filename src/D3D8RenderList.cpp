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

D3D8RenderList::D3D8RenderList(RENDERLIST_TYPE rlt, u32 iElements, LPDIRECT3DDEVICE8 pD3DDevice) :
    RenderList(rlt, iElements),
    m_iCount(0)
{
    int iVertsPerSprite = (rlt == RLT_LINKED ? 4 : 6);

    m_pD3DDevice = pD3DDevice;
    m_aVerts = new Vertex[m_iElements * iVertsPerSprite];
}

D3D8RenderList::~D3D8RenderList(void)
{
    if(m_pVB != NULL)
    {
        m_pVB->Release();
        m_pVB = NULL;
    }
    delete [] m_aVerts;
}

void D3D8RenderList::AddElement(Sprite *pSprite, Color *pColor)
{
    if(m_iCount >= m_iElements)
    {
        return;
    }

    u32 iColor = 0xFFFFFFFF;
    if(pColor != NULL)
    {
        iColor = D3DCOLOR_RGBA(pColor->iR, pColor->iG, pColor->iB, pColor->iA);
    }

    if(m_kRLT == RLT_LINKED)
    {
        Vertex v1 = { pSprite->iX, pSprite->iY, pSprite->iZ, iColor, pSprite->iS1 / m_pTexture->GetWidth(), pSprite->iT1 / m_pTexture->GetHeight() };
        Vertex v2 = { pSprite->iX + pSprite->iW, pSprite->iY, pSprite->iZ, iColor, pSprite->iS2 / m_pTexture->GetWidth(), pSprite->iT1 / m_pTexture->GetHeight() };
        Vertex v3 = { pSprite->iX, pSprite->iY + pSprite->iH, pSprite->iZ, iColor, pSprite->iS1 / m_pTexture->GetWidth(), pSprite->iT2 / m_pTexture->GetHeight() };
        Vertex v4 = { pSprite->iX + pSprite->iW, pSprite->iY + pSprite->iH, pSprite->iZ, iColor, pSprite->iS2 / m_pTexture->GetWidth(), pSprite->iT2 / m_pTexture->GetHeight() };
        m_aVerts[m_iCount * 4] = v1;
        m_aVerts[m_iCount * 4 + 1] = v2;
        m_aVerts[m_iCount * 4 + 2] = v3;
        m_aVerts[m_iCount * 4 + 3] = v4;
    }
    else if(m_kRLT == RLT_NORMAL)
    {
        Vertex v1 = { pSprite->iX, pSprite->iY, pSprite->iZ, iColor, pSprite->iS1 / m_pTexture->GetWidth(), pSprite->iT1 / m_pTexture->GetHeight() };
        Vertex v2 = { pSprite->iX + pSprite->iW, pSprite->iY, pSprite->iZ, iColor, pSprite->iS2 / m_pTexture->GetWidth(), pSprite->iT1 / m_pTexture->GetHeight() };
        Vertex v3 = { pSprite->iX, pSprite->iY + pSprite->iH, pSprite->iZ, iColor, pSprite->iS1 / m_pTexture->GetWidth(), pSprite->iT2 / m_pTexture->GetHeight() };
        Vertex v4 = { pSprite->iX, pSprite->iY + pSprite->iH, pSprite->iZ, iColor, pSprite->iS1 / m_pTexture->GetWidth(), pSprite->iT2 / m_pTexture->GetHeight() };
        Vertex v5 = { pSprite->iX + pSprite->iW, pSprite->iY + pSprite->iH, pSprite->iZ, iColor, pSprite->iS2 / m_pTexture->GetWidth(), pSprite->iT2 / m_pTexture->GetHeight() };
        Vertex v6 = { pSprite->iX + pSprite->iW, pSprite->iY, pSprite->iZ, iColor, pSprite->iS2 / m_pTexture->GetWidth(), pSprite->iT1 / m_pTexture->GetHeight() };
        m_aVerts[m_iCount * 6] = v1;
        m_aVerts[m_iCount * 6 + 1] = v2;
        m_aVerts[m_iCount * 6 + 2] = v3;
        m_aVerts[m_iCount * 6 + 3] = v4;
        m_aVerts[m_iCount * 6 + 4] = v5;
        m_aVerts[m_iCount * 6 + 5] = v6;
    }

    m_iCount++;
}

void D3D8RenderList::FillBuffer()
{
    int iVertsPerSprite = (m_kRLT == RLT_LINKED ? 4 : 6);

    m_pD3DDevice->CreateVertexBuffer(m_iElements * iVertsPerSprite * sizeof(Vertex), D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_pVB);

    BYTE *pData;
    m_pVB->Lock(0, m_iElements * iVertsPerSprite * sizeof(Vertex), &pData, D3DLOCK_DISCARD);
    memcpy(pData, m_aVerts, m_iElements * iVertsPerSprite * sizeof(Vertex));
    m_pVB->Unlock();
    pData = NULL;
}

void D3D8RenderList::UpdateBuffer(u32 iCount, UpdateData *aUpdateData, Color *pUpdateColor)
{
    int iVertsPerSprite = (m_kRLT == RLT_LINKED ? 4 : 6);

    u32 iColor = 0xFFFFFFFF;
    if(pUpdateColor != NULL)
    {
        iColor = D3DCOLOR_RGBA(pUpdateColor->iR, pUpdateColor->iG, pUpdateColor->iB, pUpdateColor->iA);
    }

    for(u32 i = 0; i < iCount; i++)
    {
        if(m_kRLT == RLT_LINKED)
        {
            Vertex v1 = { aUpdateData[i].iX, aUpdateData[i].iY, aUpdateData[i].iZ, iColor, aUpdateData[i].iS1 / m_pTexture->GetWidth(), aUpdateData[i].iT1 / m_pTexture->GetHeight() };
            Vertex v2 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY, aUpdateData[i].iZ, iColor, aUpdateData[i].iS2 / m_pTexture->GetWidth(), aUpdateData[i].iT1 / m_pTexture->GetHeight() };
            Vertex v3 = { aUpdateData[i].iX, aUpdateData[i].iY + aUpdateData[i].iH, aUpdateData[i].iZ, iColor, aUpdateData[i].iS1 / m_pTexture->GetWidth(), aUpdateData[i].iT2 / m_pTexture->GetHeight() };
            Vertex v4 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY + aUpdateData[i].iH, aUpdateData[i].iZ, iColor, aUpdateData[i].iS2 / m_pTexture->GetWidth(), aUpdateData[i].iT2 / m_pTexture->GetHeight() };
            m_aVerts[aUpdateData[i].iNum * 4] = v1;
            m_aVerts[aUpdateData[i].iNum * 4 + 1] = v2;
            m_aVerts[aUpdateData[i].iNum * 4 + 2] = v3;
            m_aVerts[aUpdateData[i].iNum * 4 + 3] = v4;
        }
        else if(m_kRLT == RLT_NORMAL)
        {
            Vertex v1 = { aUpdateData[i].iX, aUpdateData[i].iY, aUpdateData[i].iZ, iColor, aUpdateData[i].iS1 / m_pTexture->GetWidth(), aUpdateData[i].iT1 / m_pTexture->GetHeight() };
            Vertex v2 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY, aUpdateData[i].iZ, iColor, aUpdateData[i].iS2 / m_pTexture->GetWidth(), aUpdateData[i].iT1 / m_pTexture->GetHeight() };
            Vertex v3 = { aUpdateData[i].iX, aUpdateData[i].iY + aUpdateData[i].iH, aUpdateData[i].iZ, iColor, aUpdateData[i].iS1 / m_pTexture->GetWidth(), aUpdateData[i].iT2 / m_pTexture->GetHeight() };
            Vertex v4 = { aUpdateData[i].iX, aUpdateData[i].iY + aUpdateData[i].iH, aUpdateData[i].iZ, iColor, aUpdateData[i].iS1 / m_pTexture->GetWidth(), aUpdateData[i].iT2 / m_pTexture->GetHeight() };
            Vertex v5 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY + aUpdateData[i].iH, aUpdateData[i].iZ, iColor, aUpdateData[i].iS2 / m_pTexture->GetWidth(), aUpdateData[i].iT2 / m_pTexture->GetHeight() };
            Vertex v6 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY, aUpdateData[i].iZ, iColor, aUpdateData[i].iS2 / m_pTexture->GetWidth(), aUpdateData[i].iT1 / m_pTexture->GetHeight() };
            m_aVerts[aUpdateData[i].iNum * 6] = v1;
            m_aVerts[aUpdateData[i].iNum * 6 + 1] = v2;
            m_aVerts[aUpdateData[i].iNum * 6 + 2] = v3;
            m_aVerts[aUpdateData[i].iNum * 6 + 3] = v4;
            m_aVerts[aUpdateData[i].iNum * 6 + 4] = v5;
            m_aVerts[aUpdateData[i].iNum * 6 + 5] = v6;
        }
    }
    BYTE *pData;
    m_pVB->Lock(0, m_iElements * iVertsPerSprite * sizeof(Vertex), &pData, D3DLOCK_DISCARD);
    memcpy(pData, m_aVerts, m_iElements * iVertsPerSprite * sizeof(Vertex));
    m_pVB->Unlock();
    pData = NULL;
}

void D3D8RenderList::Render()
{
    static_cast<D3D8Texture *>(m_pTexture)->Set(m_pD3DDevice);
    m_pD3DDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));
    m_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

    if(m_kRLT == RLT_LINKED)
    {
        m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_iElements * 4 - 2);
    }
    else if(m_kRLT == RLT_NORMAL)
    {
        m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iElements * 2);
    }
    m_pD3DDevice->SetTexture(0, NULL);
}