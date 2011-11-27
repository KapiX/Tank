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

#include "Game.h"

Game::Game(void)
{
}

Game::~Game(void)
{
	if(m_pMap != NULL)
	{
		delete m_pMap;
		m_pMap = NULL;
	}
}

void Game::LoadResources(Texture *pFont)
{
    Keyboard::GetInstance()->Update();

    Animation::SetTimer(&m_fTimer);
    Tank::SetTimer(&m_fTimer);
    Map::SetTimer(&m_fTimer);

	SoundManager::GetInstance();

    m_bPlayer2 = false;

	m_LP.Init("levels/battlecity.tlp");

	m_pMenuPointerAnim = new Animation();
	m_pMenuPointerAnim->SetFrameRate(0.1f);
	m_pMenuPointerAnim->SetMaxFrames(2);
	m_pMenuPointerAnim->SetOscillate(true);

    m_pGameFont = pFont;
	m_pLogo = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/logo.png");
	m_pMenuPointer = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/menuPointer.png");
	m_pInGameMenuPointer = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/menuPointer2.png");
	m_pHUDP1 = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/hud_p1.png");
	m_pHUDP2 = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/hud_p2.png");
	m_pPlayerTexture = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/player_yellow.png");
	m_pEnemyTexture = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/enemy.png");

	m_pMainMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pMenuPointer);
	m_pMainMenu->SetPointerAnim(m_pMenuPointerAnim);
	m_pMainMenu->SetTitle("MAIN MENU");
	m_pMainMenu->AddItem("1 PLAYER");
	m_pMainMenu->AddItem("2 PLAYERS");
	//m_pMainMenu->AddItem("CONTROLS");
	//m_pMainMenu->AddItem("EDITOR");
	m_pMainMenu->AddItem("EXIT");

    m_pInGameMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pInGameMenuPointer);
    m_pInGameMenu->SetOrientation(MO_HORIZONTAL);
	m_pInGameMenu->SetTitle("PAUSE");
	m_pInGameMenu->AddItem("BACK TO GAME");
	m_pInGameMenu->AddItem("EXIT TO MENU");
	m_pInGameMenu->AddItem("EXIT TO SYSTEM");

	m_pMap = new Map(Window::GetInstance()->GetVideoDriver(), "graphics/blocks.png", "graphics/player_yellow.png", "graphics/player_green.png", "graphics/enemy.png", "graphics/misc.png");

    m_fTimer = 0;
	m_iSplashAlpha = 0;
	m_iSplashLogoY = -300;
	m_GameState = GS_SPLASH;
}

void Game::FreeResources()
{
	m_LP.Free();
	SoundManager::GetInstance()->Free();
}

void Game::Update(float fDelta)
{
    m_fTimer += fDelta;
	switch(m_GameState)
	{
	case GS_SPLASH:
		UpdateSplash(fDelta);
		break;
	case GS_MAINMENU:
		UpdateMainMenu(fDelta);
		break;
	case GS_LEVELSTARTING:
		UpdateLevelStarting(fDelta);
		break;
	case GS_LEVELPLAYING:
		UpdateLevelPlaying(fDelta);
		break;
	case GS_LEVELPAUSE:
		UpdateLevelPause(fDelta);
		break;
	case GS_LEVELCOMPLETED:
		UpdateLevelCompleted(fDelta);
		break;
	case GS_GAMEOVER:
		UpdateGameOver(fDelta);
		break;
	}
}

void Game::UpdateSplash(float fDelta)
{
	if(m_fTimer < 3.0f)
	{
		if(m_iSplashAlpha < 255)
		{
			if(m_iSplashAlpha + 400.0f * fDelta > 255)
			{
				m_iSplashAlpha = 255;
			}
			else
				m_iSplashAlpha += 400.0f * fDelta;
		}
	}
	else if(m_fTimer > 3.0f)
	{
		if(m_iSplashAlpha > 0)
		{
			if(m_iSplashAlpha - 400.0f * fDelta < 0)
			{
				m_iSplashAlpha = 0;
				m_iSplashLogoY = 601;
			}
			else
				m_iSplashAlpha -= 400.0f * fDelta;
		}
		if(m_iSplashLogoY > 80.0f)
		{
			m_iSplashLogoY -= 100.0f * fDelta;
		}
		else if(m_iSplashLogoY <= 80.0f && m_iSplashLogoY > 75.0f)
		{
			m_fTimer = 0;
			m_GameState = GS_MAINMENU;
		} 
	}
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RETURN))
    {
        m_iSplashLogoY = 80.0f;
        m_fTimer = 0;
		m_GameState = GS_MAINMENU;
    }
}

