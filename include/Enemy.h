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

#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Defines.h"
#include "Tank.h"
#include "Types.h"

class Enemy :
    public Tank
{
private:
    Animation *m_pkBonusAnim;
    SHIELDLEVEL m_kSL;
    bool m_bHasBonus;

    f32 m_fStopTime;
    bool m_bStopped;

    static u8 m_iEnemiesLeft;
    static u8 m_iBonusesLeft;

public:
    Enemy(VideoDriver *pVD, Texture *pTexture, bool bHasBonus, SHIELDLEVEL kSL, TANKLEVEL kTL = TL_1, DIRECTION kDir = DIR_UP, f32 iSpeed = ENEMY_SPEED);
    ~Enemy();

    void Stop();

    void Spawn();

    void Shoot();

    void Update(f32 fDelta);
    void Render();

    void OnExplode();
    void OnCollideBlock(DIRECTION kDir, BLOCK_TYPE *b1, BLOCK_TYPE *b2);
    void OnCollideTank();

    static void SetEnemiesLeft(u8 i) { m_iEnemiesLeft = i; }
    static void SetBonusesLeft(u8 i) { m_iBonusesLeft = i; }

    void SetShieldLevel(SHIELDLEVEL kSL) { m_kSL = kSL; }
    void SetHasBonus(bool bHasBonus) { m_bHasBonus = bHasBonus; m_pkBonusAnim->SetPlaying(bHasBonus); }
    void SetTankLevel(TANKLEVEL tl);

    SHIELDLEVEL GetShieldLevel() const { return m_kSL; }
    bool GetHasBonus() const { return m_bHasBonus; }
    bool GetIsSpawning() const { return m_bSpawn; }
    bool GetStopped() const { return m_bStopped; }

    static u8 GetEnemiesLeft() { return m_iEnemiesLeft; }
    static u8 GetBonusesLeft() { return m_iBonusesLeft; }
};

#endif // _ENEMY_H_