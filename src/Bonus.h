#ifndef _BONUS_H_
#define _BONUS_H_

#include "Types.h"
#include "Singleton.h"
#include "Animation.h"
#include "VideoDriver.h"
#include "Texture.h"
#include <cmath>
#include <ctime>

enum BONUS_TYPE
{
	BONUS_NONE = 0,
	BONUS_GRENADE,
	BONUS_HELMET,
	BONUS_TIME,
	BONUS_DIG,
	BONUS_LIFE,
	BONUS_STAR,
	BONUS_PISTOL,
	BONUS_BOAT
};

class Bonus : public Singleton<Bonus>
{
private:
    Animation *m_pkAnim;
    static Texture *m_pkBonusTexture;
    BONUS_TYPE m_bt;

    int m_iX;
    int m_iY;

    bool m_bIsAlive;

public:
    Bonus(BONUS_TYPE bt = BONUS_NONE);
    ~Bonus();

    void Randomize(u16 iX, u16 iY, u16 iW, u16 iH);
    void Update();
    void Render(VideoDriver *pVD);

    inline void SetBonusType(BONUS_TYPE bt) { m_bt = bt; }
	inline void SetIsAlive(bool bIsAlive) { m_bIsAlive = bIsAlive; }

    inline static void SetTexture(Texture *pTexture) { m_pkBonusTexture = pTexture; }

    inline BONUS_TYPE GetBonusType() const { return m_bt; }
    inline int GetX() const { return m_iX; }
    inline int GetY() const { return m_iY; }
    inline bool IsAlive() const { return m_bIsAlive; }
};

#endif