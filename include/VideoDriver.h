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

#ifndef _VIDEODRIVER_H_
#define _VIDEODRIVER_H_

#include "Types.h"

class RenderList;
enum RENDERLIST_TYPE;
class Texture;

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

    // TODO colors to u8
    virtual void PrintText(Texture *pFont, int iX, int iY, int iZ, const char *szText, float fScale = 1.0f, int iR = 255, int iG = 255, int iB = 255, int iA = 255, int iCharW = 7, int iCharH = 13) = 0;
    virtual void DrawSprite(Texture *pSprite, int iX, int iY, int iZ, int iX2, int iY2, int iW = 0, int iH = 0, float fScale = 1.0f) = 0;

    virtual void DrawLine(int iX, int iY, int iX2, int iY2, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255) = 0;
    virtual void DrawRectangle(int iX, int iY, int iW, int iH, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255)
    {
        DrawLine(iX, iY, iX + iW, iY, iZ, iR, iG, iB, iA);
        DrawLine(iX + iW, iY, iX + iW, iY + iH, iZ, iR, iG, iB, iA);
        DrawLine(iX + iW, iY + iH, iX, iY + iH, iZ, iR, iG, iB, iA);
        DrawLine(iX, iY + iH, iX, iY, iZ, iR, iG, iB, iA);
    }
    virtual void FillRectangle(int iX, int iY, int iW, int iH, int iZ, int iR = 255, int iG = 255, int iB = 255, int iA = 255) = 0;

private:
    virtual void Resize(u32 iWidth, u32 iHeight) = 0;
};

#endif // _VIDEODRIVER_H_