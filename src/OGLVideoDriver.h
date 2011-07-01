#ifndef _OGLVIDEODRIVER_H_
#define _OGLVIDEODRIVER_H_

#define GLEW_STATIC

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include "VideoDriver.h"
#include "Singleton.h"
#include "RenderList.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32s.lib")

#pragma comment(lib, "SDL_image.lib")

class OGLVideoDriver :
    public VideoDriver,
    public Singleton<OGLVideoDriver>
{
public:
    OGLVideoDriver(u32 iWidth, u32 iHeight);
    ~OGLVideoDriver(void);

    void Clear(u8 iR, u8 iG, u8 iB, u8 iA = 255);
    void BeginScene();
    void EndScene();

    void Zoom(f32 iZoom);

    RenderList *CreateRenderList(RENDERLIST_TYPE rlt, u32 iElements);
    Texture *CreateTexture(const char *szFilename);

	void PrintText(Texture *pFont, int iX, int iY, int iZ, const char *szText, float fScale = 1.0f, int iR = 255, int iG = 255, int iB = 255, int iA = 255, int iCharW = 7, int iCharH = 13);
	void DrawSprite(Texture *pSprite, int iX, int iY, int iZ, int iX2, int iY2, int iW = 0, int iH = 0, float fScale = 1.0f);

    void DrawLine(int iX, int iY, int iX2, int iY2, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255);
	void FillRectangle(int iX, int iY, int iW, int iH, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255);

private:
    void Resize(u32 iWidth, u32 iHeight);
};

#endif