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

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <SDL/SDL.h>
#include "VideoDriver.h"
#include "Singleton.h"

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

enum VIDEO_DRIVER
{
    VD_OPENGL = 1,
    VD_DIRECT3D8,

    VD_FORCE32 = 0xffffffff
};

class Window :
    public Singleton<Window>
{
private:
    SDL_Surface *m_pScreen;
    SDL_Event m_kEvent;
    VideoDriver *m_pVD;

    bool m_bRunning;

    f32 m_fMaxAccumulatedTime;
    f32 m_fLimit;
    f32 m_fFPS;

public:
    Window(void);
    ~Window(void);

    void Init(u32 iWidth, u32 iHeight, bool bFullscreen = false,
        VIDEO_DRIVER kVD = VD_OPENGL);
    void Shutdown();

    void Loop();

    void Quit() { m_bRunning = false; }

    void HandleEvents();

    int IsEventPending() { return SDL_PollEvent(&m_kEvent); }

    void SetPhysicsFPSLimit(f32 fLimit) { m_fLimit = fLimit; }
    f32 GetPhysicsFPSLimit() const { return m_fLimit; }

    void *GetHwnd();
    VideoDriver *GetVideoDriver() const { return m_pVD; }
    f32 GetFPS() const { return m_fFPS; }

    bool IsRunning() const { return m_bRunning; }

    void (*Update)(float fDelta);
    void (*Render)(VideoDriver *pVD);
    void (*Focus)();
    void (*Unfocus)();
};

#endif // _WINDOW_H_