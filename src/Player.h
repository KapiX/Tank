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

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Tank.h"
#include "SoundManager.h"

class Player :
    public Tank
{
private:
    Animation *m_pkShieldAnim;
    bool m_bShield;
    f32 m_iShieldTime;
    bool m_bBoat;
    u32 m_iLives;
    u32 m_iScore;

    u8 m_iNum;

    f32 m_fOldTime; // shield

    f32 m_fSlide;
    bool m_bSliding;
    DIRECTION m_kSlidingDir;

public:
    Player(VideoDriver *pVD, u8 iNum, Texture *pTexture, TANKLEVEL kTL = TL_1, DIRECTION kDir = DIR_UP, f32 iSpeed = 85.0f);
    ~Player();

    void Spawn();

    void Shoot();

    void Update(f32 fDelta);
    void Render();

    void OnExplode();

    void ActivateShield(f32 iTime);
    void Slide(f32 fSlide);

    void ResetAnimations() { Tank::ResetAnimations(); m_pkShieldAnim->Reset(); }

    void SetBoat(bool bBoat) { m_bBoat = bBoat; }
    void SetLives(u32 iLives) { m_iLives = iLives; }
    void AddLives(s32 iLives) { m_iLives += iLives; }
    void SetScore(u32 iScore) { m_iScore = iScore; }
    void AddScore(s32 iScore) { m_iScore += iScore; }

    bool GetShield() const { return m_bShield; }
    bool GetBoat() const { return m_bBoat; }
    u32 GetLives() const { return m_iLives; }
    u32 GetScore() const { return m_iScore; }

    void SetIsMoving(bool bIsMoving) { Tank::SetIsMoving(bIsMoving); SoundManager::GetInstance()->Play(bIsMoving ? SND_MOVING : SND_NMOVING); }
};

#endif // _PLAYER_H_