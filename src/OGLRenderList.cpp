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

#include "OGLRenderList.h"

OGLRenderList::OGLRenderList(RENDERLIST_TYPE rlt, u32 iElements) :
RenderList(rlt, iElements),
    m_iCount(0),
    m_bUseColors(false),
    m_aColors(NULL)
{
    int iVertsPerSprite = (rlt == RLT_LINKED ? 4 : 6);

    m_aVerts = new float[m_iElements * 3 * iVertsPerSprite];
    m_aTexCoords = new float[m_iElements * 2 * iVertsPerSprite];
}


OGLRenderList::~OGLRenderList(void)
{
    if(GLEW_ARB_vertex_buffer_object)
    {
        glDeleteBuffersARB(1, &m_iVBOVCID);
        glDeleteBuffersARB(1, &m_iVBOTCID);
        if(m_bUseColors)
        {
            glDeleteBuffersARB(1, &m_iVBOCID);
        }
    }
    delete [] m_aVerts;
    delete [] m_aTexCoords;
    if(m_bUseColors)
    {
        delete [] m_aColors;
    }
}

void OGLRenderList::AddElement(Sprite *pSprite, Color *pColor)
{
    if(m_iCount >= m_iElements)
    {
        return;
    }

    if(pColor != NULL && m_aColors == NULL && !m_bUseColors)
    {
        m_aColors = new float[m_iElements * 4 * (m_kRLT == RLT_LINKED ? 4 : 6)];
        m_bUseColors = true;
    }

    if(m_kRLT == RLT_LINKED)
    {
        // vertex 1
        m_aVerts[m_iCount * 3 * 4] = pSprite->iX;
        m_aVerts[m_iCount * 3 * 4 + 1] = pSprite->iY;
        m_aVerts[m_iCount * 3 * 4 + 2] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 4] = pSprite->iS1 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 4 + 1] = pSprite->iT1 / m_pTexture->GetHeight();
        // vertex 2
        m_aVerts[m_iCount * 3 * 4 + 3] = pSprite->iX + pSprite->iW;
        m_aVerts[m_iCount * 3 * 4 + 4] = pSprite->iY;
        m_aVerts[m_iCount * 3 * 4 + 5] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 4 + 2] = pSprite->iS2 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 4 + 3] = pSprite->iT1 / m_pTexture->GetHeight();
        // vertex 3
        m_aVerts[m_iCount * 3 * 4 + 6] = pSprite->iX;
        m_aVerts[m_iCount * 3 * 4 + 7] = pSprite->iY + pSprite->iH;
        m_aVerts[m_iCount * 3 * 4 + 8] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 4 + 4] = pSprite->iS1 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 4 + 5] = pSprite->iT2 / m_pTexture->GetHeight();
        // vertex 4
        m_aVerts[m_iCount * 3 * 4 + 9] = pSprite->iX + pSprite->iW;
        m_aVerts[m_iCount * 3 * 4 + 10] = pSprite->iY + pSprite->iH;
        m_aVerts[m_iCount * 3 * 4 + 11] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 4 + 6] = pSprite->iS2 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 4 + 7] = pSprite->iT2 / m_pTexture->GetHeight();
        if(m_bUseColors)
        {
            for(int i = 0; i < 4; i++)
            {
                m_aColors[m_iCount * 4 * 4 + (i * 4)] = pColor->iR / 255.0f;
                m_aColors[m_iCount * 4 * 4 + (i * 4) + 1] = pColor->iG / 255.0f;
                m_aColors[m_iCount * 4 * 4 + (i * 4) + 2] = pColor->iB / 255.0f;
                m_aColors[m_iCount * 4 * 4 + (i * 4) + 3] = pColor->iA / 255.0f;
            }
        }
    }
    else if(m_kRLT == RLT_NORMAL)
    {
        // vertex 1
        m_aVerts[m_iCount * 3 * 6] = pSprite->iX;
        m_aVerts[m_iCount * 3 * 6 + 1] = pSprite->iY;
        m_aVerts[m_iCount * 3 * 6 + 2] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 6] = pSprite->iS1 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 6 + 1] = pSprite->iT1 / m_pTexture->GetHeight();
        // vertex 2
        m_aVerts[m_iCount * 3 * 6 + 3] = pSprite->iX + pSprite->iW;
        m_aVerts[m_iCount * 3 * 6 + 4] = pSprite->iY;
        m_aVerts[m_iCount * 3 * 6 + 5] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 6 + 2] = pSprite->iS2 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 6 + 3] = pSprite->iT1 / m_pTexture->GetHeight();
        // vertex 3
        m_aVerts[m_iCount * 3 * 6 + 6] = pSprite->iX;
        m_aVerts[m_iCount * 3 * 6 + 7] = pSprite->iY + pSprite->iH;
        m_aVerts[m_iCount * 3 * 6 + 8] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 6 + 4] = pSprite->iS1 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 6 + 5] = pSprite->iT2 / m_pTexture->GetHeight();
        // vertex 4
        m_aVerts[m_iCount * 3 * 6 + 9] = pSprite->iX;
        m_aVerts[m_iCount * 3 * 6 + 10] = pSprite->iY + pSprite->iH;
        m_aVerts[m_iCount * 3 * 6 + 11] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 6 + 6] = pSprite->iS1 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 6 + 7] = pSprite->iT2 / m_pTexture->GetHeight();
        // vertex 5
        m_aVerts[m_iCount * 3 * 6 + 12] = pSprite->iX + pSprite->iW;
        m_aVerts[m_iCount * 3 * 6 + 13] = pSprite->iY + pSprite->iH;
        m_aVerts[m_iCount * 3 * 6 + 14] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 6 + 8] = pSprite->iS2 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 6 + 9] = pSprite->iT2 / m_pTexture->GetHeight();
        // vertex 6
        m_aVerts[m_iCount * 3 * 6 + 15] = pSprite->iX + pSprite->iW;
        m_aVerts[m_iCount * 3 * 6 + 16] = pSprite->iY;
        m_aVerts[m_iCount * 3 * 6 + 17] = pSprite->iZ;
        m_aTexCoords[m_iCount * 2 * 6 + 10] = pSprite->iS2 / m_pTexture->GetWidth();
        m_aTexCoords[m_iCount * 2 * 6 + 11] = pSprite->iT1 / m_pTexture->GetHeight();
        if(m_bUseColors)
        {
            for(int i = 0; i < 6; i++)
            {
                m_aColors[m_iCount * 4 * 6 + (i * 4)] = pColor->iR / 255.0f;
                m_aColors[m_iCount * 4 * 6 + (i * 4) + 1] = pColor->iG / 255.0f;
                m_aColors[m_iCount * 4 * 6 + (i * 4) + 2] = pColor->iB / 255.0f;
                m_aColors[m_iCount * 4 * 6 + (i * 4) + 3] = pColor->iA / 255.0f;
            }
        }
    }

    m_iCount++;
}

