#include "Keyboard.h"
#include "Window.h"
#include "OGLVideoDriver.h"
//#include "D3D8VideoDriver.h"
#include <algorithm>
#ifdef WIN32
    #include <SDL/SDL_syswm.h>
	#undef max
	#undef min
#endif

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

    m_pScreen = SDL_SetVideoMode(iWidth, iHeight, 24, flags);

    if(kVD == VD_OPENGL)
    {
        glewInit();

		SDL_WM_SetCaption("Tank [OpenGL]", NULL);

        if(GLEW_ARB_vertex_buffer_object)
        {
            SDL_WM_SetCaption("Tank [OpenGL, using VBO]", NULL);
        }
    }

	SDL_ShowCursor(SDL_DISABLE);

    switch(kVD)
    {
    case VD_OPENGL:
        m_pVD = new OGLVideoDriver(iWidth, iHeight);
        break;
    //case VD_DIRECT3D8:
    //    m_pVD = new D3D8VideoDriver(iWidth, iHeight, GetHwnd());
    //    break;
    default:
        return;
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
		// przyciêcie wartoœci
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

		Render();

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