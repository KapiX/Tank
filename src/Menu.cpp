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

void Menu::Render(int iX, int iY)
{
	if(m_Orient == MO_VERTICAL)
	{
		m_pVD->DrawSprite(m_pPointer, iX, iY + (m_iCurrent * 13), 64, 0 + (m_pPointerAnim == NULL ? 0 : m_pPointerAnim->GetCurrentFrame() * 13), 0, 13, 13);
		int i = 0;
		for(m_Items.StartIterator(); m_Items.IsIteratorValid(); m_Items.MoveForward())
		{
			m_pVD->PrintText(m_pFont, iX + 13 + 3, iY + (i * 13) + 1, 64, m_Items.GetCurrentItem());
			i++;
		}
	}
	else if(m_Orient == MO_HORIZONTAL)
	{
		int x = iX + strlen(m_szTitle) * (7 + 1) + 3;
		int shift = 0;
		m_pVD->PrintText(m_pFont, iX, iY, 64, m_szTitle, 1.0f, 0, 0, 0);
		m_pVD->DrawLine(x, iY - 1, x, iY + 13 + 1, 64, 0, 0, 0);
		int i = 0;
		for(m_Items.StartIterator(); m_Items.IsIteratorValid(); m_Items.MoveForward())
		{
			shift += 3 + (i == 0 ? 0 : strlen(m_Items.GetIterator()->prev->item) * (7 + 1)) + m_pPointer->GetWidth() + 3;
			m_pVD->PrintText(m_pFont, x + shift, iY, 64, m_Items.GetCurrentItem(), 1.0f, 0, 0, 0);
			if(m_iCurrent == i)
			{
				m_pVD->DrawSprite(m_pPointer, x + shift - 13 - 3, iY, 64, 0 + (m_pPointerAnim == NULL ? 0 : m_pPointerAnim->GetCurrentFrame() * 13), 0, 13, 13);
			}
			i++;
		}
	}
}

void Menu::NextItem()
{
	if(m_iCurrent < m_Items.GetCount() - 1)
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
	else if(item > m_Items.GetCount() - 1)
		m_iCurrent = m_Items.GetCount() - 1;
	else
		m_iCurrent = item;
}

void Menu::AddItem(const char *szText)
{
	m_Items.AppendItem(szText);
}