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

#include "Animation.h"
#include "Defines.h"
#include "Menu.h"
#include "VideoDriver.h"

Menu::Menu(VideoDriver *pVD, Texture *pFont, Texture *pPointer, u32 iPointerOffsetX, u32 iPointerOffsetY)
{
    m_pFont = pFont;
    m_pPointer = pPointer;
    m_iPointerOffsetX = iPointerOffsetX;
    m_iPointerOffsetY = iPointerOffsetY;
    m_iCurrent = 0;
    m_Orient = MO_VERTICAL;
    m_pVD = pVD;
    m_pPointerAnim = NULL;
    m_bDrawTitle = false;
}

Menu::~Menu()
{
}

// FIXME not really flexible (pointer must be exactly 13x13)
void Menu::Render(int iX, int iY, int iZ, int iR, int iG, int iB)
{
    if(m_Orient == MO_VERTICAL)
    {
        u32 iYOff = 0;
        if(m_bDrawTitle)
        {
            // draw the title
            m_pVD->PrintText(m_pFont, iX + 13 + MENU_POINTER_SPACE, iY + 1, iZ, m_szTitle.c_str(), 1.0f, iR, iG, iB);
            m_pVD->DrawLine(iX + 13 + MENU_POINTER_SPACE, iY + FONT_CHAR_HEIGHT_1, iX + 13 + MENU_POINTER_SPACE + m_szTitle.length() * FONT_CHAR_WIDTH_1 - 1, iY + FONT_CHAR_HEIGHT_1, iZ, iR, iG, iB);
            iYOff = FONT_CHAR_HEIGHT_1 + MENU_TITLE_SPACE;
        }
        m_pVD->DrawSprite(m_pPointer, iX, iY + (m_iCurrent * FONT_CHAR_HEIGHT_1) + iYOff, iZ, m_iPointerOffsetX + (m_pPointerAnim == NULL ? 0 : m_pPointerAnim->GetCurrentFrame() * 13), m_iPointerOffsetY, 13, 13);
        for(int i = 0; i < m_Items.size(); i++)
        {
            m_pVD->PrintText(m_pFont, iX + 13 + MENU_POINTER_SPACE, iY + (i * FONT_CHAR_HEIGHT_1) + iYOff, iZ, m_Items[i].c_str(), 1.0f, iR, iG, iB);
        }
    }
    else if(m_Orient == MO_HORIZONTAL)
    {
        // draw the title
        int x = iX + m_szTitle.length() * FONT_CHAR_WIDTH_1 + MENU_TITLE_SPACE;
        int shift = 0;
        m_pVD->PrintText(m_pFont, iX, iY, iZ, m_szTitle.c_str(), 1.0f, iR, iG, iB);
        m_pVD->DrawLine(x, iY - 1, x, iY + FONT_CHAR_HEIGHT + 1, iZ, iR, iG, iB);
        for(int i = 0; i < m_Items.size(); i++)
        {
            shift += MENU_POINTER_SPACE + (i == 0 ? 0 : m_Items[i - 1].length() * FONT_CHAR_WIDTH_1) + 13 + MENU_POINTER_SPACE;
            m_pVD->PrintText(m_pFont, x + shift, iY, iZ, m_Items[i].c_str(), 1.0f, iR, iG, iB);
            if(m_iCurrent == i)
            {
                m_pVD->DrawSprite(m_pPointer, x + shift - 13 - MENU_POINTER_SPACE, iY, iZ, m_iPointerOffsetX + (m_pPointerAnim == NULL ? 0 : m_pPointerAnim->GetCurrentFrame() * 13), m_iPointerOffsetY, 13, 13);
            }
        }
    }
}

void Menu::NextItem()
{
    if(m_iCurrent < m_Items.size() - 1)
        m_iCurrent++;
}

void Menu::PrevItem()
{
    if(m_iCurrent > 0)
        m_iCurrent--;
}

void Menu::SetCurrentItem(u8 item)
{
    if(item < 0)
        m_iCurrent = 0;
    else if(item > m_Items.size() - 1)
        m_iCurrent = m_Items.size() - 1;
    else
        m_iCurrent = item;
}