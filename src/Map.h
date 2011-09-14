#ifndef _MAP_H_
#define _MAP_H_

#include "Animation.h"
#include "RenderList.h"
#include "Keyboard.h"
#include "Player.h"
#include "Enemy.h"
#include "Singleton.h"
#include "Texture.h"
#include "Types.h"
#include "VideoDriver.h"
#include "Bonus.h"
#include "SoundManager.h"
#include <SDL/SDL.h>
#include <ctime>

const u8 g_ciMapWidth = 25;
const u8 g_ciMapHeight = 17;
const f32 g_cfSpawnInterval = 1;

struct BonusArea
{
	u16 iX;
	u16 iY;
	u16 iW;
	u16 iH;
};

struct SpawnPoint
{
	u8 iX;
	u8 iY;
};

struct MapInfo
{
	char id[4];
	u8 iNameSize;
	char *strName;
	BonusArea bonusArea;
	SpawnPoint p1Spawn;
	SpawnPoint p2Spawn;
	SpawnPoint e1Spawn;
	SpawnPoint e2Spawn;
	SpawnPoint e3Spawn;
	SpawnPoint eagle;
};

// pierwszy bajt - x, kolejny - y
enum BLOCK_TYPE
{
	BT_EMPTY = 48,
	BT_BRICK = 0,
	BT_BRICK_HBOT = 2048,
	BT_BRICK_HTOP = 2056,
	BT_BRICK_VLEFT = 4096,
	BT_BRICK_VRIGHT = 4104,
	BT_BRICK_LBOT = 6144,
	BT_BRICK_RTOP = 6152,
	BT_BRICK_RBOT = 6160,
	BT_BRICK_LTOP = 6168,
	BT_STEEL = 8,
	BT_SEA = 16,
	BT_JUNGLE = 2064,
	BT_ICE = 2072,
	BT_EAGLE = 32,
    BT_EAGLELTOP = 32,
    BT_EAGLELBOT = 40,
    BT_EAGLERTOP = 2080,
    BT_EAGLERBOT = 2088,
	BT_DEADEAGLE = 4128,
    BT_DEADEAGLELTOP = 4128,
    BT_DEADEAGLELBOT = 4136,
    BT_DEADEAGLERTOP = 6176,
    BT_DEADEAGLERBOT = 6184,
	BT_EDGE = 4112,
	BT_DUMMY = 49 // random totalny, potrzebne aby za pierwszym razem wyczyœciæ obszar wype³niony BT_EMPTY
};


class Map :
    public Singleton<Map>
{
private:
    RenderList *m_pRenderList;
    BLOCK_TYPE m_aBlocks[g_ciMapWidth * 2][g_ciMapHeight * 2];
    BLOCK_TYPE m_aOldBlocks[g_ciMapWidth * 2][g_ciMapHeight * 2];
    Player *m_pkPlayer1;
    Player *m_pkPlayer2;
    Enemy *m_apkEnemy[4];
    f32 m_afKillTime[4];
    f32 m_fLastKill;

    f32 m_fDigTime;
    bool m_bDigged;

	bool m_bEagleDestroyed;

	MapInfo *m_pMapInfo;
	Texture *m_pBlockTexture;
    Texture *m_pPlayer1Texture;
    Texture *m_pPlayer2Texture;
    Texture *m_pEnemyTexture;
    Texture *m_pMiscTexture;
    Texture *m_pBonusTexture;

    static Animation *m_pAnimation;
    static f32 *m_pfTimer;

    VideoDriver *m_pVD;

    bool m_b2PlayerMode;

public:
    Map(VideoDriver *pVD,
        const char *szBlockTextureName,
        const char *szPlayer1TextureName,
        const char *szPlayer2TextureName,
        const char *szEnemyTextureName,
        const char *szMiscTextureName);
    ~Map(void);

	bool LoadMap(unsigned char *data, unsigned int size);

    void CalculateDamageUp(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy = false);
    void CalculateDamageRight(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy = false);
    void CalculateDamageDown(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy = false);
    void CalculateDamageLeft(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy = false);
    void HandleCollisions(f32 fDelta);
	void DestroyEagle();

    void UpdateBlocks();
	void Update(f32 fDelta, bool bGetInput = true);
	void Render();

    void Reset();

    void Set2PlayerMode(bool b2PlayerMode) { m_b2PlayerMode = b2PlayerMode; }

	MapInfo *GetMapInfo() { return m_pMapInfo; }
    Player *GetPlayer1() { return m_pkPlayer1; }
    Player *GetPlayer2() { return m_pkPlayer2; }
	Enemy *GetEnemy(int iEnemy) { return m_apkEnemy[iEnemy]; }
    bool Is2PlayerMode() { return m_b2PlayerMode; }
	bool IsEagleDestroyed() { return m_bEagleDestroyed; }

    static Animation *SetupAnimation();
    inline static void SetTimer(f32 *pfTimer) { m_pfTimer = pfTimer; }

private:
	bool Read(SDL_RWops *pHandle, void *pData, u32 iSize, u32 iBlocks);
	bool ReadHeader(SDL_RWops *pHandle);
	bool ReadBlockPart(SDL_RWops *pHandle, int iX, int iY);
};

#endif