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

#include "Map.h"
#include <math.h>

Animation *Map::m_pAnimation = NULL;
f32 *Map::m_pfTimer = NULL;

Map::Map(VideoDriver *pVD, const char *szBlockTextureName, const char *szPlayer1TextureName, const char *szPlayer2TextureName, const char *szEnemyTextureName, const char *szMiscTextureName)
{
    m_pVD = pVD;

    if(Map::m_pAnimation == NULL)
		m_pAnimation = Map::SetupAnimation();

    m_b2PlayerMode = false;
	m_pBlockTexture = pVD->CreateTexture(szBlockTextureName);
    m_pPlayer1Texture = pVD->CreateTexture(szPlayer1TextureName);
    m_pPlayer2Texture = pVD->CreateTexture(szPlayer2TextureName);
    m_pEnemyTexture = pVD->CreateTexture(szEnemyTextureName);
    m_pMiscTexture = pVD->CreateTexture(szMiscTextureName);
    m_pBonusTexture = pVD->CreateTexture("graphics/bonuses.png");
    m_pRenderList = pVD->CreateRenderList(RLT_LINKED, 1750);
	m_pRenderList->SetTexture(m_pBlockTexture);

    Bonus::SetTexture(m_pBonusTexture);

    m_pkPlayer1 = new Player(pVD, 1, m_pPlayer1Texture, m_pMiscTexture);
    m_pkPlayer1->SetLifes(3);
    m_pkPlayer1->SetScore(0);

    m_pkPlayer2 = new Player(pVD, 2, m_pPlayer2Texture, m_pMiscTexture);
    m_pkPlayer2->SetLifes(3);
    m_pkPlayer2->SetScore(0);

    m_apkEnemy[0] = NULL;
    m_apkEnemy[1] = NULL;
    m_apkEnemy[2] = NULL;
    m_apkEnemy[3] = NULL;

    m_afKillTime[0] = -g_cfSpawnInterval;
    m_afKillTime[1] = -g_cfSpawnInterval;
    m_afKillTime[2] = -g_cfSpawnInterval;
    m_afKillTime[3] = g_cfSpawnInterval * 1.5;
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
        // degeneruj¹cy quad
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
    /*if(m_apkEnemy)
	{*/
		delete [] m_apkEnemy;
		//m_pkPlayer2 = NULL;
	//}
	if(m_pRenderList != NULL)
	{
		delete m_pRenderList;
		m_pRenderList = NULL;
	}
	if(m_pBlockTexture != NULL)
	{
		delete m_pBlockTexture;
		m_pBlockTexture = NULL;
	}
    if(m_pPlayer1Texture != NULL)
	{
		delete m_pPlayer1Texture;
		m_pPlayer1Texture = NULL;
	}
    if(m_pPlayer2Texture != NULL)
	{
		delete m_pPlayer2Texture;
		m_pPlayer2Texture = NULL;
	}
    if(m_pEnemyTexture != NULL)
	{
		delete m_pEnemyTexture;
		m_pEnemyTexture = NULL;
	}
    if(m_pMiscTexture != NULL)
	{
		delete m_pMiscTexture;
		m_pMiscTexture = NULL;
	}
    if(m_pBonusTexture != NULL)
	{
		delete m_pBonusTexture;
		m_pBonusTexture = NULL;
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

    m_aOldBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2] = m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2] = BT_EAGLELTOP;
	m_aOldBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2] = m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2] = BT_EAGLERTOP;
    m_aOldBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 1] = m_aBlocks[m_pMapInfo->eagle.iX * 2][m_pMapInfo->eagle.iY * 2 + 1] = BT_EAGLELBOT;
    m_aOldBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 1] = m_aBlocks[m_pMapInfo->eagle.iX * 2 + 1][m_pMapInfo->eagle.iY * 2 + 1] = BT_EAGLERBOT;

    m_pkPlayer1->SetSpawnPoint(m_pMapInfo->p1Spawn.iX, m_pMapInfo->p1Spawn.iY);
    m_pkPlayer2->SetSpawnPoint(m_pMapInfo->p2Spawn.iX, m_pMapInfo->p2Spawn.iY);

    m_pkPlayer1->SetIsAlive(false);
    m_pkPlayer2->SetIsAlive(false);
    m_pkPlayer1->SetIsMoving(false);
    m_pkPlayer2->SetIsMoving(false);

	m_pkPlayer1->ResetAnimations();
	m_pkPlayer2->ResetAnimations();

	Bonus::GetInstance()->SetBonusType(BONUS_NONE);
	Bonus::GetInstance()->GetAnimation()->Reset();

	m_pAnimation->Reset(); // woda

	delete m_apkEnemy[0];
    m_apkEnemy[0] = NULL;
    delete m_apkEnemy[1];
    m_apkEnemy[1] = NULL;
    delete m_apkEnemy[2];
    m_apkEnemy[2] = NULL;
    delete m_apkEnemy[3];
    m_apkEnemy[3] = NULL;

	// -g_cfSpawnInterval powoduje spawnowanie natychmiast po starcie poziomu
    m_afKillTime[0] = -g_cfSpawnInterval;
    m_afKillTime[1] = -g_cfSpawnInterval;
    m_afKillTime[2] = -g_cfSpawnInterval;
    m_afKillTime[3] = g_cfSpawnInterval * 1.5;

	m_bEagleDestroyed = false;

    Enemy::SetEnemiesLeft(20);
    Enemy::SetBonusesLeft(5);

	return true;
}