void Game::UpdateMainMenu(float fDelta)
{
	m_pMenuPointerAnim->Animate();
    if(Keyboard::GetInstance()->KeyPressed(SDLK_UP))
    {
        m_pMainMenu->PrevItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_DOWN))
    {
        m_pMainMenu->NextItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RETURN))
    {
        m_fTimer = 0;
        if(m_pMainMenu->GetCurrentItem() == MM_1PLAYER || m_pMainMenu->GetCurrentItem() == MM_2PLAYERS)
        {
            if(m_pMainMenu->GetCurrentItem() == MM_2PLAYERS)
            {
                m_bPlayer2 = true;
                m_pMap->Set2PlayerMode(m_bPlayer2);
            }
            else
            {
                m_bPlayer2 = false;
                m_pMap->Set2PlayerMode(m_bPlayer2);
            }
			unsigned int size = 0;
			unsigned char *data = 0;
			m_LP.GetLevelData(1, &data, &size);
			m_pMap->LoadMap(data, size);
			delete [] data;
            m_pMap->UpdateBlocks();
            m_iLevelStartOpeningY = 300;
            m_pInGameMenu->SetCurrentItem(0);
            m_GameState = GS_LEVELSTARTING;
        }
        else if(m_pMainMenu->GetCurrentItem() == MM_EXIT)
        {
            Window::GetInstance()->Quit();
        }
    }
}

void Game::UpdateLevelStarting(float fDelta)
{
    if(m_fTimer > 2.0f)
    {
        if(m_iLevelStartOpeningY > 0)
        {
            m_iLevelStartOpeningY -= 300.0f * fDelta;
        }
        else if(m_iLevelStartOpeningY <= 0)
        {
            m_fTimer = 0;
            m_GameState = GS_LEVELPLAYING;
			SoundManager::GetInstance()->Play(SND_LEVELSTARTING);
        }
    }
}

void Game::UpdateLevelPlaying(float fDelta)
{
	m_pMap->Update(fDelta);

    if(Keyboard::GetInstance()->KeyPressed(SDLK_ESCAPE))
    {
        m_GameState = GS_LEVELPAUSE;
		SoundManager::GetInstance()->Play(SND_PAUSE);
    }

    if(m_pMap->GetPlayer1()->GetLifes() == 0 && !m_pMap->GetPlayer1()->IsSpawning() && !m_pMap->GetPlayer1()->IsAlive() && !m_pMap->GetPlayer1()->IsExploding())
    {
        if(m_bPlayer2 && m_pMap->GetPlayer2()->GetLifes() == 0 && !m_pMap->GetPlayer2()->IsSpawning() && !m_pMap->GetPlayer2()->IsAlive() && !m_pMap->GetPlayer2()->IsExploding())
        {
            m_iGameOverY = 600.0f;
            m_GameState = GS_GAMEOVER;
			SoundManager::GetInstance()->Stop();
        }
        else if(!m_bPlayer2)
        {
            m_iGameOverY = 600.0f;
            m_GameState = GS_GAMEOVER;
			SoundManager::GetInstance()->Stop();
        }
    }

	if(m_pMap->IsEagleDestroyed())
	{
		m_iGameOverY = 600.0f;
        m_GameState = GS_GAMEOVER;
	}

	if(Enemy::GetEnemiesLeft() == 0 && !m_pMap->GetEnemy(0)->IsAlive() && !m_pMap->GetEnemy(0)->IsSpawning() && !m_pMap->GetEnemy(0)->IsExploding() && !m_pMap->GetEnemy(1)->IsAlive() && !m_pMap->GetEnemy(1)->IsSpawning() && !m_pMap->GetEnemy(1)->IsExploding() && !m_pMap->GetEnemy(2)->IsAlive() && !m_pMap->GetEnemy(2)->IsSpawning() && !m_pMap->GetEnemy(2)->IsExploding() && !m_pMap->GetEnemy(3)->IsAlive() && !m_pMap->GetEnemy(3)->IsSpawning() && !m_pMap->GetEnemy(3)->IsExploding())
	{
		m_fOldTimer = m_fTimer;
		m_GameState = GS_LEVELCOMPLETED;
	}
}

