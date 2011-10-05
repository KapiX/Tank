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
    u32 m_iLifes;
    u32 m_iScore;

    f32 m_fOldTime; // shield

public:
    Player(VideoDriver *pVD, Texture *pTankTexture, Texture *pMiscTexture, TANKLEVEL kTL = TL_1, DIRECTION kDir = DIR_UP, f32 iSpeed = 85.0f);
    ~Player();

    void Spawn();

    void Shoot();

    void Update(f32 fDelta);
    void Render();

    void OnExplode();

    void ActivateShield(f32 iTime);

	inline void ResetAnimations() { Tank::ResetAnimations(); m_pkShieldAnim->Reset(); }

    inline void SetLifes(u32 iLifes) { m_iLifes = iLifes; }
	inline void AddLifes(s32 iLifes) { m_iLifes += iLifes; }
	inline void SetScore(u32 iScore) { m_iScore = iScore; }
	inline void AddScore(s32 iScore) { m_iScore += iScore; }
	//inline void SetColor(PLAYER_COLOR pc) { m_kColor = pc; }
	
	inline bool GetShield() const { return m_bShield; }
	inline u32 GetLifes() const { return m_iLifes; }
	inline u32 GetScore() const { return m_iScore; }
	//inline PLAYER_COLOR GetColor() const { return m_kColor; }

	inline void SetIsMoving(bool bIsMoving) { Tank::SetIsMoving(bIsMoving); SoundManager::GetInstance()->Play(bIsMoving ? SND_MOVING : SND_NMOVING); }
};

#endif