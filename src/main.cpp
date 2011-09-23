/**
 TODO:
 - slizganie na BT_ICE
 - kolizje pociskow lepiej odzwierciedlic
 */

#include <SDL/SDL.h>
#include <cstdio>

#include "Game.h"
#include "Window.h"

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

Texture *pFont;
char temp[16];

void Focus()
{
	Game::GetInstance()->Unpause();
}

void Unfocus()
{
	Game::GetInstance()->Pause();
}

void Update(float fDelta)
{
	Game::GetInstance()->Update(fDelta);
}

void Render(VideoDriver *pVD)
{
	pVD->BeginScene();

    pVD->Clear(0, 0, 0);
    Game::GetInstance()->Render();
	// Rysowanie FPS
	sprintf(temp, "FPS:%d", (int) Window::GetInstance()->GetFPS());
	pVD->PrintText(pFont, 730, 6, 64, temp, 1, 0, 255, 0);

    pVD->EndScene();
}

int main(int argc, char *argv[])
{
	Window::GetInstance()->Render = Render;
	Window::GetInstance()->Update = Update;
	Window::GetInstance()->Focus = Focus;
	Window::GetInstance()->Unfocus = Unfocus;

    Window::GetInstance()->Init(800, 600, false, VD_OPENGL);

	pFont = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/font.png");
	Game::GetInstance()->LoadResources(pFont);
 
	Window::GetInstance()->Loop();

    Game::GetInstance()->FreeResources();

    Window::GetInstance()->Shutdown();

    return 0;
}