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

#include "Keyboard.h"
#include "Window.h"
#include <algorithm>
#ifdef WITH_OPENGL
#    include "OGLVideoDriver.h"
#    ifdef WITH_GLEW
#        include <GL/glew.h>
#    endif // WITH_GLEW
#endif // WITH_OPENGL
#ifdef WIN32
#    ifdef WITH_D3D8
#        include "D3D8VideoDriver.h"
#    endif // WITH_D3D8
#    include "SDL_syswm.h"
#    undef max
#    undef min
#endif // WIN32

Window::Window(void)
{
}

Window::~Window(void)
{
}

void Window::Init(u32 iWidth, u32 iHeight, bool bFullscreen, VIDEO_DRIVER kVD)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
    }

    Uint32 flags;
    flags = SDL_HWSURFACE | SDL_OPENGL;
    if(bFullscreen) flags |= SDL_FULLSCREEN;

#ifndef WIN32
    // on Windows Tank uses .ico icon
    SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
#endif

    m_pScreen = SDL_SetVideoMode(iWidth, iHeight, 24, flags);

    if(kVD == VD_NULL)
    {
        SDL_WM_SetCaption("Tank [NULL]", NULL);
    }
#if defined(WITH_OPENGL)
    else if(kVD == VD_OPENGL)
    {
#if defined(WITH_GLEW)
        glewInit();
#endif

        SDL_WM_SetCaption("Tank [OpenGL]", NULL);
    }
#endif
#if defined(WIN32) && defined(WITH_D3D8)
    else if(kVD == VD_DIRECT3D8)
    {
        SDL_WM_SetCaption("Tank [Direct3D8]", NULL);
    }
#endif

    SDL_ShowCursor(SDL_DISABLE);

    switch(kVD)
    {
    case VD_NULL:
        //m_pVD = new NullVideoDriver(iWidth, iHeight);
        break;
#if defined(WITH_OPENGL)
    case VD_OPENGL:
        m_pVD = new OGLVideoDriver(iWidth, iHeight);
        break;
#endif
#if defined(WIN32) && defined(WITH_D3D8)
    case VD_DIRECT3D8:
        m_pVD = new D3D8VideoDriver(iWidth, iHeight, GetHwnd());
        break;
#endif
    default:
        break;
    }

    m_fMaxAccumulatedTime = 1.0f;
    m_fLimit = 60.0f;
    m_fFPS = 0.0f;

    m_bRunning = true;
}

void Window::Shutdown()
{
    if(m_pScreen != NULL)
    {
        SDL_FreeSurface(m_pScreen);
        m_pScreen = NULL;
    }
    if(m_pVD != NULL)
    {
        delete m_pVD;
        m_pVD = NULL;
    }
    SDL_Quit();
}

void Window::Loop()
{
    int iFrames = 0;
    float fFramesTime = 0.0f;
    float fDelta = 0.0f;
    float fLastUpdateTime = SDL_GetTicks() / 1000.0f;
    float fAccumulator = 0.0f;
    float fTimeStep = 1.0f / m_fLimit;

    while(IsRunning())
    {
        fDelta = SDL_GetTicks() / 1000.0f - fLastUpdateTime;
        fLastUpdateTime += fDelta;
        fDelta = std::max(0.0f, fDelta);
        fAccumulator += fDelta;
        // Clamping the value
        fAccumulator = std::min(m_fMaxAccumulatedTime, std::max(fAccumulator, 0.0f));

        if(IsEventPending())
        {
            HandleEvents();
        }
        while(fAccumulator > fTimeStep)
        {
            Update(fTimeStep);
            fAccumulator -= fTimeStep;
        }

        Render(m_pVD);

        iFrames++;
        fFramesTime += fDelta;
        if(fFramesTime >= 1.0)
        {
            m_fFPS = iFrames / fFramesTime;
            iFrames = 0;
            fFramesTime = 0.0f;
        }
    }
}

void Window::HandleEvents()
{
    switch(m_kEvent.type)
    {
    case SDL_QUIT:
        m_bRunning = false;
        return;
    case SDL_ACTIVEEVENT:
        if(m_kEvent.active.state & SDL_APPINPUTFOCUS)
        {
            if(m_kEvent.active.gain)
            {
                Focus();
            }
            else
            {
                Unfocus();
            }
        }
        break;
    }
}

void *Window::GetHwnd()
{
#ifdef WIN32
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if(!SDL_GetWMInfo(&wmi)) return NULL;
    return wmi.window;
#else
    return NULL;
#endif
}