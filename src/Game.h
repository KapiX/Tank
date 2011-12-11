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

#ifndef _GAME_H_
#define _GAME_H_

#include <SDL/SDL.h>
#include <string>
#include <fstream>
#include <vector>
#include "Config.h"
#include "Map.h"
#include "RenderList.h"
#include "Singleton.h"
#include "Timer.h"
#include "Window.h"
#include "Texture.h"
#include "Menu.h"
#include "Keyboard.h"
#include "Tank.h"
#include "LevelPack.h"
#include "SoundManager.h"

const u16 g_ciMaxLevels = 1024;

enum GAMESTATE
{
	GS_SPLASH = 1,
	GS_MAINMENU,
	GS_CONTROLSMENU,
    GS_CONTROLSWAITING,
	GS_LEVELSTARTING,
	GS_LEVELPLAYING,
	GS_LEVELPAUSE,
	GS_LEVELCOMPLETED,
    GS_GAMEOVER
};

enum MAINMENU {
	MM_1PLAYER = 0,
	MM_2PLAYERS,
	MM_CONTROLS,
	//MM_EDITOR,
	MM_EXIT
};

enum INGAMEMENU {
	IM_BACK_TO_GAME = 0,
	IM_EXIT_TO_MENU,
	IM_EXIT_TO_SYSTEM
};

enum CONTROLSPLAYERMENU {
    CPM_1P = 0,
    CPM_2P
};

enum CONTROLSMENU {
    CM_CONTROLLER = 0,
    CM_UP,
    CM_DOWN,
    CM_LEFT,
    CM_RIGHT,
    CM_SHOOT,
    CM_BACK
};

enum WAITINGFOR {
    WF_P1UP = 1,
    WF_P1DOWN,
    WF_P1LEFT,
    WF_P1RIGHT,
    WF_P1SHOOT,
    WF_P2UP,
    WF_P2DOWN,
    WF_P2LEFT,
    WF_P2RIGHT,
    WF_P2SHOOT
};

class Game :
    public Singleton<Game>
{
private:
	Animation *m_pMenuPointerAnim;

    Texture *m_pGameFont;
	Texture *m_pLogo;
	Texture *m_pMenuPointer;
	Texture *m_pInGameMenuPointer;
	Texture *m_pHUDP1;
	Texture *m_pHUDP2;
	Texture *m_pPlayerTexture;
	Texture *m_pEnemyTexture;

	Menu *m_pMainMenu;
	Menu *m_pInGameMenu;

    f32 m_fTimer;
	f32 m_fOldTimer;

	Map *m_pMap;

	GAMESTATE m_GameState;

	LevelPack m_LP;

    bool m_bPlayer2;

	// SPLASH
	f32 m_iSplashAlpha;
	f32 m_iSplashLogoY;

    // LEVELSTARTING
    f32 m_iLevelStartOpeningY;

    // GAMEOVER
    f32 m_iGameOverY;

    // CONTROLS
    Menu *m_pPlayerSelectMenu;
    Menu *m_pP1ControlsMenu;
    Menu *m_pP2ControlsMenu;
    WAITINGFOR m_kWaitingFor;

public:
    Game(void);
    ~Game(void);

    void LoadResources(Texture *pFont);
    void FreeResources();

    void Update(float fDelta);
	void UpdateSplash(float fDelta);
	void UpdateMainMenu(float fDelta);
    void UpdateControlsMenu(float fDelta);
    void UpdateControlsWaiting(float fDelta);
	void UpdateLevelStarting(float fDelta);
	void UpdateLevelPlaying(float fDelta);
	void UpdateLevelPause(float fDelta);
	void UpdateLevelCompleted(float fDelta);
	void UpdateGameOver(float fDelta);

    void Render();
	void RenderSplash();
	void RenderMainMenu();
    void RenderControlsMenu();
    void RenderControlsWaiting();
	void RenderLevelStarting();
	void RenderLevelPlaying();
	void RenderLevelPause();
	void RenderLevelCompleted();
	void RenderGameOver();

    void RenderHUD();

	void Pause();
	void Unpause();
};

#endif