void OGLRenderList::FillBuffer()
{
    int iVertsPerSprite = (m_kRLT == RLT_LINKED ? 4 : 6);
    if(GLEW_ARB_vertex_buffer_object)
    {
        glGenBuffersARB(1, &m_iVBOVCID);
        glGenBuffersARB(1, &m_iVBOTCID);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOTCID);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_iElements * 2 * iVertsPerSprite * sizeof(float), m_aTexCoords, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOVCID);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_iElements * 3 * iVertsPerSprite * sizeof(float), m_aVerts, GL_STREAM_DRAW_ARB);

        if(m_bUseColors)
        {
            glGenBuffersARB(1, &m_iVBOCID);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOCID);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_iElements * 4 * iVertsPerSprite * sizeof(float), m_aColors, GL_STREAM_DRAW_ARB);
        }
    }
}

void OGLRenderList::UpdateBuffer(u32 iCount, UpdateData *aUpdateData, Color *pUpdateColor)
{
    int iVertsPerSprite = (m_kRLT == RLT_LINKED ? 4 : 6);

    for(u32 i = 0; i < iCount; i++)
    {
        if(m_kRLT == RLT_LINKED)
        {
            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 4] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 1] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 2] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 3] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite] = aUpdateData[i].iX;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 1] = aUpdateData[i].iY;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 2] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite] = aUpdateData[i].iS1 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 1] = aUpdateData[i].iT1 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 4] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 5] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 6] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 7] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 3] = aUpdateData[i].iX + aUpdateData[i].iW;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 4] = aUpdateData[i].iY;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 5] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 2] = aUpdateData[i].iS2 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 3] = aUpdateData[i].iT1 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 8] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 9] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 10] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 11] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 6] = aUpdateData[i].iX;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 7] = aUpdateData[i].iY + aUpdateData[i].iH;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 8] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 4] = aUpdateData[i].iS1 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 5] = aUpdateData[i].iT2 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 12] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 13] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 14] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 4 + 15] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 9] = aUpdateData[i].iX + aUpdateData[i].iW;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 10] = aUpdateData[i].iY + aUpdateData[i].iH;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 11] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 6] = aUpdateData[i].iS2 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 7] = aUpdateData[i].iT2 / m_pTexture->GetHeight();
        }
        else if(m_kRLT == RLT_NORMAL)
        {
            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 6] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 1] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 2] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 3] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite] = aUpdateData[i].iX;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 1] = aUpdateData[i].iY;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 2] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite] = aUpdateData[i].iS1 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 1] = aUpdateData[i].iT1 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 4] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 5] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 6] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 7] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 3] = aUpdateData[i].iX + aUpdateData[i].iW;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 4] = aUpdateData[i].iY;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 5] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 2] = aUpdateData[i].iS2 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 3] = aUpdateData[i].iT1 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 8] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 9] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 10] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 11] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 6] = aUpdateData[i].iX;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 7] = aUpdateData[i].iY + aUpdateData[i].iH;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 8] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 4] = aUpdateData[i].iS1 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 5] = aUpdateData[i].iT2 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 12] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 13] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 14] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 15] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 9] = aUpdateData[i].iX;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 10] = aUpdateData[i].iY + aUpdateData[i].iH;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 11] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 6] = aUpdateData[i].iS1 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 7] = aUpdateData[i].iT2 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 16] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 17] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 18] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 19] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 12] = aUpdateData[i].iX + aUpdateData[i].iW;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 13] = aUpdateData[i].iY + aUpdateData[i].iH;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 14] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 8] = aUpdateData[i].iS2 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 9] = aUpdateData[i].iT2 / m_pTexture->GetHeight();

            if(m_bUseColors && pUpdateColor != NULL)
            {
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 20] = pUpdateColor->iR / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 21] = pUpdateColor->iG / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 22] = pUpdateColor->iB / 255.0f;
                m_aColors[aUpdateData[i].iNum * 4 * 6 + 23] = pUpdateColor->iA / 255.0f;
            }
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 15] = aUpdateData[i].iX + aUpdateData[i].iW;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 16] = aUpdateData[i].iY;
            m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite + 17] = aUpdateData[i].iZ;
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 10] = aUpdateData[i].iS2 / m_pTexture->GetWidth();
            m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite + 11] = aUpdateData[i].iT1 / m_pTexture->GetHeight();
        }
        if(GLEW_ARB_vertex_buffer_object)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOVCID);
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, aUpdateData[i].iNum * 3 * iVertsPerSprite * sizeof(float), 3 * iVertsPerSprite * sizeof(float), &m_aVerts[aUpdateData[i].iNum * 3 * iVertsPerSprite]);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOTCID);
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, aUpdateData[i].iNum * 2 * iVertsPerSprite * sizeof(float), 2 * iVertsPerSprite * sizeof(float), &m_aTexCoords[aUpdateData[i].iNum * 2 * iVertsPerSprite]);
            if(m_bUseColors)
            {
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOCID);
                glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, aUpdateData[i].iNum * 4 * iVertsPerSprite * sizeof(float), 4 * iVertsPerSprite * sizeof(float), &m_aColors[aUpdateData[i].iNum * 4 * iVertsPerSprite]);
            }
        }
    }
}

