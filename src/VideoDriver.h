#ifndef _VIDEODRIVER_H_
#define _VIDEODRIVER_H_

#include "Types.h"
#include "RenderList.h"
#include "Texture.h"

class VideoDriver
{
public:
    VideoDriver(void) {}
    virtual ~VideoDriver(void) {}

    virtual void Clear(u8 iR, u8 iG, u8 iB, u8 iA = 255) = 0;
    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;

    virtual void Zoom(f32 iZoom) = 0;

    virtual RenderList *CreateRenderList(RENDERLIST_TYPE rlt, u32 iElements) = 0;
    virtual Texture *CreateTexture(const char *szFilename) = 0;

	virtual void PrintText(Texture *pFont, int iX, int iY, int iZ, const char *szText, float fScale = 1.0f, int iR = 255, int iG = 255, int iB = 255, int iA = 255, int iCharW = 7, int iCharH = 13) = 0;
	virtual void DrawSprite(Texture *pSprite, int iX, int iY, int iZ, int iX2, int iY2, int iW = 0, int iH = 0, float fScale = 1.0f) = 0;

    virtual void DrawLine(int iX, int iY, int iX2, int iY2, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255) = 0;
	virtual void FillRectangle(int iX, int iY, int iW, int iH, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255) = 0;

private:
    virtual void Resize(u32 iWidth, u32 iHeight) = 0;
};

#endif