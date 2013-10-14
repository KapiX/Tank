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

#include "Animation.h"
#include "Bonus.h"
#include "Config.h"
#include "Defines.h"
#include "Enemy.h"
#include "Keyboard.h"
#include "Map.h"
#include "Player.h"
#include "RenderList.h"
#include "SoundManager.h"
#include "Texture.h"
#include "VideoDriver.h"
#include <cmath>

Animation *Map::m_pAnimation = NULL;
f32 *Map::m_pfTimer = NULL;

Map::Map(VideoDriver *pVD, Texture *pAtlasTexture)
{
    m_pVD = pVD;

    if(Map::m_pAnimation == NULL)
        m_pAnimation = Map::SetupAnimation();

    m_b2PlayerMode = false;
    m_pAtlasTexture = pAtlasTexture;
    m_pRenderList = pVD->CreateRenderList(RLT_LINKED, 1750);
    m_pRenderList->SetTexture(m_pAtlasTexture);

    Bonus::SetTexture(m_pAtlasTexture);

    m_pkPlayer1 = new Player(pVD, 1, m_pAtlasTexture);
    m_pkPlayer1->SetLives(3);
    m_pkPlayer1->SetScore(0);

    m_pkPlayer2 = new Player(pVD, 2, m_pAtlasTexture);
    m_pkPlayer2->SetLives(3);
    m_pkPlayer2->SetScore(0);

    m_apkEnemy[0] = NULL;
    m_apkEnemy[1] = NULL;
    m_apkEnemy[2] = NULL;
    m_apkEnemy[3] = NULL;

    m_afKillTime[0] = -g_cfSpawnInterval;
    m_afKillTime[1] = -g_cfSpawnInterval;
    m_afKillTime[2] = -g_cfSpawnInterval;
    m_afKillTime[3] = g_cfSpawnInterval * 1.5f;
    m_fLastKill = 0;

    Sprite *spr = new Sprite;
    for(int i = 0; i < 50; i++)
    {
        for(int j = 0; j < 34; j++)
        {
            spr->iX = (f32) i * 16;
            spr->iY = (f32) j * 16 + 24;
            spr->iW = 16;
            spr->iH = 16;
            spr->iZ = 1.0f;
            spr->iS1 = 1;
            spr->iT1 = 1;
            spr->iS2 = 1;
            spr->iT2 = 1;
            m_pRenderList->AddElement(spr);
        }
        // degenerating quad
        spr->iX = (f32) i * 16;
        spr->iY = (f32) 34 * 16 + 24;
        spr->iW = 0;
        spr->iH = -(34 * 16);
        spr->iZ = 1.0f;
        spr->iS1 = 0;
        spr->iT1 = 0;
        spr->iS2 = 1;
        spr->iT2 = 1;
        m_pRenderList->AddElement(spr);
    }
    m_pRenderList->FillBuffer();
    delete spr;

    m_pMapInfo = new MapInfo;
}

Map::~Map(void)
{
    if(m_pkPlayer1 != NULL)
    {
        delete m_pkPlayer1;
        m_pkPlayer1 = NULL;
    }
    if(m_pkPlayer2 != NULL)
    {
        delete m_pkPlayer2;
        m_pkPlayer2 = NULL;
    }
    for(int i = 0; i < 4; i++)
    {
        if(m_apkEnemy[i])
        {
            delete m_apkEnemy[i];
            m_apkEnemy[i] = NULL;
        }
    }
    if(m_pRenderList != NULL)
    {
        delete m_pRenderList;
        m_pRenderList = NULL;
    }
}

bool Map::LoadMap(unsigned char *data, unsigned int size)
{
    SDL_RWops *file = SDL_RWFromMem(data, size);
    if(!ReadHeader(file))
    {
        SDL_RWclose(file);
        return false;
    }
    else
    {
        for(int x = 0; x < g_ciMapWidth * 2; x++)
        {
            for(int y = 0; y < g_ciMapHeight * 2; y++)
            {
                if(!ReadBlockPart(file, x, y))
                {
                    SDL_RWclose(file);
                    return false;
                }
            }
        }
    }
    SDL_RWclose(file);

    u32 x1 = m_pMapInfo->eagle.iX * 2;
    u32 y1 = m_pMapInfo->eagle.iY * 2;
    u32 x2 = m_pMapInfo->eagle.iX * 2 + 1; 
    u32 y2 = m_pMapInfo->eagle.iY * 2 + 1;
    
    m_aOldBlocks[x1][y1] = m_aBlocks[x1][y1] = BT_EAGLELTOP;
    m_aOldBlocks[x2][y1] = m_aBlocks[x2][y1] = BT_EAGLERTOP;
    m_aOldBlocks[x1][y2] = m_aBlocks[x1][y2] = BT_EAGLELBOT;
    m_aOldBlocks[x2][y2] = m_aBlocks[x2][y2] = BT_EAGLERBOT;

    m_pkPlayer1->SetSpawnPoint(m_pMapInfo->p1Spawn.iX, m_pMapInfo->p1Spawn.iY);
    m_pkPlayer2->SetSpawnPoint(m_pMapInfo->p2Spawn.iX, m_pMapInfo->p2Spawn.iY);

    m_pkPlayer1->SetIsAlive(false);
    m_pkPlayer2->SetIsAlive(false);
    m_pkPlayer1->SetIsMoving(false);
    m_pkPlayer2->SetIsMoving(false);

    m_pkPlayer1->ResetAnimations();
    m_pkPlayer2->ResetAnimations();
    
    m_pkPlayer1->ResetBullets();
    m_pkPlayer2->ResetBullets();

    Bonus::GetInstance()->SetBonusType(BONUS_NONE);
    Bonus::GetInstance()->GetAnimation()->Reset();

    m_pAnimation->Reset(); // sea

    delete m_apkEnemy[0];
    m_apkEnemy[0] = NULL;
    delete m_apkEnemy[1];
    m_apkEnemy[1] = NULL;
    delete m_apkEnemy[2];
    m_apkEnemy[2] = NULL;
    delete m_apkEnemy[3];
    m_apkEnemy[3] = NULL;

    // -g_cfSpawnInterval causes spawning right after level start
    m_afKillTime[0] = -g_cfSpawnInterval;
    m_afKillTime[1] = -g_cfSpawnInterval;
    m_afKillTime[2] = -g_cfSpawnInterval;
    m_afKillTime[3] = g_cfSpawnInterval * 1.5f;

    m_bEagleDestroyed = false;

    Enemy::SetEnemiesLeft(20);
    Enemy::SetBonusesLeft(5);

    return true;
}

SOUND Map::CalculateDamageUp(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;
    SOUND ret = SND_BRICKHIT;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        tmp1 = BT_BRICK_HTOP;
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        if(*pkBlock2 == BT_BRICK_LTOP
        || *pkBlock2 == BT_BRICK_RTOP
        || *pkBlock2 == BT_BRICK_HTOP
        || *pkBlock2 == BT_EMPTY
        || *pkBlock2 == BT_JUNGLE
        || *pkBlock2 == BT_ICE
        || *pkBlock2 == BT_SEA)
            tmp1 = BT_EMPTY;
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        tmp1 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
        tmp1 = BT_BRICK_RTOP;
        break;
    case BT_BRICK_VLEFT:
        tmp1 = BT_BRICK_LTOP;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }

    switch(*pkBlock2)
    {
    case BT_BRICK:
        tmp2 = BT_BRICK_HTOP;
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        if(*pkBlock1 == BT_BRICK_LTOP
        || *pkBlock1 == BT_BRICK_RTOP
        || *pkBlock1 == BT_BRICK_HTOP
        || *pkBlock1 == BT_EMPTY
        || *pkBlock1 == BT_JUNGLE
        || *pkBlock1 == BT_ICE
        || *pkBlock1 == BT_SEA)
            tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
        tmp2 = BT_BRICK_RTOP;
        break;
    case BT_BRICK_VLEFT:
        tmp2 = BT_BRICK_LTOP;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
    return ret;
}

