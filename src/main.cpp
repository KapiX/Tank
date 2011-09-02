/**
 TODO:
 - slizganie na BT_ICE
 - kolizje pociskow lepiej odzwierciedlic
 */

#include <SDL/SDL.h>
#include <cstdio>
#include <algorithm>

#include "Game.h"
#include "Timer.h"
#include "Window.h"

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")


#undef max
#undef min

template<typename T>
T clamp(const T &what, const T &a, const T &b)
{
    return std::min(b, std::max(what, a));
}

int main(int argc, char *argv[])
{
    Window::GetInstance()->Init(800, 600, false, VD_OPENGL);

    Timer kFPS;
    kFPS.Start();

    float fps = 0;
    int frames = 0;
    float framesTime = 0.0f;
    float dt = 0.0f;
    float lastUpdateTime = kFPS.GetTicks() / 1000.0f;
    float accumulator = 0.0f;
    const float LIMIT = 100.0f;
    const float TIME_STEP = 1.0f / LIMIT;
    const float MAX_ACCUMULATED_TIME = 1.0f;
    
	Texture *pFont = Window::GetInstance()->GetVideoDriver()->CreateTexture("graphics/font.png");
    Game::GetInstance()->LoadResources(pFont);
    char temp[16];

    while(Window::GetInstance()->IsRunning())
    {
        dt = kFPS.GetTicks() / 1000.0f - lastUpdateTime;
        lastUpdateTime += dt;
        dt = std::max(0.0f, dt);
        accumulator += dt;
        accumulator = clamp(accumulator, 0.0f, MAX_ACCUMULATED_TIME);

        if(Window::GetInstance()->IsEventPending())
        {
            Window::GetInstance()->HandleEvents();
        }
        while(accumulator > TIME_STEP)
        {
            Game::GetInstance()->Update(TIME_STEP);
            accumulator -= TIME_STEP;
        }

        Window::GetInstance()->GetVideoDriver()->BeginScene();

        Window::GetInstance()->GetVideoDriver()->Clear(0, 0, 0);
        Game::GetInstance()->Render();
		// Rysowanie FPS
		sprintf(temp, "FPS:%d", (int) fps);
		Window::GetInstance()->GetVideoDriver()->PrintText(pFont, 730, 6, 64, temp, 1, 0, 255, 0);

        Window::GetInstance()->GetVideoDriver()->EndScene();

        // liczenie FPS
		frames++;
        framesTime += dt;
        if(framesTime >= 1.0)
        {
            fps = frames / framesTime;
            frames = 0;
            framesTime = 0.0f;
        }
    }

    Game::GetInstance()->FreeResources();

    Window::GetInstance()->Shutdown();

    return 0;
}