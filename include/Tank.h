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

#ifndef _TANK_H_
#define _TANK_H_

#include "Defines.h"
#include "Types.h"

class Animation;
class Texture;
class VideoDriver;

class Tank
{
protected:
    class Bullet
    {
        friend class Tank;
        friend class Player;
        friend class Enemy;

    private:
        DIRECTION m_kDir;
        TANKLEVEL m_kTL;
        Animation *m_pkExplAnim;
        bool m_bExplode;
        bool m_bDestroyJungle;
        bool m_bIsAlive;
        f32 m_iSpeed;
        f32 m_iX;
        f32 m_iY;
        f32 m_iExplX;
        f32 m_iExplY;

        VideoDriver *m_pVD;

        Texture *m_pTexture;

        Bullet(VideoDriver *pVD, Texture *pTexture, TANKLEVEL kTL, DIRECTION kDir, f32 iSpeed);
        ~Bullet();

        void Update(f32 fDelta);
        void Render();

    public:
        void Destroy(bool bExplode = true);

        void SetSpeed(f32 iSpeed) { m_iSpeed = iSpeed; }
        
        void Reset() { m_bIsAlive = false; }

        f32 GetX() const { return m_iX; }
        f32 GetY() const { return m_iY; }
        f32 GetSpeed() const { return m_iSpeed; }
        DIRECTION GetDirection() const { return m_kDir; }
        TANKLEVEL GetTankLevel() const { return m_kTL; }
        bool GetDestroyJungle() const { return m_bDestroyJungle; }
        bool IsAlive() const { return m_bIsAlive; }
    };

    Animation *m_pkAnim;
    Animation *m_pkSpawnAnim;
    Animation *m_pkExplAnim;
    Bullet *m_apkBullets[2];
    TANKLEVEL m_kTL;
    DIRECTION m_kDir;
    Texture *m_pTexture;
    f32 m_iSpeed;
    f32 m_iX;
    f32 m_iY;
    u8 m_iSpawnX;
    u8 m_iSpawnY;
    bool m_bSpawn;
    bool m_bExplode;
    bool m_bIsMoving;
    bool m_bIsAlive;
    bool m_bCanMove;

    static f32 *m_pfTimer;

    VideoDriver *m_pVD;

public:
    Tank(VideoDriver *pVD, Texture *pTexture, TANKLEVEL kTL = TL_1, DIRECTION kDir = DIR_UP, f32 iSpeed = PLAYER_SPEED);
    virtual ~Tank();

    void Spawn(DIRECTION kDir = DIR_UP, TANKLEVEL kTL = TL_1);
    void Destroy();

    virtual void Shoot() = 0;

    void Update(f32 fDelta);

    virtual void Render() = 0;

    virtual void OnExplode() = 0;
    virtual void OnCollideBlock(DIRECTION kDir, BLOCK_TYPE *b1, BLOCK_TYPE *b2) = 0;
    virtual void OnCollideTank() = 0;

    void ResetAnimations()
    {
        m_pkAnim->Reset();
        m_pkSpawnAnim->Reset();
        m_pkExplAnim->Reset();
    }
    
    void ResetBullets()
    {
        if(m_apkBullets[0] != NULL) m_apkBullets[0]->Reset();
        if(m_apkBullets[1] != NULL) m_apkBullets[1]->Reset();
    }

    void SetX(f32 iX) { m_iX = iX; }
    void SetY(f32 iY) { m_iY = iY; }
    void SetPosition(f32 iX, f32 iY) { m_iX = iX; m_iY = iY; }
    void SetTankLevel(TANKLEVEL tl)
    {
        if(tl != TL_4)
        {
            m_apkBullets[0]->m_bDestroyJungle = false;
            m_apkBullets[1]->m_bDestroyJungle = false;
        }
        else
        {
            if(m_kTL == TL_4 && m_apkBullets[0]->m_bDestroyJungle && m_apkBullets[1]->m_bDestroyJungle)
            {
                m_apkBullets[0]->m_bDestroyJungle = false;
                m_apkBullets[1]->m_bDestroyJungle = false;
            }
            else if(m_kTL == TL_4 && !m_apkBullets[0]->m_bDestroyJungle && !m_apkBullets[1]->m_bDestroyJungle)
            {
                m_apkBullets[0]->m_bDestroyJungle = true;
                m_apkBullets[1]->m_bDestroyJungle = true;
            }
        }
        m_kTL = tl;
    }
    void SetDirection(DIRECTION Dir)
    {
        // FIXME Tanks blocking each other when changing direction
        u32 mult;
        switch(Dir)
        {
        case DIR_UP:
        case DIR_DOWN:
            // round
            mult = static_cast<u32>((m_iX + PLAYER_WIDTH_DIV_4) / PLAYER_WIDTH_DIV_2);
            m_iX = static_cast<f32>(mult * PLAYER_WIDTH_DIV_2);
            break;
        case DIR_RIGHT:
        case DIR_LEFT:
            // round
            mult = static_cast<u32>((m_iY + PLAYER_HEIGHT_DIV_4 - HUD_TOP_HEIGHT) / PLAYER_HEIGHT_DIV_2);
            m_iY = static_cast<f32>(mult * PLAYER_HEIGHT_DIV_2 + HUD_TOP_HEIGHT);
            break;
        }
        m_kDir = Dir;
    }
    void SetSpeed(f32 iSpeed) { m_iSpeed = iSpeed; }
    void SetSpawnPoint(u8 iX, u8 iY) { m_iSpawnX = iX; m_iSpawnY = iY; }
    void SetIsMoving(bool bIsMoving) { m_bIsMoving = bIsMoving; m_pkAnim->SetPlaying(bIsMoving); }
    void SetIsAlive(bool bIsAlive) { m_bIsAlive = bIsAlive; }

    f32 GetX() const { return m_iX; }
    f32 GetY() const { return m_iY; }
    bool IsAlive() const { return m_bIsAlive; }
    TANKLEVEL GetTankLevel() const { return m_kTL; }
    DIRECTION GetDirection() const { return m_kDir; }
    f32 GetSpeed() const { return m_iSpeed; }
    Texture *GetTexture() const { return m_pTexture; }
    u8 GetSpawnX() const { return m_iSpawnX; }
    u8 GetSpawnY() const { return m_iSpawnY; }
    bool IsMoving() const { return m_bIsMoving; }
    bool IsSpawning() const { return m_bSpawn; }
    bool IsExploding() const { return m_bExplode; }
    Animation *GetAnimation() const { return m_pkAnim; }
    bool GetExplode() const { return m_bExplode; }
    Bullet *GetBullet(int i) const { i = (i > 0) ? 1 : 0; return m_apkBullets[i]; }
    bool IsOnSpawn() const
    {
        return (
            (m_iX < m_iSpawnX * PLAYER_WIDTH + PLAYER_WIDTH) &&
            (m_iX + PLAYER_WIDTH > m_iSpawnX * PLAYER_WIDTH) &&
            (m_iY < m_iSpawnY * PLAYER_HEIGHT + PLAYER_HEIGHT) &&
            (m_iY + PLAYER_HEIGHT > m_iSpawnY * PLAYER_HEIGHT)
        );
    }

    static void SetTimer(f32 *pfTimer) { m_pfTimer = pfTimer; }
};

#endif // _TANK_H_
