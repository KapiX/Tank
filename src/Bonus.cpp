/*
    Copyright 2011-2014 Kacper Kasper <kacperkasper@gmail.com>

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
#include "Bonus.h"
#include "Defines.h"
#include "Texture.h"
#include "VideoDriver.h"

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
        iX2 = BONUS_OFFSET_X + m_pkAnim->GetCurrentFrame() * BONUS_WIDTH;
        iY2 = m_bt * BONUS_HEIGHT;
        pVD->DrawSprite(
            m_pkBonusTexture,
            m_iX, m_iY,
            BONUS_LAYER,
            iX2, iY2,
            BONUS_WIDTH,
            BONUS_HEIGHT
        );
    }
}

bool Bonus::IsColliding(u32 iX, u32 iY, u32 iW, u32 iH)
{
    u32 top1, top2, bottom1, bottom2, left1, left2, right1, right2;
    top1 = m_iY;
    bottom2 = iY + iH;
    left1 = m_iX;
    right2 = iX + iW;
    bottom1 = m_iY + BONUS_HEIGHT;
    top2 = iY;
    right1 = m_iX + BONUS_WIDTH;
    left2 = iX;
    return ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2));
}