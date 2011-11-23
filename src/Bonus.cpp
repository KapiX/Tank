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

#include "Bonus.h"

Texture *Bonus::m_pkBonusTexture = NULL;

Bonus::Bonus(BONUS_TYPE bt)
{
	m_bt = bt;
	m_pkAnim = new Animation();
	m_pkAnim->SetFrameRate(0.7f);
	m_pkAnim->SetMaxFrames(2);
	m_pkAnim->SetOscillate(true);
	m_pkAnim->SetPlaying(true);
	m_bIsAlive = false;
}

Bonus::~Bonus()
{
	if(m_pkAnim) {
		delete m_pkAnim;
		m_pkAnim = NULL;
	}
}

void Bonus::Randomize(u16 iX, u16 iY, u16 iW, u16 iH)
{
	m_iX = rand() % iW + iX;
	m_iY = rand() % iH + iY;
	m_bt = static_cast<BONUS_TYPE>((rand() % BONUS_BOAT) + 1);
	m_bIsAlive = true;
}

void Bonus::Update()
{
	m_pkAnim->Animate();
}

void Bonus::Render(VideoDriver *pVD)
{
	if(m_bIsAlive)
    {
        u32 iX2, iY2;
        iX2 = m_bt * 32;
        iY2 = m_pkAnim->GetCurrentFrame() * 32;
        pVD->DrawSprite(m_pkBonusTexture, m_iX, m_iY, 58.0f, iX2, iY2, 32, 32);
	}
}