SOUND Map::CalculateDamageRight(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;
    SOUND ret = SND_BRICKHIT;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        tmp1 = BT_BRICK_VRIGHT;
        break;
    case BT_BRICK_HTOP:
        tmp1 = BT_BRICK_RTOP;
        break;
    case BT_BRICK_HBOT:
        tmp1 = BT_BRICK_RBOT;
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        if(*pkBlock2 == BT_BRICK_VRIGHT
        || *pkBlock2 == BT_BRICK_RBOT
        || *pkBlock2 == BT_BRICK_RTOP
        || *pkBlock2 == BT_EMPTY
        || *pkBlock2 == BT_JUNGLE
        || *pkBlock2 == BT_ICE
        || *pkBlock2 == BT_SEA)
            tmp1 = BT_EMPTY;
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        tmp1 = BT_EMPTY;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }
    
    switch(*pkBlock2)
    {
    case BT_BRICK:
        tmp2 = BT_BRICK_VRIGHT;
        break;
    case BT_BRICK_HTOP:
        tmp2 = BT_BRICK_RTOP;
        break;
    case BT_BRICK_HBOT:
        tmp2 = BT_BRICK_RBOT;
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        if(*pkBlock1 == BT_BRICK_VRIGHT
        || *pkBlock1 == BT_BRICK_RBOT
        || *pkBlock1 == BT_BRICK_RTOP
        || *pkBlock1 == BT_EMPTY
        || *pkBlock1 == BT_JUNGLE
        || *pkBlock1 == BT_ICE
        || *pkBlock1 == BT_SEA)
            tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        tmp2 = BT_EMPTY;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
    return ret;
}

SOUND Map::CalculateDamageDown(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;
    SOUND ret = SND_BRICKHIT;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        tmp1 = BT_BRICK_HBOT;
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        if(*pkBlock2 == BT_BRICK_LBOT
        || *pkBlock2 == BT_BRICK_RBOT
        || *pkBlock2 == BT_BRICK_HBOT
        || *pkBlock2 == BT_EMPTY
        || *pkBlock2 == BT_JUNGLE
        || *pkBlock2 == BT_ICE
        || *pkBlock2 == BT_SEA)
            tmp1 = BT_EMPTY;
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        tmp1 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
        tmp1 = BT_BRICK_RBOT;
        break;
    case BT_BRICK_VLEFT:
        tmp1 = BT_BRICK_LBOT;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }

    switch(*pkBlock2)
    {
    case BT_BRICK:
        tmp2 = BT_BRICK_HBOT;
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        if(*pkBlock1 == BT_BRICK_LBOT
        || *pkBlock1 == BT_BRICK_RBOT
        || *pkBlock1 == BT_BRICK_HBOT
        || *pkBlock1 == BT_EMPTY
        || *pkBlock1 == BT_JUNGLE
        || *pkBlock1 == BT_ICE
        || *pkBlock1 == BT_SEA)
            tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
        tmp2 = BT_BRICK_RBOT;
        break;
    case BT_BRICK_VLEFT:
        tmp2 = BT_BRICK_LBOT;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
    return ret;
}

SOUND Map::CalculateDamageLeft(BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;
    SOUND ret = SND_BRICKHIT;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        tmp1 = BT_BRICK_VLEFT;
        break;
    case BT_BRICK_HTOP:
        tmp1 = BT_BRICK_LTOP;
        break;
    case BT_BRICK_HBOT:
        tmp1 = BT_BRICK_LBOT;
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        if(*pkBlock2 == BT_BRICK_VLEFT
        || *pkBlock2 == BT_BRICK_LBOT
        || *pkBlock2 == BT_BRICK_LTOP
        || *pkBlock2 == BT_EMPTY
        || *pkBlock2 == BT_JUNGLE
        || *pkBlock2 == BT_ICE
        || *pkBlock2 == BT_SEA)
            tmp1 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        tmp1 = BT_EMPTY;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }
    
    switch(*pkBlock2)
    {
    case BT_BRICK:
        tmp2 = BT_BRICK_VLEFT;
        break;
    case BT_BRICK_HTOP:
        tmp2 = BT_BRICK_LTOP;
        break;
    case BT_BRICK_HBOT:
        tmp2 = BT_BRICK_LBOT;
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        if(*pkBlock1 == BT_BRICK_VLEFT
        || *pkBlock1 == BT_BRICK_LBOT
        || *pkBlock1 == BT_BRICK_LTOP
        || *pkBlock1 == BT_EMPTY
        || *pkBlock1 == BT_JUNGLE
        || *pkBlock1 == BT_ICE
        || *pkBlock1 == BT_SEA)
            tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        tmp2 = BT_EMPTY;
        break;
    case BT_STEEL:
    case BT_EDGE:
        ret = SND_STEELHIT;
        break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
    return ret;
}

/*
 * Tanks collisions
 */
void Map::CanMove(Tank *pTank, f32 fDelta)
{
    if(pTank == NULL) return;
    
    f32 top1, bottom1, left1, right1;
    f32 top2, bottom2, left2, right2;
    
    if(pTank->IsAlive())
    {
        /*--------------------
         * TANKS
         *------------------*/
        if(!pTank->IsOnSpawn())
        {
            Tank *pChecked = NULL;
            for(u8 i = 0; i < 6; i++)
            {
                if(i == 1 && !m_b2PlayerMode) continue;
                    // if player 2 is not playing, don't check
                
                switch(i)
                {
                case 0: pChecked = m_pkPlayer1; break;
                case 1: pChecked = m_pkPlayer2; break;
                case 2:
                case 3:
                case 4:
                case 5:
                    pChecked = m_apkEnemy[i - 2];
                    break;
                }
                
                if(pChecked != NULL
                && pTank != pChecked
                && pTank->IsMoving()
                && pChecked->IsAlive()
                && !pChecked->IsOnSpawn())
                {
                    top1 = pTank->GetY();
                    bottom2 = pChecked->GetY() + TANK_HEIGHT;
                    left1 = pTank->GetX();
                    right2 = pChecked->GetX() + TANK_WIDTH;
                    bottom1 = pTank->GetY() + TANK_HEIGHT;
                    top2 = pChecked->GetY();
                    right1 = pTank->GetX() + TANK_WIDTH;
                    left2 = pChecked->GetX();
                    
                    switch(pTank->GetDirection())
                    {
                    case DIR_UP: top1 -= pTank->GetSpeed() * fDelta; break;
                    case DIR_DOWN: bottom1 += pTank->GetSpeed() * fDelta; break;
                    case DIR_LEFT: left1 -= pTank->GetSpeed() * fDelta; break;
                    case DIR_RIGHT: right1 += pTank->GetSpeed() * fDelta; break;
                    }
                    
                    if((top1 < bottom2)
                    && (bottom1 > top2)
                    && (right1 > left2)
                    && (left1 < right2))
                    {
                        pTank->OnCollideTank();
                    }
                }
            }
        }
        
        /*--------------------
         * BLOCKS
         *------------------*/
        u32 i = (u32) (pTank->GetX() + BLOCK_WIDTH_DIV_2) / BLOCK_WIDTH;
        u32 j = (u32) (pTank->GetY() + BLOCK_HEIGHT_DIV_2 - HUD_TOP_HEIGHT) / BLOCK_HEIGHT;
        
        switch(pTank->GetDirection())
        {
        case DIR_UP:
            top1 = pTank->GetY() - pTank->GetSpeed() * fDelta;
            bottom2 = j * BLOCK_HEIGHT + HUD_TOP_HEIGHT;
            if(top1 < bottom2)
            {
                pTank->OnCollideBlock(
                    DIR_UP, &m_aBlocks[i][j - 1], &m_aBlocks[i + 1][j - 1]
                );
            }
            break;
        case DIR_DOWN:
            bottom1 = pTank->GetY() + TANK_HEIGHT + pTank->GetSpeed() * fDelta;
            top2 = (j + 2) * BLOCK_HEIGHT + HUD_TOP_HEIGHT;
            if(bottom1 > top2)
            {
                pTank->OnCollideBlock(
                    DIR_DOWN, &m_aBlocks[i][j + 2], &m_aBlocks[i + 1][j + 2]
                );
            }
            break;
        case DIR_RIGHT:
            right1 = pTank->GetX() + TANK_WIDTH + pTank->GetSpeed() * fDelta;
            left2 = (i + 2) * BLOCK_WIDTH;
            if(right1 > left2)
            {
                pTank->OnCollideBlock(
                    DIR_RIGHT, &m_aBlocks[i + 2][j], &m_aBlocks[i + 2][j + 1]
                );
            }
            break;
        case DIR_LEFT:
            left1 = pTank->GetX() - pTank->GetSpeed() * fDelta;
            right2 = i * BLOCK_WIDTH;
            if(left1 < right2)
            {
                pTank->OnCollideBlock(
                    DIR_LEFT, &m_aBlocks[i - 1][j], &m_aBlocks[i - 1][j + 1]
                );
            }
            break;
        }
    }
}

