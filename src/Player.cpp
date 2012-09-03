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
#include "Player.h"
#include "SoundManager.h"
#include "VideoDriver.h"

Player::Player(VideoDriver *pVD, u8 iNum, Texture *pTexture, TANKLEVEL kTL, DIRECTION kDir, f32 iSpeed)
    : Tank(pVD, pTexture, kTL, kDir, iSpeed)
{
    m_pkShieldAnim = new Animation();
    m_pkShieldAnim->SetFrameRate(0.025f);
    m_pkShieldAnim->SetMaxFrames(2);
    m_pkShieldAnim->SetOscillate(true);
    m_pkShieldAnim->SetPlaying(true);
    m_bShield = false;
    m_bSpawn = false;
    m_iNum = iNum;
    m_bBoat = false;
    m_fSlide = 0.0f;
    m_bSliding = false;
}

Player::~Player()
{
    if(m_pkShieldAnim)
    {
        delete m_pkShieldAnim;
        m_pkShieldAnim = NULL;
    }
}

void Player::Spawn()
{
    if(!m_bExplode && !m_bSpawn && !m_bIsAlive && m_iLives > 0)
    {
        Tank::Spawn(DIR_UP, m_kTL);
        m_iLives--;
    }
}

void Player::Shoot()
{
    if (!m_bSpawn && !m_bExplode)
    {
        if (!m_apkBullets[0]->m_bIsAlive && !m_apkBullets[0]->m_bExplode)
        {
            m_apkBullets[0]->m_kDir = m_kDir;
            m_apkBullets[0]->m_kTL = m_kTL;
            switch (m_kDir)
            {
            case DIR_UP:
                m_apkBullets[0]->m_iX = m_iX + PLAYER_WIDTH_DIV_2 - BULLET_WIDTH_DIV_2;
                m_apkBullets[0]->m_iY = m_iY;
                break;
            case DIR_RIGHT:
                m_apkBullets[0]->m_iX = m_iX + PLAYER_WIDTH;
                m_apkBullets[0]->m_iY = m_iY + PLAYER_HEIGHT_DIV_2 - BULLET_HEIGHT_DIV_2;
                break;
            case DIR_DOWN:
                m_apkBullets[0]->m_iX = m_iX + PLAYER_WIDTH_DIV_2 - BULLET_WIDTH_DIV_2;
                m_apkBullets[0]->m_iY = m_iY + PLAYER_HEIGHT;
                break;
            case DIR_LEFT:
                m_apkBullets[0]->m_iX = m_iX;
                m_apkBullets[0]->m_iY = m_iY + PLAYER_HEIGHT_DIV_2 - BULLET_HEIGHT_DIV_2;
                break;
            }
            m_apkBullets[0]->m_bIsAlive = true;
            SoundManager::GetInstance()->Play(SND_SHOOT);
            return;
        }
        else if (!m_apkBullets[1]->m_bIsAlive && !m_apkBullets[1]->m_bExplode && (m_kTL == TL_3 || m_kTL == TL_4))
        {
            m_apkBullets[1]->m_kDir = m_kDir;
            m_apkBullets[1]->m_kTL = m_kTL;
            switch (m_kDir)
            {
            case DIR_UP:
                m_apkBullets[1]->m_iX = m_iX + PLAYER_WIDTH_DIV_2 - BULLET_WIDTH_DIV_2;
                m_apkBullets[1]->m_iY = m_iY;
                break;
            case DIR_RIGHT:
                m_apkBullets[1]->m_iX = m_iX + PLAYER_WIDTH;
                m_apkBullets[1]->m_iY = m_iY + PLAYER_HEIGHT_DIV_2 - BULLET_HEIGHT_DIV_2;
                break;
            case DIR_DOWN:
                m_apkBullets[1]->m_iX = m_iX + PLAYER_WIDTH_DIV_2 - BULLET_WIDTH_DIV_2;
                m_apkBullets[1]->m_iY = m_iY + PLAYER_HEIGHT;
                break;
            case DIR_LEFT:
                m_apkBullets[1]->m_iX = m_iX;
                m_apkBullets[1]->m_iY = m_iY + PLAYER_HEIGHT_DIV_2 - BULLET_HEIGHT_DIV_2;
                break;
            }
            m_apkBullets[1]->m_bIsAlive = true;
            SoundManager::GetInstance()->Play(SND_SHOOT);
            return;
        }
    }
}

