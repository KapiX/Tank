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

#include "Game.h"

Game::Game(void)
{
}

Game::~Game(void)
{
}

void Game::LoadResources(Texture *pFont)
{
    Keyboard::GetInstance()->Update();

    Animation::SetTimer(&m_fTimer);
    Tank::SetTimer(&m_fTimer);
    Map::SetTimer(&m_fTimer);

    SoundManager::GetInstance();

    m_bPlayer2 = false;

    m_pMenuPointerAnim = new Animation();
    m_pMenuPointerAnim->SetFrameRate(0.1f);
    m_pMenuPointerAnim->SetMaxFrames(2);
    m_pMenuPointerAnim->SetOscillate(true);

    m_pGameFont = pFont;
    m_pAtlasTexture = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/graphics.png");

    m_pMainMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 128);
    m_pMainMenu->SetPointerAnim(m_pMenuPointerAnim);
    m_pMainMenu->SetTitle("MAIN MENU");
    m_pMainMenu->AddItem("1 PLAYER");
    m_pMainMenu->AddItem("2 PLAYERS");
    m_pMainMenu->AddItem("CONTROLS");
    //m_pMainMenu->AddItem("EDITOR");
    m_pMainMenu->AddItem("EXIT");

    m_pInGameMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 141);
    m_pInGameMenu->SetOrientation(MO_HORIZONTAL);
    m_pInGameMenu->SetTitle("PAUSE");
    m_pInGameMenu->AddItem("BACK TO GAME");
    m_pInGameMenu->AddItem("EXIT TO MENU");
    m_pInGameMenu->AddItem("EXIT TO SYSTEM");

    m_pLevelPackMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 128);
    m_pLevelPackMenu->SetPointerAnim(m_pMenuPointerAnim);
    m_pLevelPackMenu->SetDrawTitle(true);
    m_pLevelPackMenu->SetTitle("SELECT LEVEL PACK");
    m_pLevelPackMenu->AddItem("BACK");

    /* not used yet
    m_pLevelMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 128);
    m_pLevelMenu->SetPointerAnim(m_pMenuPointerAnim);
    m_pLevelMenu->SetDrawTitle(true);
    m_pLevelMenu->SetTitle("SELECT LEVEL");
    m_pLevelMenu->AddItem("BACK");
    */

    m_pPlayerSelectMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 128);
    m_pPlayerSelectMenu->SetPointerAnim(m_pMenuPointerAnim);
    m_pPlayerSelectMenu->SetOrientation(MO_HORIZONTAL);
    m_pPlayerSelectMenu->SetTitle("PLAYER");
    m_pPlayerSelectMenu->AddItem("1");
    m_pPlayerSelectMenu->AddItem("2");

    m_pP1ControlsMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 128);
    m_pP1ControlsMenu->SetPointerAnim(m_pMenuPointerAnim);
    m_pP1ControlsMenu->SetTitle("PLAYER 1 CONTROLS");
    m_pP1ControlsMenu->AddItem("CONTROLLER: ");
    m_pP1ControlsMenu->AddItem("UP:         ");
    m_pP1ControlsMenu->AddItem("DOWN:       ");
    m_pP1ControlsMenu->AddItem("LEFT:       ");
    m_pP1ControlsMenu->AddItem("RIGHT:      ");
    m_pP1ControlsMenu->AddItem("SHOOT:      ");
    m_pP1ControlsMenu->AddItem("BACK");

    m_pP2ControlsMenu = new Menu(Window::GetInstance()->GetVideoDriver(), m_pGameFont, m_pAtlasTexture, 896, 128);
    m_pP2ControlsMenu->SetPointerAnim(m_pMenuPointerAnim);
    m_pP2ControlsMenu->SetTitle("PLAYER 2 CONTROLS");
    m_pP2ControlsMenu->AddItem("CONTROLLER: ");
    m_pP2ControlsMenu->AddItem("UP:         ");
    m_pP2ControlsMenu->AddItem("DOWN:       ");
    m_pP2ControlsMenu->AddItem("LEFT:       ");
    m_pP2ControlsMenu->AddItem("RIGHT:      ");
    m_pP2ControlsMenu->AddItem("SHOOT:      ");
    m_pP2ControlsMenu->AddItem("BACK");

    m_pMap = new Map(Window::GetInstance()->GetVideoDriver(), m_pAtlasTexture);

    m_fTimer = 0;
    m_iSplashAlpha = 0;
    m_iSplashLogoY = -300;
    m_GameState = GS_SPLASH;
}