void Game::UpdateLevelPause(float fDelta)
{
    if(Keyboard::GetInstance()->KeyPressed(SDLK_LEFT))
    {
        m_pInGameMenu->PrevItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RIGHT))
    {
        m_pInGameMenu->NextItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_ESCAPE))
    {
        m_GameState = GS_LEVELPLAYING;
		SoundManager::GetInstance()->Play(SND_PAUSE);
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RETURN))
    {
        if(m_pInGameMenu->GetCurrentItem() == IM_BACK_TO_GAME)
        {
            m_GameState = GS_LEVELPLAYING;
			SoundManager::GetInstance()->Play(SND_PAUSE);
        }
        else if(m_pInGameMenu->GetCurrentItem() == IM_EXIT_TO_MENU)
        {
            m_fTimer = 0;
            m_iSplashLogoY = 80.0f;
            m_pMainMenu->SetCurrentItem(0);
            m_pMap->Reset();
			m_LP.ResetCurrentLevel();
            m_GameState = GS_MAINMENU;
			SoundManager::GetInstance()->Stop();
        }
        else if(m_pInGameMenu->GetCurrentItem() == IM_EXIT_TO_SYSTEM)
        {
            Window::GetInstance()->Quit();
        }
    }
}

void Game::UpdateLevelCompleted(f32 fDelta)
{
	m_pMap->Update(fDelta);

    if(Keyboard::GetInstance()->KeyPressed(SDLK_ESCAPE))
    {
        m_GameState = GS_LEVELPAUSE;
    }

	if(m_fTimer - m_fOldTimer > 3.0f)
	{
		if(m_LP.GetNextLevel() > m_LP.GetLevelCount())
		{
			m_fTimer = 0;
            m_iSplashLogoY = 80.0f;
            m_pMainMenu->SetCurrentItem(0);
            m_pMap->Reset();
			m_LP.ResetCurrentLevel();
            m_GameState = GS_MAINMENU;
		}
		else
		{
			// �ycia odejmuja sie przy spawnowaniu, bez tego po przejsciu na kolejna mape tracimy zycie
			if(m_pMap->GetPlayer1()->IsAlive()) m_pMap->GetPlayer1()->AddLifes(1);
			if(m_bPlayer2 && m_pMap->GetPlayer2()->IsAlive()) m_pMap->GetPlayer2()->AddLifes(1);

			unsigned int size = 0;
			unsigned char *data = 0;
			m_LP.GetLevelData(m_LP.NextLevel(), &data, &size);
			m_pMap->LoadMap(data, size);
			delete [] data;
			m_pMap->UpdateBlocks();

			m_iLevelStartOpeningY = 300;
			m_fTimer = 0;
			m_GameState = GS_LEVELSTARTING;
		}
	}
}

void Game::UpdateGameOver(f32 fDelta)
{
	m_pMap->Update(fDelta, false);
    
    if(m_iGameOverY > 284.0f)
    {
        m_iGameOverY -= 100.0f * fDelta;
		m_fOldTimer = m_fTimer;
    }
    else if(m_iGameOverY <= 284.0f)
    {
        if(m_fTimer - m_fOldTimer > 5.0f)
		{
			m_pMainMenu->SetCurrentItem(0);
            m_pMap->Reset();
			m_LP.ResetCurrentLevel();
            m_GameState = GS_MAINMENU;
		}
    }
}

void Game::Render()
{
	switch(m_GameState)
	{
	case GS_SPLASH:
		RenderSplash();
		break;
	case GS_MAINMENU:
		RenderMainMenu();
		break;
	case GS_LEVELSTARTING:
		RenderLevelStarting();
		break;
	case GS_LEVELPLAYING:
		RenderLevelPlaying();
		break;
	case GS_LEVELPAUSE:
		RenderLevelPause();
		break;
	case GS_LEVELCOMPLETED:
		RenderLevelCompleted();
		break;
	case GS_GAMEOVER:
		RenderGameOver();
		break;
	}
}

void Game::RenderSplash()
{
	VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
	pVD->DrawSprite(m_pLogo, 286, m_iSplashLogoY, 64, 0, 0);
	pVD->PrintText(m_pGameFont, 210, 261, 64, "                      TANK                       ", 1.0f, 255, 255, 255, m_iSplashAlpha);
	pVD->PrintText(m_pGameFont, 210, 289, 64, "    ORIGINAL SOUNDS AND GRAPHICS - NAMCO, Ltd.   ", 1.0f, 255, 255, 255, m_iSplashAlpha);
	pVD->PrintText(m_pGameFont, 210, 303, 64, "BINARIES AND SOURCE LICENSED UNDER GNU GPL VER. 3", 1.0f, 255, 255, 255, m_iSplashAlpha);
}