void Player::Update(f32 fDelta)
{
    Tank::Update(fDelta);

    // prevent change of direction
    if(m_fSlide > 0)
    {
        m_bIsMoving = false;
        m_kDir = m_kSlidingDir;
    }
    else
    {
        m_bSliding = false;
    }

    if(m_bIsAlive)
    {
        if(m_bIsMoving)
        {
            m_pkAnim->Animate();
            switch(m_kDir)
            {
            case DIR_LEFT:
                m_iX -= m_iSpeed * fDelta;
                break;
            case DIR_RIGHT:
                m_iX += m_iSpeed * fDelta;
                break;
            case DIR_UP:
                m_iY -= m_iSpeed * fDelta;
                break;
            case DIR_DOWN:
                m_iY += m_iSpeed * fDelta;
                break;
            }
        }
        if(m_bSliding && !m_bIsMoving)
        {
            m_pkAnim->Animate();
            m_fSlide -= m_iSpeed * fDelta;
            switch(m_kSlidingDir)
            {
            case DIR_LEFT:
                m_iX -= m_iSpeed * fDelta;
                break;
            case DIR_RIGHT:
                m_iX += m_iSpeed * fDelta;
                break;
            case DIR_UP:
                m_iY -= m_iSpeed * fDelta;
                break;
            case DIR_DOWN:
                m_iY += m_iSpeed * fDelta;
                break;
            }
        }

        if(m_bShield)
        {
            m_pkShieldAnim->Animate();
        }
    }
    if(m_bSpawn && m_pkSpawnAnim->GetFrameIterator() == 14)
    {
        ActivateShield(SPAWN_SHIELD_TIME);
    }

    if(m_iShieldTime < *m_pfTimer - m_fOldTime)
    {
        m_bShield = false;
        m_iShieldTime = 0;
        m_fOldTime = 0;
    }
}

void Player::Render()
{
    if(m_bIsAlive)
    {
        u32 iX2 =
            PLAYER_OFFSET_X + (m_iNum - 1) * TANK_TYPE_WIDTH +
            (m_kDir - 1) * PLAYER_WIDTH;
        u32 iY2 =
            PLAYER_OFFSET_Y + (m_kTL - 1) * TANK_LEVEL_HEIGHT +
            m_pkAnim->GetCurrentFrame() * PLAYER_HEIGHT;
        m_pVD->DrawSprite(
            m_pTexture,
            m_iX, m_iY,
            TANK_LAYER,
            iX2, iY2,
            PLAYER_WIDTH,
            PLAYER_HEIGHT
        );
    }
    else if(m_bSpawn)
    {
        m_pVD->DrawSprite(
            m_pTexture,
            m_iX, m_iY,
            SPAWN_ANIM_LAYER,
            SPAWN_ANIM_OFFSET_X +
                m_pkSpawnAnim->GetCurrentFrame() * SPAWN_ANIM_WIDTH,
            SPAWN_ANIM_OFFSET_Y,
            SPAWN_ANIM_WIDTH,
            SPAWN_ANIM_HEIGHT
        );
    }
    else if(m_bExplode)
    {
        u32 iX, iY, iW = EXPLOSION_ANIM_WIDTH, iH = EXPLOSION_ANIM_HEIGHT;
        if(m_pkExplAnim->GetCurrentFrame() == 3)
        {
            iX = m_iX - EXPLOSION_ANIM_WIDTH_DIV_2;
            iY = m_iY - EXPLOSION_ANIM_HEIGHT_DIV_2;
            iW = EXPLOSION_ANIM_WIDTH_2;
            iH = EXPLOSION_ANIM_HEIGHT_2;
        }
        else
        {
            iX = m_iX;
            iY = m_iY;
        }
        u32 iFr = m_pkExplAnim->GetCurrentFrame();
        m_pVD->DrawSprite(
            m_pTexture,
            iX,
            iY,
            EXPLOSION_ANIM_LAYER,
            EXPLOSION_ANIM_OFFSET_X +
                (iFr + (iFr < 4 ? 0 : 1)) * EXPLOSION_ANIM_WIDTH,
            EXPLOSION_ANIM_OFFSET_Y,
            iW,
            iH
        );
    }
    m_apkBullets[0]->Render();
    m_apkBullets[1]->Render();

    if(m_bIsAlive)
    {
        if(m_bShield)
        {
            m_pVD->DrawSprite(
                m_pTexture,
                m_iX, m_iY,
                SHIELD_LAYER,
                SHIELD_OFFSET_X +
                    m_pkShieldAnim->GetCurrentFrame() * SHIELD_WIDTH,
                SHIELD_OFFSET_Y,
                SHIELD_WIDTH,
                SHIELD_HEIGHT);
        }
        if(m_bBoat)
        {
            m_pVD->DrawSprite(
                m_pTexture,
                m_iX, m_iY,
                BOAT_LAYER,
                BOAT_OFFSET_X + BOAT_WIDTH * (m_iNum - 1),
                BOAT_OFFSET_Y,
                BOAT_WIDTH,
                BOAT_HEIGHT
            );
        }
    }
}

void Player::OnExplode()
{
    if(m_pkExplAnim->GetCurrentFrame() == 7)
    {
        m_bExplode = false;
        m_pkExplAnim->SetPlaying(false);
        m_apkBullets[0]->m_bDestroyJungle = false;
        m_apkBullets[1]->m_bDestroyJungle = false;
    }
}

void Player::ActivateShield(f32 iTime)
{
    m_bShield = true;
    m_iShieldTime = iTime;
    m_fOldTime = *m_pfTimer;
}

void Player::Slide(f32 fSlide)
{
    m_bSliding = true;
    m_fSlide = fSlide;
    m_kSlidingDir = m_kDir;
}