void Game::FreeResources()
{
    if(m_pMap != NULL)
    {
        delete m_pMap;
        m_pMap = NULL;
    }
    if(m_pMenuPointerAnim != NULL)
    {
        delete m_pMenuPointerAnim;
        m_pMenuPointerAnim = NULL;
    }
    if(m_pAtlasTexture != NULL)
    {
        delete m_pAtlasTexture;
        m_pAtlasTexture = NULL;
    }
    if(m_pMainMenu != NULL)
    {
        delete m_pMainMenu;
        m_pMainMenu = NULL;
    }
    if(m_pInGameMenu != NULL)
    {
        delete m_pInGameMenu;
        m_pInGameMenu = NULL;
    }
    if(m_pLevelPackMenu != NULL)
    {
        delete m_pLevelPackMenu;
        m_pLevelPackMenu = NULL;
    }
    /* not used yet
    if(m_pLevelMenu != NULL)
    {
        delete m_pLevelMenu;
        m_pLevelMenu = NULL;
    }
    */
    if(m_pPlayerSelectMenu != NULL)
    {
        delete m_pPlayerSelectMenu;
        m_pPlayerSelectMenu = NULL;
    }
    if(m_pP1ControlsMenu != NULL)
    {
        delete m_pP1ControlsMenu;
        m_pP1ControlsMenu = NULL;
    }
    if(m_pP2ControlsMenu != NULL)
    {
        delete m_pP2ControlsMenu;
        m_pP2ControlsMenu = NULL;
    }

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
    case GS_CONTROLSMENU:
        UpdateControlsMenu(fDelta);
        break;
    case GS_CONTROLSWAITING:
        UpdateControlsWaiting(fDelta);
        break;
    case GS_LEVELPACKSELECTION:
        UpdateLevelPackSelection(fDelta);
        break;
    case GS_LEVELSELECTION:
        UpdateLevelSelection(fDelta);
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
        if(m_pMainMenu->GetCurrentItem() == MM_1PLAYER || m_pMainMenu->GetCurrentItem() == MM_2PLAYERS)
        {
            m_fTimer = 0;
            if(m_pMainMenu->GetCurrentItem() == MM_2PLAYERS)
            {
                m_bPlayer2 = true;
            }
            else
            {
                m_bPlayer2 = false;
            }
            m_pMap->Set2PlayerMode(m_bPlayer2);
            
            // GS_LEVELPACKSELECTION
            std::vector<std::string> kFiles;
            Directory::GetInstance()->ListFiles("levels", &kFiles);

            for(int i = 0; i < kFiles.size(); i++)
            {
                // only level packs
                if(kFiles[i].compare(kFiles[i].length() - 3, 3, "tlp") != 0)
                {
                    continue;
                }
                m_pLevelPackMenu->InsertItem(0, kFiles[i].substr(0, kFiles[i].length() - 4)); // w/o extension
            }
            m_GameState = GS_LEVELPACKSELECTION;
        }
        else if(m_pMainMenu->GetCurrentItem() == MM_CONTROLS)
        {
            char item[64];
            char controller1[32];
            char controller2[32];
            if(Config::GetInstance()->GetP1Controller() == KEYBOARD)
            {
                strcpy(controller1, "KEYBOARD");
                sprintf(item, "UP:         %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP1Controls()->iUp));
                m_pP1ControlsMenu->ChangeItem(CM_UP, item);
                sprintf(item, "DOWN:       %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP1Controls()->iDown));
                m_pP1ControlsMenu->ChangeItem(CM_DOWN, item);
                sprintf(item, "LEFT:       %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP1Controls()->iLeft));
                m_pP1ControlsMenu->ChangeItem(CM_LEFT, item);
                sprintf(item, "RIGHT:      %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP1Controls()->iRight));
                m_pP1ControlsMenu->ChangeItem(CM_RIGHT, item);
                sprintf(item, "SHOOT:      %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP1Controls()->iShoot));
                m_pP1ControlsMenu->ChangeItem(CM_SHOOT, item);
            }
            if(Config::GetInstance()->GetP2Controller() == KEYBOARD) 
            {
                strcpy(controller2, "KEYBOARD");
                sprintf(item, "UP:         %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP2Controls()->iUp));
                m_pP2ControlsMenu->ChangeItem(CM_UP, item);
                sprintf(item, "DOWN:       %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP2Controls()->iDown));
                m_pP2ControlsMenu->ChangeItem(CM_DOWN, item);
                sprintf(item, "LEFT:       %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP2Controls()->iLeft));
                m_pP2ControlsMenu->ChangeItem(CM_LEFT, item);
                sprintf(item, "RIGHT:      %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP2Controls()->iRight));
                m_pP2ControlsMenu->ChangeItem(CM_RIGHT, item);
                sprintf(item, "SHOOT:      %s", Keyboard::GetInstance()->GetKeyName((SDLKey) Config::GetInstance()->GetP2Controls()->iShoot));
                m_pP2ControlsMenu->ChangeItem(CM_SHOOT, item);
            }
            sprintf(item, "CONTROLLER: %s", controller1);
            m_pP1ControlsMenu->ChangeItem(CM_CONTROLLER, item);
            sprintf(item, "CONTROLLER: %s", controller2);
            m_pP2ControlsMenu->ChangeItem(CM_CONTROLLER, item);

            m_GameState = GS_CONTROLSMENU;
        }
        else if(m_pMainMenu->GetCurrentItem() == MM_EXIT)
        {
            Window::GetInstance()->Quit();
        }
    }
}

void Game::UpdateControlsMenu(float fDelta)
{
    m_pMenuPointerAnim->Animate();

    if(Keyboard::GetInstance()->KeyPressed(SDLK_LEFT))
    {
        m_pPlayerSelectMenu->PrevItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RIGHT))
    {
        m_pPlayerSelectMenu->NextItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_UP))
    {
        m_pP1ControlsMenu->PrevItem();
        m_pP2ControlsMenu->PrevItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_DOWN))
    {
        m_pP1ControlsMenu->NextItem();
        m_pP2ControlsMenu->NextItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RETURN))
    {
        if(m_pPlayerSelectMenu->GetCurrentItem() == CPM_1P)
        {
            switch(m_pP1ControlsMenu->GetCurrentItem())
            {
            case CM_UP:
                m_kWaitingFor = WF_P1UP;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_DOWN:
                m_kWaitingFor = WF_P1DOWN;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_LEFT:
                m_kWaitingFor = WF_P1LEFT;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_RIGHT:
                m_kWaitingFor = WF_P1RIGHT;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_SHOOT:
                m_kWaitingFor = WF_P1SHOOT;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_BACK:
                m_fTimer = 0;
                m_GameState = GS_MAINMENU;
                break;
            }
        }
        else
        {
            switch(m_pP2ControlsMenu->GetCurrentItem())
            {
            case CM_UP:
                m_kWaitingFor = WF_P2UP;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_DOWN:
                m_kWaitingFor = WF_P2DOWN;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_LEFT:
                m_kWaitingFor = WF_P2LEFT;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_RIGHT:
                m_kWaitingFor = WF_P2RIGHT;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_SHOOT:
                m_kWaitingFor = WF_P2SHOOT;
                m_GameState = GS_CONTROLSWAITING;
                break;
            case CM_BACK:
                m_fTimer = 0;
                m_GameState = GS_MAINMENU;
                break;
            }
        }
    }
}

void Game::UpdateControlsWaiting(float fDelta)
{
    SDLKey key;
    while((key = Keyboard::GetInstance()->GetKey()) == SDLK_UNKNOWN);
    // set key to pressed state
    Keyboard::GetInstance()->KeyPressed(key);

    char item[64];
    switch(m_kWaitingFor)
    {
    case WF_P1UP:
        Config::GetInstance()->GetP1Controls()->iUp = (u32) key;
        sprintf(item, "UP:         %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP1ControlsMenu->ChangeItem(CM_UP, item);
        break;
    case WF_P1DOWN:
        Config::GetInstance()->GetP1Controls()->iDown = (u32) key;
        sprintf(item, "DOWN:       %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP1ControlsMenu->ChangeItem(CM_DOWN, item);
        break;
    case WF_P1LEFT:
        Config::GetInstance()->GetP1Controls()->iLeft = (u32) key;
        sprintf(item, "LEFT:       %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP1ControlsMenu->ChangeItem(CM_LEFT, item);
        break;
    case WF_P1RIGHT:
        Config::GetInstance()->GetP1Controls()->iRight = (u32) key;
        sprintf(item, "RIGHT:      %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP1ControlsMenu->ChangeItem(CM_RIGHT, item);
        break;
    case WF_P1SHOOT:
        Config::GetInstance()->GetP1Controls()->iShoot = (u32) key;
        sprintf(item, "SHOOT:      %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP1ControlsMenu->ChangeItem(CM_SHOOT, item);
        break;
    case WF_P2UP:
        Config::GetInstance()->GetP2Controls()->iUp = (u32) key;
        sprintf(item, "UP:         %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP2ControlsMenu->ChangeItem(CM_UP, item);
        break;
    case WF_P2DOWN:
        Config::GetInstance()->GetP2Controls()->iDown = (u32) key;
        sprintf(item, "DOWN:       %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP2ControlsMenu->ChangeItem(CM_DOWN, item);
        break;
    case WF_P2LEFT:
        Config::GetInstance()->GetP2Controls()->iLeft = (u32) key;
        sprintf(item, "LEFT:       %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP2ControlsMenu->ChangeItem(CM_LEFT, item);
        break;
    case WF_P2RIGHT:
        Config::GetInstance()->GetP2Controls()->iRight = (u32) key;
        sprintf(item, "RIGHT:      %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP2ControlsMenu->ChangeItem(CM_RIGHT, item);
        break;
    case WF_P2SHOOT:
        Config::GetInstance()->GetP2Controls()->iShoot = (u32) key;
        sprintf(item, "SHOOT:      %s", Keyboard::GetInstance()->GetKeyName(key));
        m_pP2ControlsMenu->ChangeItem(CM_SHOOT, item);
        break;
    }
    m_GameState = GS_CONTROLSMENU;
}

void Game::UpdateLevelPackSelection(float fDelta)
{
    m_pMenuPointerAnim->Animate();

    if(Keyboard::GetInstance()->KeyPressed(SDLK_UP))
    {
        m_pLevelPackMenu->PrevItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_DOWN))
    {
        m_pLevelPackMenu->NextItem();
    }
    if(Keyboard::GetInstance()->KeyPressed(SDLK_RETURN))
    {
        if(m_pLevelPackMenu->GetCurrentItem() == m_pLevelPackMenu->ItemCount() - 1)
        {
            m_pLevelPackMenu->RemoveAllItems();
            m_pLevelPackMenu->AddItem("BACK");
            m_GameState = GS_MAINMENU;
        }
        else
        {
            char szLevelPack[256] = "";
            strcat(szLevelPack, "levels/");
            strcat(szLevelPack, m_pLevelPackMenu->GetItem(m_pLevelPackMenu->GetCurrentItem()).c_str());
            strcat(szLevelPack, ".tlp");
            m_LP.Open(szLevelPack);
            m_LP.ResetCurrentLevel();

            // load level and start the game
            size_t size = 0;
            uchar *data = 0;
            m_LP.GetLevelData(1, &data, &size);
            m_pMap->LoadMap(data, size);
            delete [] data;
            m_pMap->UpdateBlocks();
            m_iLevelStartOpeningY = 300;
            m_pInGameMenu->SetCurrentItem(0);
            m_GameState = GS_LEVELSTARTING;
        }
    }
}

void Game::UpdateLevelSelection(float fDelta)
{
    m_pMenuPointerAnim->Animate();
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

    if(m_pMap->GetPlayer1()->GetLives() == 0 && !m_pMap->GetPlayer1()->IsSpawning() && !m_pMap->GetPlayer1()->IsAlive() && !m_pMap->GetPlayer1()->IsExploding())
    {
        if(m_bPlayer2 && m_pMap->GetPlayer2()->GetLives() == 0 && !m_pMap->GetPlayer2()->IsSpawning() && !m_pMap->GetPlayer2()->IsAlive() && !m_pMap->GetPlayer2()->IsExploding())
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
            m_pLevelPackMenu->RemoveAllItems();
            m_pLevelPackMenu->AddItem("BACK");
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
            m_pLevelPackMenu->RemoveAllItems();
            m_pLevelPackMenu->AddItem("BACK");
            m_GameState = GS_MAINMENU;
        }
        else
        {
            // lives are lost when spawning, so without this we would have
            // 1 life loss when starting new level
            if(m_pMap->GetPlayer1()->IsAlive()) m_pMap->GetPlayer1()->AddLives(1);
            if(m_bPlayer2 && m_pMap->GetPlayer2()->IsAlive()) m_pMap->GetPlayer2()->AddLives(1);

            size_t size = 0;
            uchar *data = 0;
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
            m_pLevelPackMenu->RemoveAllItems();
            m_pLevelPackMenu->AddItem("BACK");
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
    case GS_CONTROLSMENU:
        RenderControlsMenu();
        break;
    case GS_CONTROLSWAITING:
        RenderControlsWaiting();
        break;
    case GS_LEVELPACKSELECTION:
        RenderLevelPackSelection();
        break;
    case GS_LEVELSELECTION:
        RenderLevelSelection();
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
    pVD->DrawSprite(m_pAtlasTexture, 286, m_iSplashLogoY, 64, 0, 256, 228, 204);
    pVD->PrintText(m_pGameFont, 210, 261, 64, "                      TANK                       ", 1.0f, 255, 255, 255, m_iSplashAlpha);
    pVD->PrintText(m_pGameFont, 210, 289, 64, "    ORIGINAL SOUNDS AND GRAPHICS - NAMCO, Ltd.   ", 1.0f, 255, 255, 255, m_iSplashAlpha);
    pVD->PrintText(m_pGameFont, 210, 303, 64, "BINARIES AND SOURCE LICENSED UNDER GNU GPL VER. 3", 1.0f, 255, 255, 255, m_iSplashAlpha);
}

void Game::RenderMainMenu()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    pVD->DrawSprite(m_pAtlasTexture, 286, m_iSplashLogoY, 64, 0, 256, 228, 204);
    m_pMainMenu->Render(350, 300);
}

void Game::RenderControlsMenu()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    pVD->DrawSprite(m_pAtlasTexture, 286, m_iSplashLogoY, 64, 0, 256, 228, 204);
    m_pPlayerSelectMenu->Render(350, 300);

    if(m_pPlayerSelectMenu->GetCurrentItem() == CPM_1P)
    {
        m_pP1ControlsMenu->Render(334, 326);
    }
    else if(m_pPlayerSelectMenu->GetCurrentItem() == CPM_2P)
    {
        m_pP2ControlsMenu->Render(334, 326);
    }
}

void Game::RenderControlsWaiting()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    pVD->DrawSprite(m_pAtlasTexture, 286, m_iSplashLogoY, 62, 0, 256, 228, 204);
    m_pPlayerSelectMenu->Render(350, 300, 62);

    if(m_pPlayerSelectMenu->GetCurrentItem() == CPM_1P)
    {
        m_pP1ControlsMenu->Render(334, 326, 62);
    }
    else if(m_pPlayerSelectMenu->GetCurrentItem() == CPM_2P)
    {
        m_pP2ControlsMenu->Render(334, 326, 62);
    }

    pVD->FillRectangle(250, 250, 300, 100, 63, 0, 0, 0);
    pVD->DrawLine(250, 250, 550, 250, 64);
    pVD->DrawLine(550, 250, 550, 350, 64);
    pVD->DrawLine(550, 350, 250, 350, 64);
    pVD->DrawLine(250, 350, 250, 250, 64);
    pVD->PrintText(m_pGameFont, 250 + 94, 250 + 43, 64, "PRESS A KEY...");
}

void Game::RenderLevelPackSelection()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    pVD->DrawSprite(m_pAtlasTexture, 286, m_iSplashLogoY, 62, 0, 256, 228, 204);
    m_pLevelPackMenu->Render(350, 300);
}

void Game::RenderLevelSelection()
{
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

        // top bar
        pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
        pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
        // bottom bar
        RenderHUD();
    }
}

void Game::RenderLevelPlaying()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();

    m_pMap->Render();

    // top bar
    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    // bottom bar
    RenderHUD();
}

void Game::RenderLevelPause()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();

    m_pMap->Render();

    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    m_pInGameMenu->Render(6, 6, 64, 0, 0, 0);
    RenderHUD();
}

void Game::RenderLevelCompleted()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();

    m_pMap->Render();

    // top bar
    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    // bottom bar
    RenderHUD();
}

void Game::RenderGameOver()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();

    m_pMap->Render();

    // top bar
    pVD->FillRectangle(0, 0, 800, 24, 1, 255, 255, 255, 255);
    pVD->PrintText(m_pGameFont, 6, 6, 2, m_pMap->GetMapInfo()->strName, 1.0f, 0, 0, 0);
    // bottom bar
    RenderHUD();

    pVD->PrintText(m_pGameFont, 384, m_iGameOverY, 64, "GAME", 1.0f, 255, 0, 0, 255);
    pVD->PrintText(m_pGameFont, 384, m_iGameOverY + 14, 64, "OVER", 1.0f, 255, 0, 0, 255);
}

void Game::RenderHUD()
{
    VideoDriver *pVD = Window::GetInstance()->GetVideoDriver();
    static char lifes1[4], score1[9], enemies[3];

    pVD->FillRectangle(0, 568, 800, 32, 1, 255, 255, 255, 255);
    // Player 1
    sprintf(lifes1, "%d", m_pMap->GetPlayer1()->GetLives());
    sprintf(score1, "%08d", m_pMap->GetPlayer1()->GetScore());
    pVD->PrintText(m_pGameFont, 6, 572, 2, score1, 2.0f, 0, 0, 0);
    pVD->DrawSprite(m_pAtlasTexture, 6 + (14 + 2) * 8 + 4 - 2, 568, 2, 640, 0, 32, 32, 1.0f);
    pVD->PrintText(m_pGameFont, 6 + (14 + 2) * 8 + 4 + 32 + 4 - 2, 572, 2, lifes1, 2.0f, 0, 0, 0);
    // Player 2
    if(m_bPlayer2)
    {
        static char lifes2[4], score2[9];
        sprintf(lifes2, "%d", m_pMap->GetPlayer2()->GetLives());
        sprintf(score2, "%08d", m_pMap->GetPlayer2()->GetScore());
        pVD->PrintText(m_pGameFont, 800 - 6 - (14 + 2) * 8, 572, 2, score2, 2.0f, 0, 0, 0);
        pVD->DrawSprite(m_pAtlasTexture, 800 - (6 + (14 + 2) * 8 + 4) - 32, 568, 2, 768, 0, 32, 32, 1.0f);
        pVD->PrintText(m_pGameFont, 800 - (6 + (14 + 2) * 8 + 4 + 32 + 4) - 14, 572, 2, lifes2, 2.0f, 0, 0, 0);
    }
    // Enemies count
    sprintf(enemies, "%d", Enemy::GetEnemiesLeft());
    pVD->PrintText(m_pGameFont, 400 - (strlen(enemies) / 2 * (14 + 2)), 572, 2, enemies, 2.0f, 0, 0, 0);
    pVD->DrawSprite(m_pAtlasTexture, 400 - (strlen(enemies) / 2 * (14 + 2)) - 32, 568, 2, 128, 0, 32, 32, 1.0f);
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