/*
 * Bullets collisions
 */
void Map::HandleCollisions(f32 fDelta)
{
    f32 left1, left2;
    f32 right1, right2;
    f32 top1, top2;
    f32 bottom1, bottom2;

    Player *pkPlayer;

    u8 iPCount = (m_b2PlayerMode ? 2 : 1);
    for(u8 p = 0; p < iPCount; p++)
    {
        if(p == 0) pkPlayer = m_pkPlayer1;
        else if(p == 1) pkPlayer = m_pkPlayer2;
        
        // BONUSES
        if(pkPlayer->IsAlive() && Bonus::GetInstance()->IsAlive())
        {
            top1 = pkPlayer->GetY();
            bottom2 = Bonus::GetInstance()->GetY() + 32;
            left1 = pkPlayer->GetX();
            right2 = Bonus::GetInstance()->GetX() + 32;
            bottom1 = pkPlayer->GetY() + 32;
            top2 = Bonus::GetInstance()->GetY();
            right1 = pkPlayer->GetX() + 32;
            left2 = Bonus::GetInstance()->GetX();
            if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
            {
                switch(Bonus::GetInstance()->GetBonusType())
                {
                case BONUS_GRENADE:
                    if(m_apkEnemy[0]->IsAlive())
                    {
                        m_apkEnemy[0]->Destroy();
                        m_afKillTime[0] = *m_pfTimer;
                        m_fLastKill = *m_pfTimer;
                    }
                    if(m_apkEnemy[1]->IsAlive())
                    {
                        m_apkEnemy[1]->Destroy();
                        m_afKillTime[1] = *m_pfTimer;
                        m_fLastKill = *m_pfTimer;
                    }
                    if(m_apkEnemy[2]->IsAlive())
                    {
                        m_apkEnemy[2]->Destroy();
                        m_afKillTime[2] = *m_pfTimer;
                        m_fLastKill = *m_pfTimer;
                    }
                    if(m_apkEnemy[3]->IsAlive())
                    {
                        m_apkEnemy[3]->Destroy();
                        m_afKillTime[3] = *m_pfTimer + g_cfSpawnInterval * 1.5;
                        m_fLastKill = *m_pfTimer;
                    }
                    break;
                case BONUS_HELMET:
                    pkPlayer->ActivateShield(10.0f);
                    break;
                case BONUS_TIME:
                    m_apkEnemy[0]->Stop();
                    m_apkEnemy[1]->Stop();
                    m_apkEnemy[2]->Stop();
                    m_apkEnemy[3]->Stop();
                    break;
                case BONUS_DIG:
                    m_fDigTime = *m_pfTimer;
                    m_bDigged = true;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 - 1] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 1] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 1] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 2] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 2] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 2] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 2] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 1] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 1] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 - 1] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 - 1] = BT_STEEL;
                    if(m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
                        m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 - 1] = BT_STEEL;
                    break;
                case BONUS_LIFE:
                    pkPlayer->AddLives(1);
                    break;
                case BONUS_STAR:
                    pkPlayer->SetTankLevel((pkPlayer->GetTankLevel() < TL_4 ? (TANKLEVEL) (pkPlayer->GetTankLevel() + 1) : TL_4));
                    pkPlayer->GetBullet(0)->SetSpeed(pkPlayer->GetSpeed() * 3 * 2);
                    pkPlayer->GetBullet(1)->SetSpeed(pkPlayer->GetSpeed() * 3 * 2);
                    break;
                case BONUS_PISTOL:
                    pkPlayer->SetTankLevel(TL_4);
                    pkPlayer->GetBullet(0)->SetSpeed(pkPlayer->GetSpeed() * 3 * 2);
                    pkPlayer->GetBullet(1)->SetSpeed(pkPlayer->GetSpeed() * 3 * 2);
                    break;
                case BONUS_BOAT:
                    pkPlayer->SetBoat(true);
                    break;
                }
                Bonus::GetInstance()->SetIsAlive(false);
                pkPlayer->AddScore(500);
                if(Bonus::GetInstance()->GetBonusType() == BONUS_LIFE)
                {
                    SoundManager::GetInstance()->Play(SND_LIFE);
                }
                else if(Bonus::GetInstance()->GetBonusType() == BONUS_GRENADE)
                {
                    SoundManager::GetInstance()->Play(SND_EEXPLOSION);
                }
                else
                {
                    SoundManager::GetInstance()->Play(SND_BONUS);
                }
            }
        }
        for(u8 e = 0; e < 4; e++)
        {
            if(m_apkEnemy[e] != NULL)
            {
                // TANK_BULLET-ENEMY_BULLET collision
                if(m_apkEnemy[e]->GetBullet(0)->IsAlive())
                {
                    if(pkPlayer->GetBullet(0)->IsAlive())
                    {
                        top1 = pkPlayer->GetBullet(0)->GetY();
                        bottom2 = m_apkEnemy[e]->GetBullet(0)->GetY() + 8;
                        left1 = pkPlayer->GetBullet(0)->GetX();
                        right2 = m_apkEnemy[e]->GetBullet(0)->GetX() + 8;
                        bottom1 = pkPlayer->GetBullet(0)->GetY() + 8;
                        top2 = m_apkEnemy[e]->GetBullet(0)->GetY();
                        right1 = pkPlayer->GetBullet(0)->GetX() + 8;
                        left2 = m_apkEnemy[e]->GetBullet(0)->GetX();
                        if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
                        {
                            m_apkEnemy[e]->GetBullet(0)->Destroy(false);
                            pkPlayer->GetBullet(0)->Destroy(false);
                        }
                    }
                    if(pkPlayer->GetBullet(1)->IsAlive())
                    {
                        top1 = pkPlayer->GetBullet(1)->GetY();
                        bottom2 = m_apkEnemy[e]->GetBullet(0)->GetY() + 8;
                        left1 = pkPlayer->GetBullet(1)->GetX();
                        right2 = m_apkEnemy[e]->GetBullet(0)->GetX() + 8;
                        bottom1 = pkPlayer->GetBullet(1)->GetY() + 8;
                        top2 = m_apkEnemy[e]->GetBullet(0)->GetY();
                        right1 = pkPlayer->GetBullet(1)->GetX() + 8;
                        left2 = m_apkEnemy[e]->GetBullet(0)->GetX();
                        if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
                        {
                            m_apkEnemy[e]->GetBullet(0)->Destroy(false);
                            pkPlayer->GetBullet(1)->Destroy(false);
                        }
                    }
                }

                // TANK_BULLET-ENEMY collision
                if(m_apkEnemy[e]->IsAlive())
                {
                    if(pkPlayer->GetBullet(0)->IsAlive())
                    {
                        top1 = pkPlayer->GetBullet(0)->GetY();
                        bottom2 = m_apkEnemy[e]->GetY() + 32;
                        left1 = pkPlayer->GetBullet(0)->GetX();
                        right2 = m_apkEnemy[e]->GetX() + 32;
                        bottom1 = pkPlayer->GetBullet(0)->GetY() + 8;
                        top2 = m_apkEnemy[e]->GetY();
                        right1 = pkPlayer->GetBullet(0)->GetX() + 8;
                        left2 = m_apkEnemy[e]->GetX();
                        if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
                        {
                            if(m_apkEnemy[e]->GetShieldLevel() != SL_1)
                            {
                                if(m_apkEnemy[e]->GetTankLevel() == TL_2)
                                {
                                    m_apkEnemy[e]->SetShieldLevel((SHIELDLEVEL) (m_apkEnemy[e]->GetShieldLevel() - 1));
                                    if(m_apkEnemy[e]->GetHasBonus())
                                    {
                                        Bonus::GetInstance()->Randomize(m_pMapInfo->bonusArea.iX, m_pMapInfo->bonusArea.iY, m_pMapInfo->bonusArea.iW, m_pMapInfo->bonusArea.iH);
                                        SoundManager::GetInstance()->Play(SND_TBONUSHIT);
                                    }
                                }
                                else
                                {
                                    if(m_apkEnemy[e]->GetHasBonus())
                                    {
                                        m_apkEnemy[e]->SetHasBonus(false);
                                        Bonus::GetInstance()->Randomize(m_pMapInfo->bonusArea.iX, m_pMapInfo->bonusArea.iY, m_pMapInfo->bonusArea.iW, m_pMapInfo->bonusArea.iH);
                                        SoundManager::GetInstance()->Play(SND_TBONUSHIT);
                                    }
                                    else
                                    {
                                        m_apkEnemy[e]->SetShieldLevel((SHIELDLEVEL) (m_apkEnemy[e]->GetShieldLevel() - 1));
                                        SoundManager::GetInstance()->Play(SND_SHIELDHIT);
                                    }
                                }
                                pkPlayer->GetBullet(0)->Destroy();
                            }
                            else
                            {
                                if(m_apkEnemy[e]->GetHasBonus())
                                {
                                    m_apkEnemy[e]->SetHasBonus(false);
                                    Bonus::GetInstance()->Randomize(m_pMapInfo->bonusArea.iX, m_pMapInfo->bonusArea.iY, m_pMapInfo->bonusArea.iW, m_pMapInfo->bonusArea.iH);
                                    pkPlayer->GetBullet(0)->Destroy();
                                    SoundManager::GetInstance()->Play(SND_TBONUSHIT);
                                }
                                else
                                {
                                    pkPlayer->AddScore(m_apkEnemy[e]->GetTankLevel() * 100);
                                    m_apkEnemy[e]->Destroy();
                                    m_afKillTime[e] = *m_pfTimer;
                                    m_fLastKill = *m_pfTimer;
                                    pkPlayer->GetBullet(0)->Destroy(false);
                                    SoundManager::GetInstance()->Play(SND_EEXPLOSION);
                                }
                            }
                        }
                    }
                    if(pkPlayer->GetBullet(1)->IsAlive())
                    {
                        top1 = pkPlayer->GetBullet(1)->GetY();
                        bottom2 = m_apkEnemy[e]->GetY() + 32;
                        left1 = pkPlayer->GetBullet(1)->GetX();
                        right2 = m_apkEnemy[e]->GetX() + 32;
                        bottom1 = pkPlayer->GetBullet(1)->GetY() + 8;
                        top2 = m_apkEnemy[e]->GetY();
                        right1 = pkPlayer->GetBullet(1)->GetX() + 8;
                        left2 = m_apkEnemy[e]->GetX();
                        if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
                        {
                            if(m_apkEnemy[e]->GetShieldLevel() != SL_1)
                            {
                                if(m_apkEnemy[e]->GetTankLevel() == TL_2)
                                {
                                    m_apkEnemy[e]->SetShieldLevel((SHIELDLEVEL) (m_apkEnemy[e]->GetShieldLevel() - 1));
                                    if(m_apkEnemy[e]->GetHasBonus())
                                    {
                                        Bonus::GetInstance()->Randomize(m_pMapInfo->bonusArea.iX, m_pMapInfo->bonusArea.iY, m_pMapInfo->bonusArea.iW, m_pMapInfo->bonusArea.iH);
                                        SoundManager::GetInstance()->Play(SND_TBONUSHIT);
                                    }
                                }
                                else
                                {
                                    if(m_apkEnemy[e]->GetHasBonus())
                                    {
                                        m_apkEnemy[e]->SetHasBonus(false);
                                        Bonus::GetInstance()->Randomize(m_pMapInfo->bonusArea.iX, m_pMapInfo->bonusArea.iY, m_pMapInfo->bonusArea.iW, m_pMapInfo->bonusArea.iH);
                                        SoundManager::GetInstance()->Play(SND_TBONUSHIT);
                                    }
                                    else
                                    {
                                        m_apkEnemy[e]->SetShieldLevel((SHIELDLEVEL) (m_apkEnemy[e]->GetShieldLevel() - 1));
                                        SoundManager::GetInstance()->Play(SND_SHIELDHIT);
                                    }
                                }
                                pkPlayer->GetBullet(1)->Destroy();
                            }
                            else
                            {
                                if(m_apkEnemy[e]->GetHasBonus())
                                {
                                    m_apkEnemy[e]->SetHasBonus(false);
                                    Bonus::GetInstance()->Randomize(m_pMapInfo->bonusArea.iX, m_pMapInfo->bonusArea.iY, m_pMapInfo->bonusArea.iW, m_pMapInfo->bonusArea.iH);
                                    pkPlayer->GetBullet(1)->Destroy();
                                    SoundManager::GetInstance()->Play(SND_TBONUSHIT);
                                }
                                else
                                {
                                    pkPlayer->AddScore(m_apkEnemy[e]->GetTankLevel() * 100);
                                    m_apkEnemy[e]->Destroy();
                                    pkPlayer->GetBullet(1)->Destroy(false);
                                    SoundManager::GetInstance()->Play(SND_EEXPLOSION);
                                }
                            }
                        }
                    }
                }
                // ENEMY_BULLET-TANK collision
                if(pkPlayer->IsAlive())
                {
                    if(m_apkEnemy[e]->GetBullet(0)->IsAlive())
                    {
                        top1 = m_apkEnemy[e]->GetBullet(0)->GetY();
                        bottom2 = pkPlayer->GetY() + 32;
                        left1 = m_apkEnemy[e]->GetBullet(0)->GetX();
                        right2 = pkPlayer->GetX() + 32;
                        bottom1 = m_apkEnemy[e]->GetBullet(0)->GetY() + 8;
                        top2 = pkPlayer->GetY();
                        right1 = m_apkEnemy[e]->GetBullet(0)->GetX() + 8;
                        left2 = pkPlayer->GetX();
                        if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
                        {
                            if(!pkPlayer->GetShield())
                            {
                                if(!pkPlayer->GetBoat())
                                {
                                    if(pkPlayer->GetTankLevel() == TL_4)
                                    {
                                        m_apkEnemy[e]->GetBullet(0)->Destroy(false);
                                        pkPlayer->SetTankLevel(TL_3);
                                        SoundManager::GetInstance()->Play(SND_SHIELDHIT);
                                    }
                                    else
                                    {
                                        m_apkEnemy[e]->GetBullet(0)->Destroy(); // zycia odejmuja sie przy spawnie
                                        pkPlayer->Destroy();
                                        SoundManager::GetInstance()->Play(SND_FEXPLOSION);
                                    }
                                }
                                else
                                {
                                    m_apkEnemy[e]->GetBullet(0)->Destroy(false);
                                    pkPlayer->SetBoat(false);
                                }
                            }
                            else
                            {
                                m_apkEnemy[e]->GetBullet(0)->Destroy(false);
                                SoundManager::GetInstance()->Play(SND_STEELHIT);
                            }
                        }
                    }
                }
            }
        }

        int iBCount = (pkPlayer->GetTankLevel() == TL_4 || pkPlayer->GetTankLevel() == TL_3 ? 2 : 1);
            // bullet count
        for(u8 b = 0; b < iBCount; b++)
        {
            if(pkPlayer->GetBullet(b)->IsAlive())
            {
                // BULLET-BLOCK collision
                int i = (int) (pkPlayer->GetBullet(b)->GetX()) / 16;
                int j = (int) (pkPlayer->GetBullet(b)->GetY() - 24) / 16;

                switch(pkPlayer->GetBullet(b)->GetDirection())
                {
                case DIR_UP:
                    top1 = pkPlayer->GetBullet(b)->GetY() - pkPlayer->GetBullet(b)->GetSpeed() * fDelta;
                    bottom2 = j * 16 + 24;
                    if(top1 < bottom2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i + 1][j];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_VLEFT && b1 != BT_BRICK_LBOT && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_VRIGHT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_RTOP && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(b1 == BT_EAGLELBOT || b1 == BT_EAGLERBOT ||
                                b2 == BT_EAGLELBOT || b2 == BT_EAGLERBOT)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                if(pkPlayer->GetBullet(b)->GetTankLevel() == TL_4)
                                {
                                    if((b1 & b2) != BT_EDGE)
                                    {
                                        m_aBlocks[i][j] = BT_EMPTY;
                                        m_aBlocks[i + 1][j] = BT_EMPTY;
                                        
                                        if(b1 == BT_STEEL || b2 == BT_STEEL)
                                            SoundManager::GetInstance()->Play(SND_STEELHIT);
                                        else
                                            SoundManager::GetInstance()->Play(SND_BRICKHIT);
                                    }
                                    else
                                    {
                                        SoundManager::GetInstance()->Play(SND_STEELHIT);
                                    }
                                }
                                else
                                {
                                    SoundManager::GetInstance()->Play(CalculateDamageUp(&m_aBlocks[i][j], &m_aBlocks[i + 1][j]));
                                }
                            }
                            pkPlayer->GetBullet(b)->Destroy();
                        }
                        else if(pkPlayer->GetBullet(b)->GetDestroyJungle())
                        {
                            if(m_aBlocks[i][j] == BT_JUNGLE)
                            {
                                m_aBlocks[i][j] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                            if(m_aBlocks[i + 1][j] == BT_JUNGLE)
                            {
                                m_aBlocks[i + 1][j] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                        }
                    }
                    break;
                case DIR_DOWN:
                    bottom1 = pkPlayer->GetBullet(b)->GetY() + 8 + pkPlayer->GetBullet(b)->GetSpeed() * fDelta;
                    top2 = j * 16 + 24;
                    if(bottom1 > top2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i + 1][j];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_VLEFT && b1 != BT_BRICK_LBOT && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_VRIGHT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_RTOP && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(m_aBlocks[i][j] == BT_EAGLELTOP || m_aBlocks[i][j] == BT_EAGLERTOP ||
                                m_aBlocks[i + 1][j] == BT_EAGLELTOP || m_aBlocks[i + 1][j] == BT_EAGLERTOP)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                if(pkPlayer->GetBullet(b)->GetTankLevel() == TL_4)
                                {
                                    if((b1 & b2) != BT_EDGE)
                                    {
                                        m_aBlocks[i][j] = BT_EMPTY;
                                        m_aBlocks[i + 1][j] = BT_EMPTY;

                                        if(b1 == BT_STEEL || b2 == BT_STEEL)
                                            SoundManager::GetInstance()->Play(SND_STEELHIT);
                                        else
                                            SoundManager::GetInstance()->Play(SND_BRICKHIT);
                                    }
                                    else
                                    {
                                        SoundManager::GetInstance()->Play(SND_STEELHIT);
                                    }
                                }
                                else
                                {
                                    SoundManager::GetInstance()->Play(CalculateDamageDown(&m_aBlocks[i][j], &m_aBlocks[i + 1][j]));
                                }
                            }
                            pkPlayer->GetBullet(b)->Destroy();
                        }
                        else if(pkPlayer->GetBullet(b)->GetDestroyJungle())
                        {
                            if(m_aBlocks[i][j] == BT_JUNGLE)
                            {
                                m_aBlocks[i][j] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                            if(m_aBlocks[i + 1][j] == BT_JUNGLE)
                            {
                                m_aBlocks[i + 1][j] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                        }
                    }
                    break;
                case DIR_RIGHT:
                    right1 = pkPlayer->GetBullet(b)->GetX() + 8 + pkPlayer->GetBullet(b)->GetSpeed() * fDelta;
                    left2 = i * 16;
                    if(right1 > left2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i][j + 1];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_HTOP && b1 != BT_BRICK_RTOP && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_HBOT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_LBOT && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(m_aBlocks[i][j] == BT_EAGLELBOT || m_aBlocks[i][j] == BT_EAGLELTOP ||
                                m_aBlocks[i][j + 1] == BT_EAGLELBOT || m_aBlocks[i][j + 1] == BT_EAGLELTOP)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                if(pkPlayer->GetBullet(b)->GetTankLevel() == TL_4)
                                {
                                    if((b1 & b2) != BT_EDGE)
                                    {
                                        m_aBlocks[i][j] = BT_EMPTY;
                                        m_aBlocks[i][j + 1] = BT_EMPTY;

                                        if(b1 == BT_STEEL || b2 == BT_STEEL)
                                            SoundManager::GetInstance()->Play(SND_STEELHIT);
                                        else
                                            SoundManager::GetInstance()->Play(SND_BRICKHIT);
                                    }
                                    else
                                    {
                                        SoundManager::GetInstance()->Play(SND_STEELHIT);
                                    }
                                }
                                else
                                {
                                    SoundManager::GetInstance()->Play(CalculateDamageRight(&m_aBlocks[i][j], &m_aBlocks[i][j + 1]));
                                }
                            }
                            pkPlayer->GetBullet(b)->Destroy();
                        }
                        else if(pkPlayer->GetBullet(b)->GetDestroyJungle())
                        {
                            if(m_aBlocks[i][j] == BT_JUNGLE)
                            {
                                m_aBlocks[i][j] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                            if(m_aBlocks[i][j + 1] == BT_JUNGLE)
                            {
                                m_aBlocks[i][j + 1] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                        }
                    }
                    break;
                case DIR_LEFT:
                    left1 = pkPlayer->GetBullet(b)->GetX() - pkPlayer->GetBullet(b)->GetSpeed() * fDelta;
                    right2 = i * 16;
                    if(left1 < right2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i][j + 1];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_HTOP && b1 != BT_BRICK_RTOP && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_HBOT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_LBOT && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(b1 == BT_EAGLERBOT || b1 == BT_EAGLERTOP ||
                                b2 == BT_EAGLERBOT || b2 == BT_EAGLERTOP)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                if(pkPlayer->GetBullet(b)->GetTankLevel() == TL_4)
                                {
                                    if((b1 & b2) != BT_EDGE)
                                    {
                                        m_aBlocks[i][j] = BT_EMPTY;
                                        m_aBlocks[i][j + 1] = BT_EMPTY;

                                        if(b1 == BT_STEEL || b2 == BT_STEEL)
                                            SoundManager::GetInstance()->Play(SND_STEELHIT);
                                        else
                                            SoundManager::GetInstance()->Play(SND_BRICKHIT);
                                    }
                                    else
                                    {
                                        SoundManager::GetInstance()->Play(SND_STEELHIT);
                                    }
                                }
                                else
                                {
                                    SoundManager::GetInstance()->Play(CalculateDamageLeft(&m_aBlocks[i][j], &m_aBlocks[i][j + 1]));
                                }
                            }
                            pkPlayer->GetBullet(b)->Destroy();
                        }
                        else if(pkPlayer->GetBullet(b)->GetDestroyJungle())
                        {
                            if(m_aBlocks[i][j] == BT_JUNGLE)
                            {
                                m_aBlocks[i][j] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                            if(m_aBlocks[i][j + 1] == BT_JUNGLE)
                            {
                                m_aBlocks[i][j + 1] = BT_EMPTY;
                                SoundManager::GetInstance()->Play(SND_BRICKHIT);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // ENEMIES
    for(int e = 0; e < 4; e++)
    {
        if(m_apkEnemy[e] != NULL)
        {
            if(m_apkEnemy[e]->GetBullet(0)->IsAlive())
            {
                // BULLET-BLOCK collision
                int i = (int) (m_apkEnemy[e]->GetBullet(0)->GetX()) / 16;
                int j = (int) (m_apkEnemy[e]->GetBullet(0)->GetY() - 24) / 16;

                switch(m_apkEnemy[e]->GetBullet(0)->GetDirection())
                {
                case DIR_UP:
                    top1 = m_apkEnemy[e]->GetBullet(0)->GetY() - m_apkEnemy[e]->GetBullet(0)->GetSpeed() * fDelta;
                    bottom2 = j * 16 + 24;
                    if(top1 < bottom2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i + 1][j];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_VLEFT && b1 != BT_BRICK_LBOT && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_VRIGHT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_RTOP && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(b1 == BT_EAGLELBOT || b1 == BT_EAGLERBOT ||
                                b2 == BT_EAGLELBOT || b2 == BT_EAGLERBOT)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                CalculateDamageUp(&m_aBlocks[i][j], &m_aBlocks[i + 1][j]);
                            }
                            m_apkEnemy[e]->GetBullet(0)->Destroy();
                        }
                    }
                    break;
                case DIR_DOWN:
                    bottom1 = m_apkEnemy[e]->GetBullet(0)->GetY() + 8 + m_apkEnemy[e]->GetBullet(0)->GetSpeed() * fDelta;
                    top2 = j * 16 + 24;
                    if(bottom1 > top2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i + 1][j];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_VLEFT && b1 != BT_BRICK_LBOT && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_VRIGHT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_RTOP && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(b1 == BT_EAGLELTOP || b1 == BT_EAGLERTOP ||
                                b2 == BT_EAGLELTOP || b2 == BT_EAGLERTOP)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                CalculateDamageDown(&m_aBlocks[i][j], &m_aBlocks[i + 1][j]);
                            }
                            m_apkEnemy[e]->GetBullet(0)->Destroy();
                        }
                    }
                    break;
                case DIR_RIGHT:
                    right1 = m_apkEnemy[e]->GetBullet(0)->GetX() + 8 + m_apkEnemy[e]->GetBullet(0)->GetSpeed() * fDelta;
                    left2 = i * 16;
                    if(right1 > left2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i][j + 1];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_HTOP && b1 != BT_BRICK_RTOP && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_HBOT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_LBOT && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(b1 == BT_EAGLELBOT || b1 == BT_EAGLELTOP ||
                                b2 == BT_EAGLELBOT || b2 == BT_EAGLELTOP)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                CalculateDamageRight(&m_aBlocks[i][j], &m_aBlocks[i][j + 1]);
                            }
                            m_apkEnemy[e]->GetBullet(0)->Destroy();
                        }
                    }
                    break;
                case DIR_LEFT:
                    left1 = m_apkEnemy[e]->GetBullet(0)->GetX() - m_apkEnemy[e]->GetBullet(0)->GetSpeed() * fDelta;
                    right2 = i * 16;
                    if(left1 < right2)
                    {
                        BLOCK_TYPE b1 = m_aBlocks[i][j];
                        BLOCK_TYPE b2 = m_aBlocks[i][j + 1];
                        if((b1 != BT_EMPTY && b1 != BT_ICE && b1 != BT_JUNGLE && b1 != BT_SEA && b1 != BT_BRICK_HTOP && b1 != BT_BRICK_RTOP && b1 != BT_BRICK_LTOP && b1 != BT_DEADEAGLELTOP && b1 != BT_DEADEAGLELBOT && b1 != BT_DEADEAGLERTOP && b1 != BT_DEADEAGLERBOT) ||
                            (b2 != BT_EMPTY && b2 != BT_ICE && b2 != BT_JUNGLE && b2 != BT_SEA && b2 != BT_BRICK_HBOT && b2 != BT_BRICK_RBOT && b2 != BT_BRICK_LBOT && b2 != BT_DEADEAGLELTOP && b2 != BT_DEADEAGLELBOT && b2 != BT_DEADEAGLERTOP && b2 != BT_DEADEAGLERBOT))
                        {
                            if(b1 == BT_EAGLERBOT || b1 == BT_EAGLERTOP ||
                                b2 == BT_EAGLERBOT || b2 == BT_EAGLERTOP)
                            {
                                DestroyEagle();
                            }
                            else
                            {
                                CalculateDamageLeft(&m_aBlocks[i][j], &m_aBlocks[i][j + 1]);
                            }
                            m_apkEnemy[e]->GetBullet(0)->Destroy();
                        }
                    }
                    break;
                }
            }
        }
    }
}

