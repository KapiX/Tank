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
#include "Enemy.h"
#include "VideoDriver.h"

u8 Enemy::m_iEnemiesLeft = 0;
u8 Enemy::m_iBonusesLeft = 0;

Enemy::Enemy(VideoDriver *pVD, Texture *pTexture, bool bHasBonus, SHIELDLEVEL kSL, TANKLEVEL kTL, DIRECTION kDir, f32 iSpeed)
    : Tank(pVD, pTexture, kTL, kDir, iSpeed)
{
    if(kTL == TL_2) m_iSpeed *= 2;
    else if(kTL == TL_3) m_apkBullets[0]->SetSpeed(m_apkBullets[0]->GetSpeed() * 2);
    m_bHasBonus = bHasBonus;
    m_pkBonusAnim = new Animation();
    m_pkBonusAnim->SetFrameRate(1.0f);
    m_pkBonusAnim->SetMaxFrames(2);
    m_pkBonusAnim->SetOscillate(true);
    m_pkBonusAnim->SetPlaying(m_bHasBonus);
    m_kSL = kSL;
    m_bSpawn = false;
    m_bStopped = false;
    SetIsMoving(true);
}

Enemy::~Enemy()
{
    if(m_pkBonusAnim)
    {
        delete m_pkBonusAnim;
        m_pkBonusAnim = NULL;
    }
}

void Enemy::Stop()
{
    m_fStopTime = *m_pfTimer;
    m_bStopped = true;
}

void Enemy::Spawn()
{
    if(!m_bExplode && !m_bSpawn && !m_bIsAlive && m_iEnemiesLeft > 0)
    {
        Tank::Spawn(DIR_DOWN, m_kTL);
        m_iEnemiesLeft--;
        if(m_bHasBonus) m_iBonusesLeft--;
    }
}

void Enemy::Shoot()
{
    if (!m_bSpawn && !m_bExplode && m_bIsAlive)
    {
        if (!m_apkBullets[0]->m_bIsAlive && !m_apkBullets[0]->m_bExplode)
        {
            m_apkBullets[0]->m_kDir = m_kDir;
            m_apkBullets[0]->m_kTL = m_kTL;
            switch (m_kDir)
            {
            case DIR_UP:
                m_apkBullets[0]->m_iX = m_iX + ENEMY_WIDTH_DIV_2 - BULLET_WIDTH_DIV_2;
                m_apkBullets[0]->m_iY = m_iY;
                break;
            case DIR_RIGHT:
                m_apkBullets[0]->m_iX = m_iX + ENEMY_WIDTH - BULLET_WIDTH;
                m_apkBullets[0]->m_iY = m_iY + ENEMY_HEIGHT_DIV_2 - BULLET_HEIGHT_DIV_2;
                break;
            case DIR_DOWN:
                m_apkBullets[0]->m_iX = m_iX + ENEMY_WIDTH_DIV_2 - BULLET_WIDTH_DIV_2;
                m_apkBullets[0]->m_iY = m_iY + ENEMY_HEIGHT - BULLET_HEIGHT;
                break;
            case DIR_LEFT:
                m_apkBullets[0]->m_iX = m_iX;
                m_apkBullets[0]->m_iY = m_iY + ENEMY_HEIGHT_DIV_2 - BULLET_HEIGHT_DIV_2;
                break;
            }
            m_apkBullets[0]->m_bIsAlive = true;
            return;
        }
    }
}

void Enemy::Update(f32 fDelta)
{
    Tank::Update(fDelta);

    // Checking if time bonus is over
    if(*m_pfTimer - m_fStopTime > BONUS_STOP_TIME)
        m_bStopped = false;

    if(m_bIsAlive && m_bIsMoving && !m_bStopped)
    {
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

    m_pkBonusAnim->Animate();

    if(!m_bStopped)
    {
        m_pkAnim->Animate();

        static bool shot = false;
        static bool chdir = false;

        // random moves
        f32 time1 = (int) *m_pfTimer % 3;
        f32 time2 = (int) *m_pfTimer % ((rand() % 5) + 1);

        if(time1 == 0 && !chdir)
        {
            SetDirection((DIRECTION) (rand() % 4 + 1));
            chdir = true;
        }
        else if(time1 != 0 && chdir)
        {
            chdir = false;
        }
        if(time2 == 0 && !shot)
        {
            Shoot();
            shot = true;
        }
        else if(time2 != 0 && shot)
        {
            shot = false;
        }
    }
}

void Enemy::Render()
{
    if(m_bIsAlive)
    {
        // Enemy
        u32 iX2 =
            (m_bHasBonus ?
                m_pkBonusAnim->GetCurrentFrame() : m_kSL) * TANK_TYPE_WIDTH +
            (m_kDir - 1) * ENEMY_WIDTH;
        u32 iY2 =
            (m_kTL - 1) * TANK_LEVEL_HEIGHT +
            m_pkAnim->GetCurrentFrame() * ENEMY_HEIGHT;
        m_pVD->DrawSprite(
            m_pTexture,
            m_iX, m_iY,
            TANK_LAYER,
            iX2, iY2,
            ENEMY_WIDTH,
            ENEMY_HEIGHT
        );
    }
    else if(m_bSpawn)
    {
        // Spawn animation
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
}

void Enemy::OnExplode()
{
    if(m_pkExplAnim->GetCurrentFrame() == 7)
    {
        m_bExplode = false;
        m_pkExplAnim->SetPlaying(false);
    }
}

void Enemy::SetTankLevel(TANKLEVEL tl)
{
    Tank::SetTankLevel(tl);
    m_iSpeed = ENEMY_SPEED;
    m_apkBullets[0]->SetSpeed(ENEMY_BULLET_SPEED);
    if(tl == TL_2)
    {
        m_iSpeed = ENEMY_LVL2_SPEED;
    }
    else if(tl == TL_3)
    {
        m_apkBullets[0]->SetSpeed(ENEMY_LVL3_BULLET_SPEED);
    }
}