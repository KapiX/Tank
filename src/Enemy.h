#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Tank.h"

enum SHIELDLEVEL
{
    SL_1 = 1,
    SL_2,
    SL_3,
    SL_4,
};

class Enemy :
    public Tank
{
private:
    Animation *m_pkBonusAnim;
    SHIELDLEVEL m_kSL;
    bool m_bHasBonus;

    f32 m_fStopTime;
    bool m_bStopped;

    static int m_iEnemiesLeft;
    static int m_iBonusesLeft;

public:
    Enemy(VideoDriver *pVD, Texture *pEnemyTexture, Texture *pMiscTexture, bool bHasBonus, SHIELDLEVEL kSL, TANKLEVEL kTL = TL_1, DIRECTION kDir = DIR_UP, f32 iSpeed = 50.0f);
    ~Enemy();

    void Stop();

    void Spawn();

    void Shoot();

    void Update(f32 fDelta);
    void Render();

    void OnExplode();

    inline static void SetEnemiesLeft(int i) { m_iEnemiesLeft = i; }
    inline static void SetBonusesLeft(int i) { m_iBonusesLeft = i; }

	inline void SetShieldLevel(SHIELDLEVEL kSL) { m_kSL = kSL; }
    inline void SetHasBonus(bool bHasBonus) { m_bHasBonus = bHasBonus; m_pkBonusAnim->SetPlaying(bHasBonus); }

    inline void SetTankLevel(TANKLEVEL tl)
	{
        Tank::SetTankLevel(tl);
        m_iSpeed = 50.0f;
        m_apkBullets[0]->SetSpeed(m_iSpeed * 3);
		if(tl == TL_2)
        {
            m_iSpeed *= 2;
        }
        else if(tl == TL_3)
        {
            m_apkBullets[0]->SetSpeed(m_apkBullets[0]->GetSpeed() * 2);
        }
	}

    inline SHIELDLEVEL GetShieldLevel() const { return m_kSL; }
	inline bool GetHasBonus() const { return m_bHasBonus; }
	inline bool GetIsSpawning() const { return m_bSpawn; }

    inline static int GetEnemiesLeft() { return m_iEnemiesLeft; }
    inline static int GetBonusesLeft() { return m_iBonusesLeft; }
};

#endif