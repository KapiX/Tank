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
    //VD_DIRECT3D8,

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
	void (*Render)();
	void (*Focus)();
	void (*Unfocus)();
};

#endif