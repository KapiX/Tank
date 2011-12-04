/*
	Copyright 2011 Kacper Kasper

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

#include "Menu.h"

Menu::Menu(VideoDriver *pVD, Texture *pFont, Texture *pPointer)
{
	m_pFont = pFont;
	m_pPointer = pPointer;
	m_iCurrent = 0;
	m_Orient = MO_VERTICAL;
	m_pVD = pVD;
	m_pPointerAnim = NULL;
}

Menu::~Menu()
{
}

void Menu::Render(int iX, int iY, int iZ, int iR, int iG, int iB)
{
	if(m_Orient == MO_VERTICAL)
	{
		m_pVD->DrawSprite(m_pPointer, iX, iY + (m_iCurrent * 13), iZ, 0 + (m_pPointerAnim == NULL ? 0 : m_pPointerAnim->GetCurrentFrame() * 13), 0, 13, 13);
		for(int i = 0; i < m_Items.size(); i++)
		{
            m_pVD->PrintText(m_pFont, iX + 13 + 3, iY + (i * 13) + 1, iZ, m_Items[i].c_str(), 1.0f, iR, iG, iB);
		}
	}
	else if(m_Orient == MO_HORIZONTAL)
	{
        int x = iX + m_szTitle.length() * (7 + 1) + 3;
		int shift = 0;
        m_pVD->PrintText(m_pFont, iX, iY, iZ, m_szTitle.c_str(), 1.0f, iR, iG, iB);
		m_pVD->DrawLine(x, iY - 1, x, iY + 13 + 1, iZ, iR, iG, iB);
		for(int i = 0; i < m_Items.size(); i++)
		{
            shift += 3 + (i == 0 ? 0 : m_Items[i - 1].length() * (7 + 1)) + m_pPointer->GetWidth() + 3;
            m_pVD->PrintText(m_pFont, x + shift, iY, iZ, m_Items[i].c_str(), 1.0f, iR, iG, iB);
			if(m_iCurrent == i)
			{
				m_pVD->DrawSprite(m_pPointer, x + shift - 13 - 3, iY, iZ, 0 + (m_pPointerAnim == NULL ? 0 : m_pPointerAnim->GetCurrentFrame() * 13), 0, 13, 13);
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

void Menu::AddItem(std::string szText)
{
    m_Items.push_back(szText);
}

void Menu::ChangeItem(u8 item, std::string szText)
{
    m_Items[item] = szText;
}