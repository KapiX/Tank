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

#include "Tank.h"

Tank::Bullet::Bullet(VideoDriver *pVD, Texture *pTexture, TANKLEVEL kTL, DIRECTION kDir, f32 iSpeed)
{
    m_pVD = pVD;

    m_pTexture = pTexture;
    m_kTL = kTL;
    m_iSpeed = iSpeed;
    m_kDir = kDir;
    m_bExplode = false;
    m_bIsAlive = false;
    m_bDestroyJungle = false;

    m_pkExplAnim = new Animation();
    m_pkExplAnim->SetFrameRate(0.05f);
    m_pkExplAnim->SetMaxFrames(3 + 1);
    m_pkExplAnim->SetOscillate(false);
    m_pkExplAnim->SetPlaying(false);
}

Tank::Bullet::~Bullet()
{
    if(m_pkExplAnim != NULL)
    {
        delete m_pkExplAnim;
        m_pkExplAnim = NULL;
    }
}

void Tank::Bullet::Update(f32 fDelta)
{
    if(!m_bExplode && m_bIsAlive)
    {
        switch(m_kDir)
        {
        case DIR_UP:
            m_iY -= m_iSpeed * fDelta;
            break;
        case DIR_DOWN:
            m_iY += m_iSpeed * fDelta;
            break;
        case DIR_LEFT:
            m_iX -= m_iSpeed * fDelta;
            break;
        case DIR_RIGHT:
            m_iX += m_iSpeed * fDelta;
            break;
        }
    }
    else if(m_bExplode)
    {
        m_pkExplAnim->Animate();
        if(m_pkExplAnim->GetCurrentFrame() == 3)
        {
            m_bExplode = false;
            m_pkExplAnim->SetPlaying(false);
        }
    }
}

void Tank::Bullet::Render()
{
    if(m_bIsAlive)
    {
        u32 iX2, iY2;
        iX2 = (m_kDir - 1) * 8 + 292;
        iY2 = 288;
        m_pVD->DrawSprite(m_pTexture, m_iX, m_iY, 4.0f, iX2, iY2, 8, 8);
    }
    else if(m_bExplode)
    {
        u32 iX, iY, iW = 32, iH = 32;
        iX = m_iX - 16;
        iY = m_iY - 16;
        switch(m_kDir)
        {
        case DIR_UP:
        case DIR_DOWN:
            iX += 4;
            break;
        case DIR_RIGHT:
        case DIR_LEFT:
            iY += 4;
            break;
        }
        m_pVD->DrawSprite(m_pTexture, iX, iY, 44.0f, 228 + m_pkExplAnim->GetCurrentFrame() * 32, 256, iW, iH);
    }
}

void Tank::Bullet::Destroy(bool bExplode)
{
    m_bIsAlive = false;
    m_bExplode = bExplode;
    m_pkExplAnim->SetPlaying(true);
}

f32 *Tank::m_pfTimer = NULL;

Tank::Tank(VideoDriver *pVD, Texture *pTexture, TANKLEVEL kTL, DIRECTION kDir, f32 iSpeed)
{
    m_pVD = pVD;
    m_kTL = kTL;
    m_kDir = kDir;
    m_iSpeed = iSpeed;
    m_pTexture = pTexture;

    m_apkBullets[0] = new Bullet(pVD, pTexture, kTL, kDir, iSpeed * 3);
    m_apkBullets[1] = new Bullet(pVD, pTexture, kTL, kDir, iSpeed * 3);

    m_pkAnim = new Animation();
    m_pkAnim->SetFrameRate(0.05f);
    m_pkAnim->SetMaxFrames(2);
    m_pkAnim->SetOscillate(true);
    m_pkAnim->SetPlaying(false);

    u8 *frames = new u8[16];
    frames[0] = 0;
    frames[1] = 1;
    frames[2] = 2;
    frames[3] = 3;
    frames[4] = 4;
    frames[5] = 5;
    frames[6] = 0;
    frames[7] = 1;
    frames[8] = 2;
    frames[9] = 3;
    frames[10] = 4;
    frames[11] = 5;
    frames[12] = 6;
    frames[13] = 7;
    frames[14] = 8;
    frames[15] = 9;

    m_pkSpawnAnim = new Animation();
    m_pkSpawnAnim->SetFrames(frames);
    m_pkSpawnAnim->SetFrameRate(0.07f);
    m_pkSpawnAnim->SetMaxFrames(15 + 1); // needed for checking whether animation is over
    m_pkSpawnAnim->SetOscillate(false);
    m_pkSpawnAnim->SetPlaying(false);

    m_pkExplAnim = new Animation();
    m_pkExplAnim->SetFrameRate(0.05f);
    m_pkExplAnim->SetMaxFrames(7 + 1);
    m_pkExplAnim->SetOscillate(false);
    m_pkExplAnim->SetPlaying(false);

    m_bExplode = false;
    m_bSpawn = false;
    m_bIsMoving = false;
    m_bIsAlive = false;
}

Tank::~Tank()
{
    if(m_apkBullets[0])
    {
        delete m_apkBullets[0];
        m_apkBullets[0] = NULL;
    }
    if(m_apkBullets[1])
    {
        delete m_apkBullets[1];
        m_apkBullets[1] = NULL;
    }
    if(m_pkAnim)
    {
        delete m_pkAnim;
        m_pkAnim = NULL;
    }
    if(m_pkExplAnim)
    {
        delete m_pkExplAnim;
        m_pkExplAnim = NULL;
    }
    if(m_pkSpawnAnim)
    {
        delete m_pkSpawnAnim;
        m_pkSpawnAnim = NULL;
    }
}

void Tank::Spawn(DIRECTION kDir, TANKLEVEL kTL)
{
    m_iX = (f32) m_iSpawnX * 32;
    m_iY = (f32) m_iSpawnY * 32 + 24;
    m_kDir = kDir;
    m_kTL = kTL;
    m_bIsAlive = false;
    m_bSpawn = true;
    m_pkSpawnAnim->SetPlaying(true);
}

void Tank::Destroy()
{
    m_bIsAlive = false;
    m_bExplode = true;
    m_pkExplAnim->SetPlaying(true);
    m_kTL = TL_1;
    m_apkBullets[0]->m_iSpeed = m_iSpeed * 3;
    m_apkBullets[1]->m_iSpeed = m_iSpeed * 3;
    m_apkBullets[0]->m_bDestroyJungle = false;
    m_apkBullets[1]->m_bDestroyJungle = false;
}

void Tank::Update(f32 fDelta)
{
    m_apkBullets[0]->Update(fDelta);
    m_apkBullets[1]->Update(fDelta);

    if(m_bSpawn)
    {
        m_pkSpawnAnim->Animate();
        if(m_pkSpawnAnim->GetFrameIterator() == 15)
        {
            m_bSpawn = false;
            m_pkSpawnAnim->SetPlaying(false);
            m_bIsAlive = true;
        }
    }
    if(m_bExplode)
    {
        m_pkExplAnim->Animate();
        OnExplode();
    }
}