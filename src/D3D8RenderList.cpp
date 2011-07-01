#include "D3D8RenderList.h"

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

void D3D8RenderList::AddElement(Sprite *pSprite)
{
    if(m_iCount >= m_iElements)
    {
        return;
    }

    D3DSURFACE_DESC desc;
    m_pTexture->GetLevelDesc(0, &desc);

    if(m_kRLT == RLT_LINKED)
    {
        Vertex v1 = { pSprite->iX, pSprite->iY, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS1), 1.0f / (desc.Height / pSprite->iT1) };
        Vertex v2 = { pSprite->iX + pSprite->iW, pSprite->iY, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS2), 1.0f / (desc.Height / pSprite->iT1) };
        Vertex v3 = { pSprite->iX, pSprite->iY + pSprite->iH, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS1), 1.0f / (desc.Height / pSprite->iT2) };
        Vertex v4 = { pSprite->iX + pSprite->iW, pSprite->iY + pSprite->iH, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS2), 1.0f / (desc.Height / pSprite->iT2) };
        m_aVerts[m_iCount * 4] = v1;
        m_aVerts[m_iCount * 4 + 1] = v2;
        m_aVerts[m_iCount * 4 + 2] = v3;
        m_aVerts[m_iCount * 4 + 3] = v4;
    }
    else if(m_kRLT == RLT_NORMAL)
    {
        Vertex v1 = { pSprite->iX, pSprite->iY, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS1), 1.0f / (desc.Height / pSprite->iT1) };
        Vertex v2 = { pSprite->iX + pSprite->iW, pSprite->iY, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS2), 1.0f / (desc.Height / pSprite->iT1) };
        Vertex v3 = { pSprite->iX, pSprite->iY + pSprite->iH, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS1), 1.0f / (desc.Height / pSprite->iT2) };
        Vertex v4 = { pSprite->iX, pSprite->iY + pSprite->iH, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS1), 1.0f / (desc.Height / pSprite->iT2) };
        Vertex v5 = { pSprite->iX + pSprite->iW, pSprite->iY + pSprite->iH, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS2), 1.0f / (desc.Height / pSprite->iT2) };
        Vertex v6 = { pSprite->iX + pSprite->iW, pSprite->iY, 1.0f, 1.0f, 1.0f / (desc.Width / pSprite->iS2), 1.0f / (desc.Height / pSprite->iT1) };
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

void D3D8RenderList::UpdateBuffer(u32 iCount, UpdateData *aUpdateData)
{
    int iVertsPerSprite = (m_kRLT == RLT_LINKED ? 4 : 6);

    D3DSURFACE_DESC desc;
    m_pTexture->GetLevelDesc(0, &desc);

    for(u32 i = 0; i < iCount; i++)
    {
        if(m_kRLT == RLT_LINKED)
        {
            Vertex v1 = { aUpdateData[i].iX, aUpdateData[i].iY, 1.0f, 1.0f, 1.0f / (desc.Width / aUpdateData[i].iS1), 1.0f / (desc.Height / aUpdateData[i].iT1) };
            Vertex v2 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY, 1.0f, 1.0f, 1.0f / (desc.Width / aUpdateData[i].iS2), 1.0f / (desc.Height / aUpdateData[i].iT1) };
            Vertex v3 = { aUpdateData[i].iX, aUpdateData[i].iY + aUpdateData[i].iH, 1.0f, 1.0f, 1.0f / (desc.Width / aUpdateData[i].iS1), 1.0f / (desc.Height / aUpdateData[i].iT2) };
            Vertex v4 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY + aUpdateData[i].iH, 1.0f, 1.0f, 1.0f / (desc.Width / aUpdateData[i].iS2), 1.0f / (desc.Height / aUpdateData[i].iT2) };
            m_aVerts[aUpdateData[i].iNum * 4] = v1;
            m_aVerts[aUpdateData[i].iNum * 4 + 1] = v2;
            m_aVerts[aUpdateData[i].iNum * 4 + 2] = v3;
            m_aVerts[aUpdateData[i].iNum * 4 + 3] = v4;
        }
        else if(m_kRLT == RLT_NORMAL)
        {
            Vertex v1 = { aUpdateData[i].iX, aUpdateData[i].iY, 1.0f, 1.0f, (aUpdateData[i].iS1 / desc.Width), (aUpdateData[i].iT1 / desc.Height) };
            Vertex v2 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY, 1.0f, 1.0f, (aUpdateData[i].iS2 / desc.Width), (aUpdateData[i].iT1 / desc.Height) };
            Vertex v3 = { aUpdateData[i].iX, aUpdateData[i].iY + aUpdateData[i].iH, 1.0f, 1.0f, (aUpdateData[i].iS1 / desc.Width), (aUpdateData[i].iT2 / desc.Height) };
            Vertex v4 = { aUpdateData[i].iX, aUpdateData[i].iY + aUpdateData[i].iH, 1.0f, 1.0f, (aUpdateData[i].iS1 / desc.Width), (aUpdateData[i].iT2 / desc.Height) };
            Vertex v5 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY + aUpdateData[i].iH, 1.0f, 1.0f, (aUpdateData[i].iS2 / desc.Width), (aUpdateData[i].iT2 / desc.Height) };
            Vertex v6 = { aUpdateData[i].iX + aUpdateData[i].iW, aUpdateData[i].iY, 1.0f, 1.0f, (aUpdateData[i].iS2 / desc.Width), (aUpdateData[i].iT1 / desc.Height) };
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

void D3D8RenderList::LoadTexture(const char *szFilename)
{
    D3DXCreateTextureFromFile(m_pD3DDevice, szFilename, &m_pTexture);
}

void D3D8RenderList::FreeTexture()
{
    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }
}

void D3D8RenderList::Render()
{
    m_pD3DDevice->SetTexture(0, m_pTexture);
    m_pD3DDevice->SetStreamSource(0, m_pVB, sizeof(Vertex));
    m_pD3DDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

    if(m_kRLT == RLT_LINKED)
    {
        m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_iElements * 4);
    }
    else if(m_kRLT == RLT_NORMAL)
    {
        m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iElements * 2);
    }
}