void Map::DestroyEagle()
{
    SoundManager::GetInstance()->Stop();
    m_bEagleDestroyed = true;
    m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2] = BT_DEADEAGLELTOP;
    m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2] = BT_DEADEAGLERTOP;
    m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 1] = BT_DEADEAGLELBOT;
    m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 1] = BT_DEADEAGLERBOT;
    SoundManager::GetInstance()->Play(SND_FEXPLOSION);
}

void Map::UpdateBlocks()
{
    for(int i = 0; i < g_ciMapWidth * 2; i++) 
    {
        for(int j = 0; j < g_ciMapHeight * 2; j++)
        {
            UpdateData ud;
            ud.iNum = i * (34 + 1) + j;
            ud.iX = i * 16;
            ud.iY = j * 16 + 24;
            ud.iZ = (m_aBlocks[i][j] != BT_JUNGLE ? 1.0f : 16.0f);
            ud.iW = 16;
            ud.iH = 16;
            ud.iS1 = (u32) 896 + (m_aBlocks[i][j] >> 8) * 2;
            ud.iT1 = (u32) (m_aBlocks[i][j] + (m_aBlocks[i][j] == BT_SEA ? m_pAnimation->GetCurrentFrame() * 8 : 0)) * 2;
            ud.iS2 = ud.iS1 + 16;
            ud.iT2 = ud.iT1 + 16;
            m_aOldBlocks[i][j] = m_aBlocks[i][j];

            m_pRenderList->UpdateBuffer(1, &ud);
        }
    }
    m_pAnimation->Animate();
}

