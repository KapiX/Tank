#ifndef _TANK_H_
#define _TANK_H_

#include "Animation.h"
#include "Texture.h"
#include "Types.h"
#include "VideoDriver.h"

enum DIRECTION
{
    DIR_UP = 1,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
};

enum TANKLEVEL
{
    TL_1 = 1,
    TL_2,
    TL_3,
    TL_4,
};

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

        Texture *m_pBulletTexture;
	    Texture *m_pMiscTexture;

        Bullet(VideoDriver *pVD, Texture *pBulletTexture, Texture *pMiscTexture, TANKLEVEL kTL, DIRECTION kDir, f32 iSpeed);
        ~Bullet();

	    void Update(f32 fDelta);
	    void Render();

    public:
        void Destroy(bool bExplode = true);

	    inline void SetSpeed(f32 iSpeed) { m_iSpeed = iSpeed; }

        inline f32 GetX() const { return m_iX; }
	    inline f32 GetY() const { return m_iY; }
        inline f32 GetSpeed() const { return m_iSpeed; }
	    inline DIRECTION GetDirection() const { return m_kDir; }
        inline TANKLEVEL GetTankLevel() const { return m_kTL; }
        inline bool GetDestroyJungle() const { return m_bDestroyJungle; }
        inline bool IsAlive() const { return m_bIsAlive; }
    };

    Animation *m_pkAnim;
    Animation *m_pkSpawnAnim;
    Animation *m_pkExplAnim;
    Bullet *m_apkBullets[2];
    TANKLEVEL m_kTL;
    DIRECTION m_kDir;
    Texture *m_pTankTexture;
    Texture *m_pMiscTexture;
    f32 m_iSpeed;
    f32 m_iX;
    f32 m_iY;
    u8 m_iSpawnX;
    u8 m_iSpawnY;
    bool m_bSpawn;
    bool m_bExplode;
    bool m_bIsMoving;
    bool m_bIsAlive;

    static f32 *m_pfTimer;

    VideoDriver *m_pVD;

public:
    Tank(VideoDriver *pVD, Texture *pTankTexture, Texture *pMiscTexture, TANKLEVEL kTL = TL_1, DIRECTION kDir = DIR_UP, f32 iSpeed = 85.0f);
    virtual ~Tank();

    void Spawn(DIRECTION kDir = DIR_UP, TANKLEVEL kTL = TL_1);
    void Destroy();

    virtual void Shoot() = 0;

    void Update(f32 fDelta);

    virtual void Render() = 0;

    virtual void OnExplode() = 0;

	inline void ResetAnimations()
	{
		m_pkAnim->Reset();
		m_pkSpawnAnim->Reset();
		m_pkExplAnim->Reset();
		if(m_apkBullets[0] != NULL) m_apkBullets[0]->m_pkExplAnim->Reset();
		if(m_apkBullets[1] != NULL) m_apkBullets[1]->m_pkExplAnim->Reset();
	}

    inline void SetX(f32 iX) { m_iX = iX; }
	inline void SetY(f32 iY) { m_iY = iY; }
    inline void SetPosition(f32 iX, f32 iY) { m_iX = iX; m_iY = iY; }
    inline void SetTankLevel(TANKLEVEL tl)
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
	inline void SetDirection(DIRECTION Dir)
    {
        int modulo;
        switch(Dir)
        {
        case DIR_UP:
            modulo = (int) m_iX % 16;
            if(modulo <= 8)
            {
                m_iX = (int) m_iX - modulo;
            }
            else
            {
                m_iX = (int) m_iX + modulo % 8;
            }
            break;
        case DIR_RIGHT:
            modulo = (int) (m_iY - 24) % 16;
            if(modulo <= 8)
            {
                m_iY = (int) m_iY - modulo;
            }
            else
            {
                m_iY = (int) m_iY + modulo % 8;
            }
            break;
        case DIR_DOWN:
            modulo = (int) m_iX % 16;
            if(modulo <= 8)
            {
                m_iX = (int) m_iX - modulo;
            }
            else
            {
                m_iX = (int) m_iX + modulo % 8;
            }
            break;
        case DIR_LEFT:
            modulo = (int) (m_iY - 24) % 16;
            if(modulo <= 8)
            {
                m_iY = (int) m_iY - modulo;
            }
            else
            {
                m_iY = (int) m_iY + modulo % 8;
            }
            break;
        }
        m_kDir = Dir;
    }
	inline void SetSpeed(f32 iSpeed) { m_iSpeed = iSpeed; }
	inline void SetSpawnPoint(u8 iX, u8 iY) { m_iSpawnX = iX; m_iSpawnY = iY; }
    inline void SetIsMoving(bool bIsMoving) { m_bIsMoving = bIsMoving; m_pkAnim->SetPlaying(bIsMoving); }
	inline void SetIsAlive(bool bIsAlive) { m_bIsAlive = bIsAlive; }

    inline f32 GetX() const { return m_iX; }
	inline f32 GetY() const { return m_iY; }
    inline bool IsAlive() const { return m_bIsAlive; }
    inline TANKLEVEL GetTankLevel() const { return m_kTL; }
	inline DIRECTION GetDirection() const { return m_kDir; }
	inline f32 GetSpeed() const { return m_iSpeed; }
	inline Texture *GetTankTexture() const { return m_pTankTexture; }
	inline Texture *GetMiscTexture() const { return m_pMiscTexture; }
	inline u8 GetSpawnX() const { return m_iSpawnX; }
	inline u8 GetSpawnY() const { return m_iSpawnY; }
	inline bool IsMoving() const { return m_bIsMoving; }
	inline bool IsSpawning() const { return m_bSpawn; }
	inline bool IsExploding() const { return m_bExplode; }
	inline Animation *GetAnimation() const { return m_pkAnim; }
	inline bool GetExplode() const { return m_bExplode; }
    inline Bullet *GetBullet(int i) const { i = (i > 0) ? 1 : 0; return m_apkBullets[i]; }

    inline static void SetTimer(f32 *pfTimer) { m_pfTimer = pfTimer; }
};

#endif