void Game::RenderMainMenu()
{
	VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
	pVD->DrawSprite(m_pLogo, 286, m_iSplashLogoY, 64, 0, 0);
	m_pMainMenu->Render(350, 300);
}

void Game::RenderLevelStarting()
{
	VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    pVD->FillRectangle(0, 0, 800, m_iLevelStartOpeningY, 60, 117, 117, 117);
    pVD->FillRectangle(0, 600, 800, -m_iLevelStartOpeningY, 60, 117, 117, 117);
    if(m_fTimer < 2.0f)
    {
        pVD->PrintText(m_pGameFont, 400 - (strlen(m_pMap->GetMapInfo()->strName) * 7) / 2, 300 - 7, 64, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    }
    else
    {
        m_pMap->Render();

        // g�rna belka
        pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
        pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
        // dolna belka
	    RenderHUD();
    }
}

void Game::RenderLevelPlaying()
{
	VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
	
    m_pMap->Render();

    // g�rna belka
    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    // dolna belka
	RenderHUD();
}

void Game::RenderLevelPause()
{
	VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
	
    m_pMap->Render();

    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    m_pInGameMenu->Render(6, 6);
	RenderHUD();
}

void Game::RenderHUD()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    static char lifes1[4], score1[9], enemies[3];
	
    pVD->FillRectangle(0, 568, 800, 32, 1, 255, 255, 255, 255);
	// Gracz 1
    sprintf(lifes1, "%d", m_pMap->GetPlayer1()->GetLifes());
    sprintf(score1, "%08d", m_pMap->GetPlayer1()->GetScore());
	pVD->PrintText(m_pGameFont, 6, 572, 2, score1, 2.0f, 0, 0, 0);
	pVD->DrawSprite(m_pHUDP1, 6 + (14 + 2) * 8 + 4 - 2, 571, 2, 0, 0, 26, 26, 1.0f);
	pVD->PrintText(m_pGameFont, 6 + (14 + 2) * 8 + 4 + 26 + 4 - 2, 572, 2, lifes1, 2.0f, 0, 0, 0);
	// Gracz 2
    if(m_bPlayer2)
    {
        static char lifes2[4], score2[9];
        sprintf(lifes2, "%d", m_pMap->GetPlayer2()->GetLifes());
        sprintf(score2, "%08d", m_pMap->GetPlayer2()->GetScore());
	    pVD->PrintText(m_pGameFont, 800 - 6 - (14 + 2) * 8, 572, 2, score2, 2.0f, 0, 0, 0);
	    pVD->DrawSprite(m_pHUDP2, 800 - (6 + (14 + 2) * 8 + 4) - 26, 571, 2, 0, 0, 26, 26, 1.0f);
	    pVD->PrintText(m_pGameFont, 800 - (6 + (14 + 2) * 8 + 4 + 26 + 4) - 14, 572, 2, lifes2, 2.0f, 0, 0, 0);
    }
    // Liczba wrog�w
    sprintf(enemies, "%d", Enemy::GetEnemiesLeft());
    pVD->PrintText(m_pGameFont, 400 - (strlen(enemies) / 2 * (14 + 2)), 572, 2, enemies, 2.0f, 0, 0, 0);
	pVD->DrawSprite(m_pEnemyTexture, 400 - (strlen(enemies) / 2 * (14 + 2)) - 32, 568, 2, 128, 0, 32, 32, 1.0f);
}

void Game::RenderLevelCompleted()
{
	VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
	
    m_pMap->Render();

    // g�rna belka
    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    // dolna belka
	RenderHUD();
}

void Game::RenderGameOver()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();

    m_pMap->Render();

    // g�rna belka
    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    // dolna belka
	RenderHUD();

	pVD->PrintText(m_pGameFont, 384, m_iGameOverY, 64, "GAME", 1.0f, 255, 0, 0, 255);
    pVD->PrintText(m_pGameFont, 384, m_iGameOverY + 14, 64, "OVER", 1.0f, 255, 0, 0, 255);
}

void Game::Pause()
{
	if(m_GameState == GS_LEVELPLAYING)
	{
		m_GameState = GS_LEVELPAUSE;
		SoundManager::GetInstance()->Play(SND_PAUSE);
	}
}

void Game::Unpause()
{
	if(m_GameState == GS_LEVELPAUSE)
	{
		m_GameState = GS_LEVELPLAYING;
		SoundManager::GetInstance()->Play(SND_PAUSE);
	}
}