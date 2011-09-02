#ifndef _GAME_H_
#define _GAME_H_

#include <SDL/SDL.h>
#include <string>
#include <fstream>
#include <vector>
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

const u16 g_ciMaxLevels = 1024;

enum GAMESTATE
{
	GS_SPLASH = 1,
	GS_MAINMENU,
	//GS_CONTROLSMENU,
	GS_LEVELSTARTING,
	GS_LEVELPLAYING,
	GS_LEVELPAUSE,
	GS_LEVELCOMPLETED,
    GS_GAMEOVER
};

enum MAINMENU {
	MM_1PLAYER = 0,
	MM_2PLAYERS,
	//MM_CONTROLS,
	//MM_EDITOR,
	MM_EXIT
};

enum INGAMEMENU {
	IM_BACK_TO_GAME = 0,
	IM_EXIT_TO_MENU,
	IM_EXIT_TO_SYSTEM
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

public:
    Game(void);
    ~Game(void);

    void LoadResources(Texture *pFont);
    void FreeResources();

    void Update(float fDelta);
	void UpdateSplash(float fDelta);
	void UpdateMainMenu(float fDelta);
	void UpdateLevelStarting(float fDelta);
	void UpdateLevelPlaying(float fDelta);
	void UpdateLevelPause(float fDelta);
	void UpdateLevelCompleted(float fDelta);
	void UpdateGameOver(float fDelta);

    void Render();
	void RenderSplash();
	void RenderMainMenu();
	void RenderLevelStarting();
	void RenderLevelPlaying();
	void RenderLevelPause();
	void RenderLevelCompleted();
	void RenderGameOver();

    void RenderHUD();
};

#endif