void Map::CalculateDamageUp(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_HTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        if(*pkBlock2 == BT_BRICK_LTOP || *pkBlock2 == BT_BRICK_RTOP || *pkBlock2 == BT_BRICK_HTOP || *pkBlock2 == BT_EMPTY || *pkBlock2 == BT_JUNGLE || *pkBlock2 == BT_ICE || *pkBlock2 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_RTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_LTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }

    switch(*pkBlock2)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_HTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        if(*pkBlock1 == BT_BRICK_LTOP || *pkBlock1 == BT_BRICK_RTOP || *pkBlock1 == BT_BRICK_HTOP || *pkBlock1 == BT_EMPTY || *pkBlock1 == BT_JUNGLE || *pkBlock1 == BT_ICE || *pkBlock1 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        tmp2 = BT_EMPTY;
        break;
    case BT_BRICK_VRIGHT:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_RTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_LTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
}

void Map::CalculateDamageRight(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_VRIGHT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_RTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_RBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        if(*pkBlock2 == BT_BRICK_VRIGHT || *pkBlock2 == BT_BRICK_RBOT || *pkBlock2 == BT_BRICK_RTOP || *pkBlock2 == BT_EMPTY || *pkBlock2 == BT_JUNGLE || *pkBlock2 == BT_ICE || *pkBlock2 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }
    switch(*pkBlock2)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_VRIGHT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_RTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_RBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        if(*pkBlock1 == BT_BRICK_VRIGHT || *pkBlock1 == BT_BRICK_RBOT || *pkBlock1 == BT_BRICK_RTOP || *pkBlock1 == BT_EMPTY || *pkBlock1 == BT_JUNGLE || *pkBlock1 == BT_ICE || *pkBlock1 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
}

void Map::CalculateDamageDown(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_HBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        if(*pkBlock2 == BT_BRICK_LBOT || *pkBlock2 == BT_BRICK_RBOT || *pkBlock2 == BT_BRICK_HBOT || *pkBlock2 == BT_EMPTY || *pkBlock2 == BT_JUNGLE || *pkBlock2 == BT_ICE || *pkBlock2 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_RBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_LBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }

    switch(*pkBlock2)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_HBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
    case BT_BRICK_LBOT:
    case BT_BRICK_RBOT:
        if(*pkBlock1 == BT_BRICK_LBOT || *pkBlock1 == BT_BRICK_RBOT || *pkBlock1 == BT_BRICK_HBOT || *pkBlock1 == BT_EMPTY || *pkBlock1 == BT_JUNGLE || *pkBlock1 == BT_ICE || *pkBlock1 == BT_SEA || (kTL == TL_4  && !bEnemy))
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
    case BT_BRICK_LTOP:
    case BT_BRICK_RTOP:
        tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_RBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_LBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
}

void Map::CalculateDamageLeft(TANKLEVEL kTL, BLOCK_TYPE *pkBlock1, BLOCK_TYPE *pkBlock2, bool bEnemy)
{
    BLOCK_TYPE tmp1, tmp2;
    tmp1 = *pkBlock1;
    tmp2 = *pkBlock2;

    switch(*pkBlock1)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_VLEFT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_LTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
        else
            tmp1 = BT_BRICK_LBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        if(*pkBlock2 == BT_BRICK_VLEFT || *pkBlock2 == BT_BRICK_LBOT || *pkBlock2 == BT_BRICK_LTOP || *pkBlock2 == BT_EMPTY || *pkBlock2 == BT_JUNGLE || *pkBlock2 == BT_ICE || *pkBlock2 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp1 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }
    switch(*pkBlock2)
    {
    case BT_BRICK:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_VLEFT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HTOP:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_LTOP;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_HBOT:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
        else
            tmp2 = BT_BRICK_LBOT;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VLEFT:
    case BT_BRICK_LBOT:
    case BT_BRICK_LTOP:
        if(*pkBlock1 == BT_BRICK_VLEFT || *pkBlock1 == BT_BRICK_LBOT || *pkBlock1 == BT_BRICK_LTOP || *pkBlock1 == BT_EMPTY || *pkBlock1 == BT_JUNGLE || *pkBlock1 == BT_ICE || *pkBlock1 == BT_SEA || (kTL == TL_4 && !bEnemy))
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_BRICK_VRIGHT:
    case BT_BRICK_RBOT:
    case BT_BRICK_RTOP:
        tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_BRICKHIT);
        break;
    case BT_STEEL:
        if(kTL == TL_4 && !bEnemy)
            tmp2 = BT_EMPTY;
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
        break;
	case BT_EDGE:
		if(!bEnemy) SoundManager::GetInstance()->Play(SND_STEELHIT);
		break;
    }

    *pkBlock1 = tmp1;
    *pkBlock2 = tmp2;
}

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

        // TANK-BLOCK collision
        int i = (int) (pkPlayer->GetX() + 8) / 16;
        int j = (int) (pkPlayer->GetY() + 8 - 24) / 16;

        switch(pkPlayer->GetDirection())
        {
        case DIR_UP:
            top1 = pkPlayer->GetY();
            bottom2 = j * 16 + 24;
            if(top1 < bottom2)
            {
                if((m_aBlocks[i][j - 1] != BT_EMPTY && m_aBlocks[i][j - 1] != BT_ICE && m_aBlocks[i][j - 1] != BT_JUNGLE) ||
                   (m_aBlocks[i + 1][j - 1] != BT_EMPTY && m_aBlocks[i + 1][j - 1] != BT_ICE && m_aBlocks[i + 1][j - 1] != BT_JUNGLE))
				{
                    if(m_aBlocks[i][j - 1] == BT_SEA && m_aBlocks[i + 1][j - 1] == BT_SEA && pkPlayer->GetBoat()) ;
					else
					{
						pkPlayer->SetY(bottom2);
					}
					pkPlayer->Slide(0.0f);
				}
				if(pkPlayer->IsMoving() && m_aBlocks[i][j] == BT_ICE && m_aBlocks[i + 1][j] == BT_ICE) pkPlayer->Slide(32.0f);
            }
            break;
        case DIR_DOWN:
            bottom1 = pkPlayer->GetY() + 32;
            top2 = (j + 2) * 16 + 24;
            if(bottom1 > top2)
            {
                if((m_aBlocks[i][j + 2] != BT_EMPTY && m_aBlocks[i][j + 2] != BT_ICE && m_aBlocks[i][j + 2] != BT_JUNGLE) ||
                   (m_aBlocks[i + 1][j + 2] != BT_EMPTY && m_aBlocks[i + 1][j + 2] != BT_ICE && m_aBlocks[i + 1][j + 2] != BT_JUNGLE))
				{
                    if(m_aBlocks[i][j + 2] == BT_SEA && m_aBlocks[i + 1][j + 2] == BT_SEA && pkPlayer->GetBoat()) ;
					else
					{
						pkPlayer->SetY(top2 - 32);
					}
					pkPlayer->Slide(0.0f);
				}
				if(pkPlayer->IsMoving() && m_aBlocks[i][j + 1] == BT_ICE && m_aBlocks[i + 1][j + 1] == BT_ICE) pkPlayer->Slide(32.0f);
            }
            break;
        case DIR_RIGHT:
            right1 = pkPlayer->GetX() + 32;
            left2 = (i + 2) * 16;
            if(right1 > left2)
            {
                if((m_aBlocks[i + 2][j] != BT_EMPTY && m_aBlocks[i + 2][j] != BT_ICE && m_aBlocks[i + 2][j] != BT_JUNGLE) ||
                   (m_aBlocks[i + 2][j + 1] != BT_EMPTY && m_aBlocks[i + 2][j + 1] != BT_ICE && m_aBlocks[i + 2][j + 1] != BT_JUNGLE))
				{
                    if(m_aBlocks[i + 2][j] == BT_SEA && m_aBlocks[i + 2][j + 1] == BT_SEA && pkPlayer->GetBoat()) ;
					else
					{
						pkPlayer->SetX(left2 - 32);
					}
					pkPlayer->Slide(0.0f);
				}
				if(pkPlayer->IsMoving() && m_aBlocks[i + 1][j] == BT_ICE && m_aBlocks[i + 1][j + 1] == BT_ICE) pkPlayer->Slide(32.0f);
            }
            break;
        case DIR_LEFT:
            left1 = pkPlayer->GetX();
            right2 = i * 16;
            if(left1 < right2)
            {
                if((m_aBlocks[i - 1][j] != BT_EMPTY && m_aBlocks[i - 1][j] != BT_ICE && m_aBlocks[i - 1][j] != BT_JUNGLE) ||
                   (m_aBlocks[i - 1][j + 1] != BT_EMPTY && m_aBlocks[i - 1][j + 1] != BT_ICE && m_aBlocks[i - 1][j + 1] != BT_JUNGLE))
				{
                    if(m_aBlocks[i - 1][j] == BT_SEA && m_aBlocks[i - 1][j + 1] == BT_SEA && pkPlayer->GetBoat()) // do nothing
						;
					else
					{
						pkPlayer->SetX(right2);
					}
					pkPlayer->Slide(0.0f);
				}
				if(pkPlayer->IsMoving() && m_aBlocks[i][j] == BT_ICE && m_aBlocks[i + 1][j + 1] == BT_ICE) pkPlayer->Slide(32.0f);
            }
            break;
        }

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
                    pkPlayer->AddLifes(1);
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

		// TANK-ENEMY collision
		for(u8 e = 0; e < 4; e++)
		{
			if(m_apkEnemy[e] != NULL)
			{
                if(m_apkEnemy[e]->IsAlive() && !m_apkEnemy[e]->IsOnSpawn() && pkPlayer->IsMoving() && pkPlayer->IsAlive())
				{
					top1 = pkPlayer->GetY();
					bottom2 = m_apkEnemy[e]->GetY() + 32;
					left1 = pkPlayer->GetX();
					right2 = m_apkEnemy[e]->GetX() + 32;
					bottom1 = pkPlayer->GetY() + 32;
					top2 = m_apkEnemy[e]->GetY();
					right1 = pkPlayer->GetX() + 32;
					left2 = m_apkEnemy[e]->GetX();
					if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
					{
						switch(pkPlayer->GetDirection())
                        {
                        case DIR_UP:
                            pkPlayer->SetY(pkPlayer->GetY() + pkPlayer->GetSpeed() * fDelta);
                            break;
                        case DIR_DOWN:
                            pkPlayer->SetY(pkPlayer->GetY() - pkPlayer->GetSpeed() * fDelta);
                            break;
                        case DIR_LEFT:
                            pkPlayer->SetX(pkPlayer->GetX() + pkPlayer->GetSpeed() * fDelta);
                            break;
                        case DIR_RIGHT:
                            pkPlayer->SetX(pkPlayer->GetX() - pkPlayer->GetSpeed() * fDelta);
                            break;
                        }
					}
				}
                if(pkPlayer->IsAlive() && !pkPlayer->IsOnSpawn() && m_apkEnemy[e]->IsMoving() && m_apkEnemy[e]->IsAlive() && !m_apkEnemy[e]->GetStopped())
				{
					top1 = m_apkEnemy[e]->GetY();
					bottom2 = pkPlayer->GetY() + 32;
					left1 = m_apkEnemy[e]->GetX();
					right2 = pkPlayer->GetX() + 32;
					bottom1 = m_apkEnemy[e]->GetY() + 32;
					top2 = pkPlayer->GetY();
					right1 = m_apkEnemy[e]->GetX() + 32;
					left2 = pkPlayer->GetX();
					if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
					{
						switch(m_apkEnemy[e]->GetDirection())
                        {
                        case DIR_UP:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_DOWN:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_LEFT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_RIGHT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        }
					}
				}

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
								CalculateDamageUp(pkPlayer->GetBullet(b)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i + 1][j]);
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
								CalculateDamageDown(pkPlayer->GetBullet(b)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i + 1][j]);
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
								CalculateDamageRight(pkPlayer->GetBullet(b)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i][j + 1]);
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
								CalculateDamageLeft(pkPlayer->GetBullet(b)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i][j + 1]);
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

    if(iPCount == 2)
    {
        // TANK-TANK collision
        if (m_pkPlayer1->IsAlive() && m_pkPlayer1->IsMoving() && !m_pkPlayer2->IsOnSpawn())
        {
            top1 = m_pkPlayer1->GetY();
            bottom2 = m_pkPlayer2->GetY() + 32;
            left1 = m_pkPlayer1->GetX();
            right2 = m_pkPlayer2->GetX() + 32;
            bottom1 = m_pkPlayer1->GetY() + 32;
            top2 = m_pkPlayer2->GetY();
            right1 = m_pkPlayer1->GetX() + 32;
            left2 = m_pkPlayer2->GetX();
            if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
            {
                switch(m_pkPlayer1->GetDirection())
                {
                case DIR_UP:
                    m_pkPlayer1->SetY(m_pkPlayer1->GetY() + m_pkPlayer1->GetSpeed() * fDelta);
                    break;
                case DIR_DOWN:
                    m_pkPlayer1->SetY(m_pkPlayer1->GetY() - m_pkPlayer1->GetSpeed() * fDelta);
                    break;
                case DIR_LEFT:
                    m_pkPlayer1->SetX(m_pkPlayer1->GetX() + m_pkPlayer1->GetSpeed() * fDelta);
                    break;
                case DIR_RIGHT:
                    m_pkPlayer1->SetX(m_pkPlayer1->GetX() - m_pkPlayer1->GetSpeed() * fDelta);
                    break;
                }
            }
        }
        if (m_pkPlayer2->IsAlive() && m_pkPlayer2->IsMoving() && !m_pkPlayer1->IsOnSpawn())
        {
            top1 = m_pkPlayer2->GetY();
            bottom2 = m_pkPlayer1->GetY() + 32;
            left1 = m_pkPlayer2->GetX();
            right2 = m_pkPlayer1->GetX() + 32;
            bottom1 = m_pkPlayer2->GetY() + 32;
            top2 = m_pkPlayer1->GetY();
            right1 = m_pkPlayer2->GetX() + 32;
            left2 = m_pkPlayer1->GetX();
            if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2))
            {
                switch(m_pkPlayer2->GetDirection())
                {
                case DIR_UP:
                    m_pkPlayer2->SetY(m_pkPlayer2->GetY() + m_pkPlayer2->GetSpeed() * fDelta);
                    break;
                case DIR_DOWN:
                    m_pkPlayer2->SetY(m_pkPlayer2->GetY() - m_pkPlayer2->GetSpeed() * fDelta);
                    break;
                case DIR_LEFT:
                    m_pkPlayer2->SetX(m_pkPlayer2->GetX() + m_pkPlayer2->GetSpeed() * fDelta);
                    break;
                case DIR_RIGHT:
                    m_pkPlayer2->SetX(m_pkPlayer2->GetX() - m_pkPlayer2->GetSpeed() * fDelta);
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
            if(m_apkEnemy[e]->IsAlive())
            {
                // ENEMY-ENEMY collision
                if (m_apkEnemy[e]->IsMoving() && !m_apkEnemy[e]->IsOnSpawn())
                {
                    top1 = m_apkEnemy[e]->GetY();
                    bottom2 = m_apkEnemy[(e + 1) % 4]->GetY() + 32;
                    left1 = m_apkEnemy[e]->GetX();
                    right2 = m_apkEnemy[(e + 1) % 4]->GetX() + 32;
                    bottom1 = m_apkEnemy[e]->GetY() + 32;
                    top2 = m_apkEnemy[(e + 1) % 4]->GetY();
                    right1 = m_apkEnemy[e]->GetX() + 32;
                    left2 = m_apkEnemy[(e + 1) % 4]->GetX();
                    if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2) && !m_apkEnemy[(e + 1) % 4]->IsOnSpawn())
                    {
                        switch(m_apkEnemy[e]->GetDirection())
                        {
                        case DIR_UP:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_DOWN:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_LEFT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_RIGHT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        }
                        m_apkEnemy[e]->SetDirection((DIRECTION) (rand() % 4 + 1));
                    }
                    bottom2 = m_apkEnemy[(e + 2) % 4]->GetY() + 32;
                    right2 = m_apkEnemy[(e + 2) % 4]->GetX() + 32;
                    top2 = m_apkEnemy[(e + 2) % 4]->GetY();
                    left2 = m_apkEnemy[(e + 2) % 4]->GetX();
                    if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2) && !m_apkEnemy[(e + 2) % 4]->IsOnSpawn())
                    {
                        switch(m_apkEnemy[e]->GetDirection())
                        {
                        case DIR_UP:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_DOWN:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_LEFT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_RIGHT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        }
                        m_apkEnemy[e]->SetDirection((DIRECTION) (rand() % 4 + 1));
                    }
                    bottom2 = m_apkEnemy[(e + 3) % 4]->GetY() + 32;
                    right2 = m_apkEnemy[(e + 3) % 4]->GetX() + 32;
                    top2 = m_apkEnemy[(e + 3) % 4]->GetY();
                    left2 = m_apkEnemy[(e + 3) % 4]->GetX();
                    if ((top1 < bottom2) && (bottom1 > top2) && (right1 > left2) && (left1 < right2) && !m_apkEnemy[(e + 3) % 4]->IsOnSpawn())
                    {
                        switch(m_apkEnemy[e]->GetDirection())
                        {
                        case DIR_UP:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_DOWN:
                            m_apkEnemy[e]->SetY(m_apkEnemy[e]->GetY() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_LEFT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() + m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        case DIR_RIGHT:
                            m_apkEnemy[e]->SetX(m_apkEnemy[e]->GetX() - m_apkEnemy[e]->GetSpeed() * fDelta);
                            break;
                        }
                        m_apkEnemy[e]->SetDirection((DIRECTION) (rand() % 4 + 1));
                    }
                }

                // ENEMY-BLOCK collision
                int i = (int) (m_apkEnemy[e]->GetX() + 8) / 16;
                int j = (int) (m_apkEnemy[e]->GetY() + 8 - 24) / 16;

                switch(m_apkEnemy[e]->GetDirection())
                {
                case DIR_UP:
                    top1 = m_apkEnemy[e]->GetY();
                    bottom2 = j * 16 + 24;
                    if(top1 < bottom2)
                    {
                        if((m_aBlocks[i][j - 1] != BT_EMPTY && m_aBlocks[i][j - 1] != BT_ICE && m_aBlocks[i][j - 1] != BT_JUNGLE) ||
                           (m_aBlocks[i + 1][j - 1] != BT_EMPTY && m_aBlocks[i + 1][j - 1] != BT_ICE && m_aBlocks[i + 1][j - 1] != BT_JUNGLE))
                        {
                            DIRECTION dir = (DIRECTION) (rand() % 3 + 2); // bez DIR_UP
							m_apkEnemy[e]->SetY(bottom2);
                            m_apkEnemy[e]->SetDirection(dir);
                        }
                    }
                    break;
                case DIR_DOWN:
                    bottom1 = m_apkEnemy[e]->GetY() + 32;
                    top2 = (j + 2) * 16 + 24;
                    if(bottom1 > top2)
                    {
                        if((m_aBlocks[i][j + 2] != BT_EMPTY && m_aBlocks[i][j + 2] != BT_ICE && m_aBlocks[i][j + 2] != BT_JUNGLE) ||
                           (m_aBlocks[i + 1][j + 2] != BT_EMPTY && m_aBlocks[i + 1][j + 2] != BT_ICE && m_aBlocks[i + 1][j + 2] != BT_JUNGLE))
                        {
                            DIRECTION dir = (DIRECTION) (rand() % 3 + 1);
                            if(dir == 3) dir = (DIRECTION) 4; // bez DIR_DOWN
							m_apkEnemy[e]->SetY(top2 - 32);
                            m_apkEnemy[e]->SetDirection(dir);
                        }
                    }
                    break;
                case DIR_RIGHT:
                    right1 = m_apkEnemy[e]->GetX() + 32;
                    left2 = (i + 2) * 16;
                    if(right1 > left2)
                    {
                        if((m_aBlocks[i + 2][j] != BT_EMPTY && m_aBlocks[i + 2][j] != BT_ICE && m_aBlocks[i + 2][j] != BT_JUNGLE) ||
                           (m_aBlocks[i + 2][j + 1] != BT_EMPTY && m_aBlocks[i + 2][j + 1] != BT_ICE && m_aBlocks[i + 2][j + 1] != BT_JUNGLE))
                        {
                            DIRECTION dir = (DIRECTION) (rand() % 3 + 2);
                            if(dir == 2) dir = (DIRECTION) 1; // bez DIR_RIGHT
							m_apkEnemy[e]->SetX(left2 - 32);
                            m_apkEnemy[e]->SetDirection(dir);
                        }
                    }
                    break;
                case DIR_LEFT:
                    left1 = m_apkEnemy[e]->GetX();
                    right2 = i * 16;
                    if(left1 < right2)
                    {
                        if((m_aBlocks[i - 1][j] != BT_EMPTY && m_aBlocks[i - 1][j] != BT_ICE && m_aBlocks[i - 1][j] != BT_JUNGLE) ||
                           (m_aBlocks[i - 1][j + 1] != BT_EMPTY && m_aBlocks[i - 1][j + 1] != BT_ICE && m_aBlocks[i - 1][j + 1] != BT_JUNGLE))
                        {
                            DIRECTION dir = (DIRECTION) (rand() % 3 + 1); // bez DIR_LEFT
							m_apkEnemy[e]->SetX(right2);
                            m_apkEnemy[e]->SetDirection(dir);
                        }
                    }
                    break;
                }
            }
        
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
								CalculateDamageUp(m_apkEnemy[e]->GetBullet(0)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i + 1][j], true);
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
								CalculateDamageDown(m_apkEnemy[e]->GetBullet(0)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i + 1][j], true);
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
								CalculateDamageRight(m_apkEnemy[e]->GetBullet(0)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i][j + 1], true);
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
								CalculateDamageLeft(m_apkEnemy[e]->GetBullet(0)->GetTankLevel(), &m_aBlocks[i][j], &m_aBlocks[i][j + 1], true);
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
			ud.iS1 = (u8) (m_aBlocks[i][j] >> 8);
			ud.iT1 = (u8) m_aBlocks[i][j] + (m_aBlocks[i][j] == BT_SEA ? m_pAnimation->GetCurrentFrame() * 8 : 0);
		    ud.iS2 = ud.iS1 + 8;
		    ud.iT2 = ud.iT1 + 8;
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
			    ud.iS1 = (u8) (m_aBlocks[i][j] >> 8);
			    ud.iT1 = (u8) m_aBlocks[i][j] + (m_aBlocks[i][j] == BT_SEA ? m_pAnimation->GetCurrentFrame() * 8 : 0);
		        ud.iS2 = ud.iS1 + 8;
		        ud.iT2 = ud.iT1 + 8;
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

    if(m_apkEnemy[0] == NULL)
        m_apkEnemy[0] = new Enemy(m_pVD, m_pEnemyTexture, m_pMiscTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));
    if(m_apkEnemy[1] == NULL)
        m_apkEnemy[1] = new Enemy(m_pVD, m_pEnemyTexture, m_pMiscTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));
    if(m_apkEnemy[2] == NULL)
        m_apkEnemy[2] = new Enemy(m_pVD, m_pEnemyTexture, m_pMiscTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));
    if(m_apkEnemy[3] == NULL)
        m_apkEnemy[3] = new Enemy(m_pVD, m_pEnemyTexture, m_pMiscTexture, (bool) (rand() % 2), (SHIELDLEVEL) (rand() % 4 + 1), (TANKLEVEL) (rand() % 4 + 1));

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
		/*
		 * CONTROLS
		 */
		// PLAYER 1
		if(Keyboard::GetInstance()->IsKeyDown(SDLK_UP))
		{
			m_pkPlayer1->SetDirection(DIR_UP);
			m_pkPlayer1->SetIsMoving(true);
			SoundManager::GetInstance()->Play(SND_MOVING);
		}
		else if(Keyboard::GetInstance()->IsKeyDown(SDLK_RIGHT))
		{
			m_pkPlayer1->SetDirection(DIR_RIGHT);
			m_pkPlayer1->SetIsMoving(true);
		}
		else if(Keyboard::GetInstance()->IsKeyDown(SDLK_DOWN))
		{
			m_pkPlayer1->SetDirection(DIR_DOWN);
			m_pkPlayer1->SetIsMoving(true);
		}
		else if(Keyboard::GetInstance()->IsKeyDown(SDLK_LEFT))
		{
			m_pkPlayer1->SetDirection(DIR_LEFT);
			m_pkPlayer1->SetIsMoving(true);
		}
		if(Keyboard::GetInstance()->IsKeyUp(SDLK_UP) && Keyboard::GetInstance()->IsKeyUp(SDLK_RIGHT) &&
		   Keyboard::GetInstance()->IsKeyUp(SDLK_DOWN) && Keyboard::GetInstance()->IsKeyUp(SDLK_LEFT))
		{
			m_pkPlayer1->SetIsMoving(false);
		}
		if(Keyboard::GetInstance()->KeyPressed(SDLK_RCTRL))
		{
			m_pkPlayer1->Shoot();
		}
		if(Keyboard::GetInstance()->KeyPressed(SDLK_k))
		{
			m_pkPlayer1->Destroy();
		}

		// PLAYER 2
		if(m_b2PlayerMode)
		{
			if(Keyboard::GetInstance()->IsKeyDown(SDLK_w))
			{
				m_pkPlayer2->SetDirection(DIR_UP);
				m_pkPlayer2->SetIsMoving(true);
			}
			else if(Keyboard::GetInstance()->IsKeyDown(SDLK_d))
			{
				m_pkPlayer2->SetDirection(DIR_RIGHT);
				m_pkPlayer2->SetIsMoving(true);
			}
			else if(Keyboard::GetInstance()->IsKeyDown(SDLK_s))
			{
				m_pkPlayer2->SetDirection(DIR_DOWN);
				m_pkPlayer2->SetIsMoving(true);
			}
			else if(Keyboard::GetInstance()->IsKeyDown(SDLK_a))
			{
				m_pkPlayer2->SetDirection(DIR_LEFT);
				m_pkPlayer2->SetIsMoving(true);
			}
			if(Keyboard::GetInstance()->IsKeyUp(SDLK_w) && Keyboard::GetInstance()->IsKeyUp(SDLK_d) &&
			   Keyboard::GetInstance()->IsKeyUp(SDLK_s) && Keyboard::GetInstance()->IsKeyUp(SDLK_a))
			{
				m_pkPlayer2->SetIsMoving(false);
			}
			if(Keyboard::GetInstance()->KeyPressed(SDLK_q))
			{
				m_pkPlayer2->Shoot();
			}
		}
	} // bGetInput


    Bonus::GetInstance()->Update();

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
    m_pkPlayer1 = new Player(m_pVD, 1, m_pPlayer1Texture, m_pMiscTexture);
    m_pkPlayer1->SetLifes(3);
    m_pkPlayer1->SetScore(0);

    delete m_pkPlayer2;
    m_pkPlayer2 = new Player(m_pVD, 2, m_pPlayer2Texture, m_pMiscTexture);
    m_pkPlayer2->SetLifes(3);
    m_pkPlayer2->SetScore(0);

    delete m_apkEnemy[0];
    m_apkEnemy[0] = NULL;
    delete m_apkEnemy[1];
    m_apkEnemy[1] = NULL;
    delete m_apkEnemy[2];
    m_apkEnemy[2] = NULL;
    delete m_apkEnemy[3];
    m_apkEnemy[3] = NULL;

	// -g_cfSpawnInterval powoduje spawnowanie natychmiast po starcie poziomu
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

bool Map::ReadBlockPart(SDL_RWops *pHandle, int iX, int iY)
{
	u8 val;
	u16 type;
	if(!Read(pHandle, &val, 1, 1))
		return false;

	switch(val)
	{
	case 0:
		type = BT_EMPTY;
		break;
	case 1:
		type = BT_BRICK;
		break;
	case 2:
		type = BT_STEEL;
		break;
	case 3:
		type = BT_SEA;
		break;
	case 4:
		type = BT_JUNGLE;
		break;
	case 5:
		type = BT_ICE;
		break;
	case 6:
		type = BT_EDGE;
		break;
	}

    m_aBlocks[iX][iY] = m_aOldBlocks[iX][iY] = (BLOCK_TYPE) type;

	return true;
}