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

#include "Config.h"
#include "Game.h"
#include "Texture.h"
#include "VideoDriver.h"
#include "Window.h"
#include "SDL.h"
#include <cstdio>

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
    // Drawing FPS
    sprintf(temp, "FPS:%d", (int) Window::GetInstance()->GetFPS());
    pVD->PrintText(pFont, 730, 6, 64, temp, 1, 0, 255, 0);

    pVD->EndScene();
}

void Cleanup()
{
    Game::GetInstance()->FreeResources();
    delete pFont;
    
    Window::GetInstance()->Shutdown();
}

int main(int argc, char *argv[])
{
    Config::GetInstance()->ReadFromFile("config.cfg");
    Window::GetInstance()->Render = Render;
    Window::GetInstance()->Update = Update;
    Window::GetInstance()->Focus = Focus;
    Window::GetInstance()->Unfocus = Unfocus;

    Window::GetInstance()->Init(800, 600, Config::GetInstance()->GetFullscreen(), (VIDEO_DRIVER) Config::GetInstance()->GetVideoDriver());

    pFont = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/font.png");
    Game::GetInstance()->LoadResources(pFont);

    Window::GetInstance()->Loop();
    
    ::Cleanup();

    Config::GetInstance()->SaveToFile("config.cfg");

    return 0;
}
