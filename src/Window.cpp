#include "Keyboard.h"
#include "Window.h"
#include "OGLVideoDriver.h"
//#include "D3D8VideoDriver.h"
#ifdef WIN32
    #include <SDL/SDL_syswm.h>
#endif

Window::Window(void)
{
}

Window::~Window(void)
{
}

void Window::Init(u32 iWidth, u32 iHeight, bool bFullscreen, VIDEO_DRIVER kVD)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
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

void Window::HandleEvents()
{
    switch(m_kEvent.type)
    {
    case SDL_QUIT:
        m_bRunning = false;
        return;
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