void OGLRenderList::Render()
{
    glBindTexture(GL_TEXTURE_2D, m_pTexture->GetID());

    if(GLEW_ARB_vertex_buffer_object)
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOTCID);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOVCID);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        if(m_bUseColors)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_iVBOCID);
            glColorPointer(4, GL_FLOAT, 0, 0);

            glEnableClientState(GL_COLOR_ARRAY);
        }
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);

        if(m_kRLT == RLT_LINKED)
        {
            glDrawArrays(GL_TRIANGLE_STRIP, 0, m_iElements * 4);
        }
        else if(m_kRLT == RLT_NORMAL)
        {
            glDrawArrays(GL_TRIANGLES, 0, m_iElements * 6);
        }

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        if(m_bUseColors)
        {
            glDisableClientState(GL_COLOR_ARRAY);
        }
    }
    else
    {
        if(m_kRLT == RLT_LINKED)
        {
            glBegin(GL_TRIANGLE_STRIP);
            if(m_bUseColors)
            {
                glColor4f(m_aColors[0], m_aColors[1], m_aColors[2], m_aColors[3]);
            }
            else
            {
                glColor4f(1, 1, 1, 1);
            }
            for(u32 i = 0; i < m_iElements; i++)
            {
                glTexCoord2f(m_aTexCoords[i * 2 * 4], m_aTexCoords[i * 2 * 4 + 1]);
                glVertex3f(m_aVerts[i * 3 * 4], m_aVerts[i * 3 * 4 + 1], m_aVerts[i * 3 * 4 + 2]);
                glTexCoord2f(m_aTexCoords[i * 2 * 4 + 2], m_aTexCoords[i * 2 * 4 + 3]);
                glVertex3f(m_aVerts[i * 3 * 4 + 3], m_aVerts[i * 3 * 4 + 4], m_aVerts[i * 3 * 4 + 5]);
                glTexCoord2f(m_aTexCoords[i * 2 * 4 + 4], m_aTexCoords[i * 2 * 4 + 5]);
                glVertex3f(m_aVerts[i * 3 * 4 + 6], m_aVerts[i * 3 * 4 + 7], m_aVerts[i * 3 * 4 + 8]);
                glTexCoord2f(m_aTexCoords[i * 2 * 4 + 6], m_aTexCoords[i * 2 * 4 + 7]);
                glVertex3f(m_aVerts[i * 3 * 4 + 9], m_aVerts[i * 3 * 4 + 10], m_aVerts[i * 3 * 4 + 11]);
            }
            glEnd();
        }
        else if(m_kRLT == RLT_NORMAL)
        {
            glBegin(GL_TRIANGLES);
            if(m_bUseColors)
            {
                glColor4f(m_aColors[0], m_aColors[1], m_aColors[2], m_aColors[3]);
            }
            else
            {
                glColor4f(1, 1, 1, 1);
            }
            for(u32 i = 0; i < m_iElements; i++)
            {
                glTexCoord2f(m_aTexCoords[i * 2 * 6], m_aTexCoords[i * 2 * 6 + 1]);
                glVertex3f(m_aVerts[i * 3 * 6], m_aVerts[i * 3 * 6 + 1], m_aVerts[i * 3 * 6 + 2]);
                glTexCoord2f(m_aTexCoords[i * 2 * 6 + 2], m_aTexCoords[i * 2 * 6 + 3]);
                glVertex3f(m_aVerts[i * 3 * 6 + 3], m_aVerts[i * 3 * 6 + 4], m_aVerts[i * 3 * 6 + 5]);
                glTexCoord2f(m_aTexCoords[i * 2 * 6 + 4], m_aTexCoords[i * 2 * 6 + 5]);
                glVertex3f(m_aVerts[i * 3 * 6 + 6], m_aVerts[i * 3 * 6 + 7], m_aVerts[i * 3 * 6 + 8]);
                glTexCoord2f(m_aTexCoords[i * 2 * 6 + 6], m_aTexCoords[i * 2 * 6 + 7]);
                glVertex3f(m_aVerts[i * 3 * 6 + 9], m_aVerts[i * 3 * 6 + 10], m_aVerts[i * 3 * 6 + 11]);
                glTexCoord2f(m_aTexCoords[i * 2 * 6 + 8], m_aTexCoords[i * 2 * 6 + 9]);
                glVertex3f(m_aVerts[i * 3 * 6 + 12], m_aVerts[i * 3 * 6 + 13], m_aVerts[i * 3 * 6 + 14]);
                glTexCoord2f(m_aTexCoords[i * 2 * 6 + 10], m_aTexCoords[i * 2 * 6 + 11]);
                glVertex3f(m_aVerts[i * 3 * 6 + 15], m_aVerts[i * 3 * 6 + 16], m_aVerts[i * 3 * 6 + 17]);
            }
            glEnd();
        }
    }
}