void Map::Update(f32 fDelta, bool bGetInput)
{
    for(int i = 0; i < g_ciMapWidth * 2; i++) 
    {
        for(int j = 0; j < g_ciMapHeight * 2; j++)
        {
            if(m_aBlocks[i][j] != m_aOldBlocks[i][j] || m_aBlocks[i][j] == BT_SEA)
            {
                UpdateData ud;
                ud.iNum = i * (34 + 1) + j;
                ud.iX = i * 16;
                ud.iY = j * 16 + 24;
                ud.iZ = (m_aBlocks[i][j] != BT_JUNGLE ? 1.0f : 16.0f);
                ud.iW = 16;
                ud.iH = 16;
                ud.iS1 = (u32) 896 + (m_aBlocks[i][j] >> 8) * 2;
                ud.iT1 = (u32) (m_aBlocks[i][j] + (m_aBlocks[i][j] == BT_SEA ? m_pAnimation->GetCurrentFrame() * 8 : 0)) * 2;
                ud.iS2 = ud.iS1 + 16;
                ud.iT2 = ud.iT1 + 16;
                m_aOldBlocks[i][j] = m_aBlocks[i][j];

                m_pRenderList->UpdateBuffer(1, &ud);
            }
        }
    }

    m_pAnimation->Animate();

    if(*m_pfTimer - m_fDigTime > 20.0f && m_bDigged)
    {
        m_bDigged = false;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 - 1] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 1] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 1] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 - 1][m_pMapInfo->eagle.iY * 2 + 2] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 2] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 2] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 2] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 2] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 1] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 + 1] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 + 2][m_pMapInfo->eagle.iY * 2 - 1] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 - 1] = BT_BRICK;
        if(m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 - 1] != BT_EDGE)
            m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 - 1] = BT_BRICK;
    }

    srand(std::time(NULL) % 100 + std::time(NULL) % 10 + std::time(NULL) * 2);

    // FIXME no allocations
    if(m_apkEnemy[0] == NULL)
        m_apkEnemy[0] = new Enemy(m_pVD, m_pAtlasTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));
    if(m_apkEnemy[1] == NULL)
        m_apkEnemy[1] = new Enemy(m_pVD, m_pAtlasTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));
    if(m_apkEnemy[2] == NULL)
        m_apkEnemy[2] = new Enemy(m_pVD, m_pAtlasTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));
    if(m_apkEnemy[3] == NULL)
        m_apkEnemy[3] = new Enemy(m_pVD, m_pAtlasTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));

    if(m_apkEnemy[0] != NULL && *m_pfTimer - m_afKillTime[0] >= g_cfSpawnInterval && !m_apkEnemy[0]->IsAlive() && !m_apkEnemy[0]->GetIsSpawning())
    {
        if(Enemy::GetBonusesLeft() > 0)
            m_apkEnemy[0]->SetHasBonus((bool) (rand() % 2));
        else
            m_apkEnemy[0]->SetHasBonus(false);
        m_apkEnemy[0]->SetShieldLevel((SHIELDLEVEL) (rand() % 4 + 1));
        m_apkEnemy[0]->SetTankLevel((TANKLEVEL) (rand() % 4 + 1));
        m_apkEnemy[0]->SetSpawnPoint(m_pMapInfo->e1Spawn.iX, m_pMapInfo->e1Spawn.iY);
        m_apkEnemy[0]->Spawn();
    }
    if(m_apkEnemy[1] != NULL && *m_pfTimer - m_afKillTime[1] >= g_cfSpawnInterval && !m_apkEnemy[1]->IsAlive() && !m_apkEnemy[1]->GetIsSpawning())
    {
        if(Enemy::GetBonusesLeft() > 0)
            m_apkEnemy[1]->SetHasBonus((bool) (rand() % 2));
        else
            m_apkEnemy[1]->SetHasBonus(false);
        m_apkEnemy[1]->SetShieldLevel((SHIELDLEVEL) (rand() % 4 + 1));
        m_apkEnemy[1]->SetTankLevel((TANKLEVEL) (rand() % 4 + 1));
        m_apkEnemy[1]->SetSpawnPoint(m_pMapInfo->e2Spawn.iX, m_pMapInfo->e2Spawn.iY);
        m_apkEnemy[1]->Spawn();
    }
    if(m_apkEnemy[2] != NULL && *m_pfTimer - m_afKillTime[2] >= g_cfSpawnInterval && !m_apkEnemy[2]->IsAlive() && !m_apkEnemy[2]->GetIsSpawning())
    {
        if(Enemy::GetBonusesLeft() > 0)
            m_apkEnemy[2]->SetHasBonus((bool) (rand() % 2));
        else
            m_apkEnemy[2]->SetHasBonus(false);
        m_apkEnemy[2]->SetShieldLevel((SHIELDLEVEL) (rand() % 4 + 1));
        m_apkEnemy[2]->SetTankLevel((TANKLEVEL) (rand() % 4 + 1));
        m_apkEnemy[2]->SetSpawnPoint(m_pMapInfo->e3Spawn.iX, m_pMapInfo->e3Spawn.iY);
        m_apkEnemy[2]->Spawn();
    }
    if(m_apkEnemy[3] != NULL && *m_pfTimer - m_afKillTime[3] >= g_cfSpawnInterval && !m_apkEnemy[3]->IsAlive() && !m_apkEnemy[3]->GetIsSpawning())
    {
        if(Enemy::GetBonusesLeft() > 0)
            m_apkEnemy[3]->SetHasBonus((bool) (rand() % 2));
        else
            m_apkEnemy[3]->SetHasBonus(false);
        m_apkEnemy[3]->SetShieldLevel((SHIELDLEVEL) (rand() % 4 + 1));
        m_apkEnemy[3]->SetTankLevel((TANKLEVEL) (rand() % 4 + 1));
        m_apkEnemy[3]->SetSpawnPoint(m_pMapInfo->e1Spawn.iX, m_pMapInfo->e1Spawn.iY);
        m_apkEnemy[3]->Spawn();
    }

    m_pkPlayer1->Spawn();
    if(m_b2PlayerMode)
    {
        m_pkPlayer2->Spawn();
    }

    if(bGetInput)
    {
        bool bP1NotMoving = true, bP2NotMoving = true;
        /*
        * CONTROLS
        */
        // PLAYER 1
        if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP1Controls()->iUp))
        {
            m_pkPlayer1->SetDirection(DIR_UP);
            m_pkPlayer1->SetIsMoving(true);
            if(m_pkPlayer1->IsAlive()) bP1NotMoving = false;
        }
        else if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP1Controls()->iRight))
        {
            m_pkPlayer1->SetDirection(DIR_RIGHT);
            m_pkPlayer1->SetIsMoving(true);
            if(m_pkPlayer1->IsAlive()) bP1NotMoving = false;
        }
        else if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP1Controls()->iDown))
        {
            m_pkPlayer1->SetDirection(DIR_DOWN);
            m_pkPlayer1->SetIsMoving(true);
            if(m_pkPlayer1->IsAlive()) bP1NotMoving = false;
        }
        else if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP1Controls()->iLeft))
        {
            m_pkPlayer1->SetDirection(DIR_LEFT);
            m_pkPlayer1->SetIsMoving(true);
            if(m_pkPlayer1->IsAlive()) bP1NotMoving = false;
        }
        if(Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP1Controls()->iUp) &&
            Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP1Controls()->iRight) &&
            Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP1Controls()->iDown) &&
            Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP1Controls()->iLeft))
        {
            m_pkPlayer1->SetIsMoving(false);
            if(m_pkPlayer1->IsAlive()) bP1NotMoving = true;
        }
        if(Keyboard::GetInstance()->KeyPressed((SDLKey) Config::GetInstance()->GetP1Controls()->iShoot))
        {
            m_pkPlayer1->Shoot();
        }
        // PLAYER 2
        if(m_b2PlayerMode)
        {
            if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP2Controls()->iUp))
            {
                m_pkPlayer2->SetDirection(DIR_UP);
                m_pkPlayer2->SetIsMoving(true);
                if(m_pkPlayer2->IsAlive()) bP2NotMoving = false;
            }
            else if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP2Controls()->iRight))
            {
                m_pkPlayer2->SetDirection(DIR_RIGHT);
                m_pkPlayer2->SetIsMoving(true);
                if(m_pkPlayer2->IsAlive()) bP2NotMoving = false;
            }
            else if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP2Controls()->iDown))
            {
                m_pkPlayer2->SetDirection(DIR_DOWN);
                m_pkPlayer2->SetIsMoving(true);
                if(m_pkPlayer2->IsAlive()) bP2NotMoving = false;
            }
            else if(Keyboard::GetInstance()->IsKeyDown((SDLKey) Config::GetInstance()->GetP2Controls()->iLeft))
            {
                m_pkPlayer2->SetDirection(DIR_LEFT);
                m_pkPlayer2->SetIsMoving(true);
                if(m_pkPlayer2->IsAlive()) bP2NotMoving = false;
            }
            if(Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP2Controls()->iUp) &&
                Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP2Controls()->iRight) &&
                Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP2Controls()->iDown) &&
                Keyboard::GetInstance()->IsKeyUp((SDLKey) Config::GetInstance()->GetP2Controls()->iLeft))
            {
                m_pkPlayer2->SetIsMoving(false);
                if(m_pkPlayer2->IsAlive()) bP2NotMoving = true;
            }
            if(Keyboard::GetInstance()->KeyPressed((SDLKey) Config::GetInstance()->GetP2Controls()->iShoot))
            {
                m_pkPlayer2->Shoot();
            }
        }

        if(bP1NotMoving && bP2NotMoving)
        {
            SoundManager::GetInstance()->Play(SND_NMOVING);
        }
        else
        {
            SoundManager::GetInstance()->Play(SND_MOVING);
        }
    }
    else
    {
        m_pkPlayer1->SetIsMoving(false);
        m_pkPlayer2->SetIsMoving(false);
    }

    Bonus::GetInstance()->Update();
    
    CanMove(m_pkPlayer1, fDelta);
    if(m_b2PlayerMode) CanMove(m_pkPlayer2, fDelta);
    if(m_apkEnemy[0] != NULL) CanMove(m_apkEnemy[0], fDelta);
    if(m_apkEnemy[1] != NULL) CanMove(m_apkEnemy[1], fDelta);
    if(m_apkEnemy[2] != NULL) CanMove(m_apkEnemy[2], fDelta);
    if(m_apkEnemy[3] != NULL) CanMove(m_apkEnemy[3], fDelta);
    
    m_pkPlayer1->Update(fDelta);
    if(m_b2PlayerMode) m_pkPlayer2->Update(fDelta);

    if(m_apkEnemy[0] != NULL) m_apkEnemy[0]->Update(fDelta);
    if(m_apkEnemy[1] != NULL) m_apkEnemy[1]->Update(fDelta);
    if(m_apkEnemy[2] != NULL) m_apkEnemy[2]->Update(fDelta);
    if(m_apkEnemy[3] != NULL) m_apkEnemy[3]->Update(fDelta);

    HandleCollisions(fDelta);
}

