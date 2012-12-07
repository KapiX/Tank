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

#ifndef _MAP_H_
#define _MAP_H_

#include "Singleton.h"
#include "Types.h"
#include "SDL.h"
#include <ctime>

class Animation;
class Enemy;
class Player;
class RenderList;
class Tank;
class Texture;
class VideoDriver;

const u8 g_ciMapWidth = 25;
const u8 g_ciMapHeight = 17;
const f32 g_cfSpawnInterval = 1;

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
    Texture *m_pAtlasTexture;

    static Animation *m_pAnimation;
    static f32 *m_pfTimer;

    VideoDriver *m_pVD;

    bool m_b2PlayerMode;

public:
    Map(VideoDriver *pVD, Texture *pAtlasTexture);
    ~Map(void);

    bool LoadMap(unsigned char *data, unsigned int size);

    SOUND CalculateDamageUp(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2);
    SOUND CalculateDamageRight(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2);
    SOUND CalculateDamageDown(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2);
    SOUND CalculateDamageLeft(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2);
    void CanMove(Tank *pTank, f32 fDelta);
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

#endif // _MAP_H_