void Map::Render()
{
    m_pRenderList->Render();
    Bonus::GetInstance()->Render(m_pVD);
    m_pkPlayer1->Render();
    if(m_b2PlayerMode)
    {
        m_pkPlayer2->Render();
    }
    if(m_apkEnemy[0] != NULL) m_apkEnemy[0]->Render();
    if(m_apkEnemy[1] != NULL) m_apkEnemy[1]->Render();
    if(m_apkEnemy[2] != NULL) m_apkEnemy[2]->Render();
    if(m_apkEnemy[3] != NULL) m_apkEnemy[3]->Render();
}

void Map::Reset()
{
    delete m_pkPlayer1;
    m_pkPlayer1 = new Player(m_pVD, 1, m_pAtlasTexture);
    m_pkPlayer1->SetLives(3);
    m_pkPlayer1->SetScore(0);

    delete m_pkPlayer2;
    m_pkPlayer2 = new Player(m_pVD, 2, m_pAtlasTexture);
    m_pkPlayer2->SetLives(3);
    m_pkPlayer2->SetScore(0);

    delete m_apkEnemy[0];
    m_apkEnemy[0] = NULL;
    delete m_apkEnemy[1];
    m_apkEnemy[1] = NULL;
    delete m_apkEnemy[2];
    m_apkEnemy[2] = NULL;
    delete m_apkEnemy[3];
    m_apkEnemy[3] = NULL;

    // -g_cfSpawnInterval causes spawning right after level start
    m_afKillTime[0] = -g_cfSpawnInterval;
    m_afKillTime[1] = -g_cfSpawnInterval;
    m_afKillTime[2] = -g_cfSpawnInterval;
    m_afKillTime[3] = g_cfSpawnInterval * 1.5;

    m_bEagleDestroyed = false;
}

Animation *Map::SetupAnimation()
{
    Animation *pAnimation;
    pAnimation = new Animation();
    pAnimation->SetFrameRate(1.25f);
    pAnimation->SetMaxFrames(2);
    pAnimation->SetOscillate(true);
    return pAnimation;
}

bool Map::Read(SDL_RWops *pHandle, void *pData, u32 iSize, u32 iBlocks)
{
    if(SDL_RWread(pHandle, pData, iSize, iBlocks) < 0)
    {
        return false;
    }
    return true;
}

bool Map::ReadHeader(SDL_RWops *pHandle)
{
    if(!Read(pHandle, m_pMapInfo->id, 1, 4))
        return false;
    if(m_pMapInfo->id[0] != 'T' ||
        m_pMapInfo->id[1] != 'L' ||
        m_pMapInfo->id[2] != 'V' ||
        m_pMapInfo->id[3] != '1')
    {
        return false;
    }

    if(!Read(pHandle, &m_pMapInfo->iNameSize, 1, 1))
        return false;
    m_pMapInfo->strName = new char[m_pMapInfo->iNameSize + 1];
    memset(m_pMapInfo->strName, 0, m_pMapInfo->iNameSize + 1);
    if(!Read(pHandle, m_pMapInfo->strName, 1, m_pMapInfo->iNameSize))
        return false;

    if(!Read(pHandle, &m_pMapInfo->bonusArea.iX, 2, 1) ||
        !Read(pHandle, &m_pMapInfo->bonusArea.iY, 2, 1) ||
        !Read(pHandle, &m_pMapInfo->bonusArea.iW, 2, 1) ||
        !Read(pHandle, &m_pMapInfo->bonusArea.iH, 2, 1))
    {
        return false;
    }

    if(!Read(pHandle, &m_pMapInfo->p1Spawn.iX, 1, 1) ||
        !Read(pHandle, &m_pMapInfo->p1Spawn.iY, 1, 1))
    {
        return false;
    }
    if(!Read(pHandle, &m_pMapInfo->p2Spawn.iX, 1, 1) ||
        !Read(pHandle, &m_pMapInfo->p2Spawn.iY, 1, 1))
    {
        return false;
    }
    if(!Read(pHandle, &m_pMapInfo->e1Spawn.iX, 1, 1) ||
        !Read(pHandle, &m_pMapInfo->e1Spawn.iY, 1, 1))
    {
        return false;
    }
    if(!Read(pHandle, &m_pMapInfo->e2Spawn.iX, 1, 1) ||
        !Read(pHandle, &m_pMapInfo->e2Spawn.iY, 1, 1))
    {
        return false;
    }
    if(!Read(pHandle, &m_pMapInfo->e3Spawn.iX, 1, 1) ||
        !Read(pHandle, &m_pMapInfo->e3Spawn.iY, 1, 1))
    {
        return false;
    }
    if(!Read(pHandle, &m_pMapInfo->eagle.iX, 1, 1) ||
        !Read(pHandle, &m_pMapInfo->eagle.iY, 1, 1))
    {
        return false;
    }

    return true;
}

bool Map::ReadBlockPart(SDL_RWops *pHandle, int iX, int iY) {
    u8 val;
    u16 type;
    if(!Read(pHandle, &val, 1, 1))
        return false;

    switch(val) {
    default:
    case 0: type = BT_EMPTY; break;
    case 1: type = BT_BRICK; break;
    case 2: type = BT_STEEL; break;
    case 3: type = BT_SEA; break;
    case 4: type = BT_JUNGLE; break;
    case 5: type = BT_ICE; break;
    case 6: type = BT_EDGE; break;
    }

    m_aBlocks[iX][iY] = m_aOldBlocks[iX][iY] = static_cast<BLOCK_TYPE>(type);

    return true;
}
