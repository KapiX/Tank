/*
	Copyright 2011 Kacper Kasper

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

#include "OGLVideoDriver.h"
#include "OGLRenderList.h"
#include "OGLTexture.h"

OGLVideoDriver::OGLVideoDriver(u32 iWidth, u32 iHeight)
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Resize(iWidth, iHeight);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_GREATER, 0.1f);
    glEnable(GL_ALPHA_TEST);

    IMG_Init(IMG_INIT_PNG);
}

OGLVideoDriver::~OGLVideoDriver(void)
{
    IMG_Quit();
}

void OGLVideoDriver::Clear(u8 iR, u8 iG, u8 iB, u8 iA)
{
	glClearColor((GLclampf) iR / 255, (GLclampf) iG / 255, (GLclampf) iB / 255, (GLclampf) iA / 255);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void OGLVideoDriver::BeginScene()
{
}

void OGLVideoDriver::EndScene()
{
	SDL_GL_SwapBuffers();
}

void OGLVideoDriver::Zoom(f32 iZoom)
{
	glScalef(iZoom, iZoom, 1.0f);
}

RenderList *OGLVideoDriver::CreateRenderList(RENDERLIST_TYPE rlt, u32 iElements)
{
    return new OGLRenderList(rlt, iElements);
}

Texture *OGLVideoDriver::CreateTexture(const char *szFilename)
{
    return new OGLTexture(szFilename);
}

void OGLVideoDriver::PrintText(Texture *pFont, int iX, int iY, int iZ, const char *szText, float fScale, int iR, int iG, int iB, int iA, int iCharW, int iCharH)
{
	glColor4f(iR / 255.0f, iG / 255.0f, iB / 255.0f, iA / 255.0f);
	glBindTexture(GL_TEXTURE_2D, pFont->GetID());
	for(u32 i = 0; i < strlen(szText); i++)
	{
		f32 iS1 = (f32) (((uchar) szText[i] % 16) * (iCharW + 1)) / pFont->GetWidth();
		f32 iT1 = (f32) (((uchar) szText[i] / 16) * (iCharH + 1)) / pFont->GetHeight();
        f32 iS2 = (f32) (((uchar) szText[i] % 16 + 1) * (iCharW + 1) - 1) / pFont->GetWidth();
        f32 iT2 = (f32) (((uchar) szText[i] / 16 + 1) * (iCharH + 1) - 1) / pFont->GetHeight();

		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(iS1, iT1);
            glVertex3f((GLfloat) iX + ((iCharW + 1) * i) * fScale, (GLfloat) iY, (GLfloat) iZ);
            glTexCoord2f(iS2, iT1);
            glVertex3f((GLfloat) iX + (((iCharW + 1) * i) + iCharW) * fScale, (GLfloat) iY, (GLfloat) iZ);
            glTexCoord2f(iS1, iT2);
            glVertex3f((GLfloat) iX + ((iCharW + 1) * i) * fScale, (GLfloat) iY + iCharH * fScale, (GLfloat) iZ);
            glTexCoord2f(iS2, iT2);
            glVertex3f((GLfloat) iX + (((iCharW + 1) * i) + iCharW) * fScale, (GLfloat) iY + iCharH * fScale, (GLfloat) iZ);
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(1, 1, 1, 1);
}

void OGLVideoDriver::DrawSprite(Texture *pSprite, int iX, int iY, int iZ, int iX2, int iY2, int iW, int iH, float fScale)
{
	if(iW == 0)
	{
		iW = pSprite->GetWidth();
	}
	if(iH == 0)
	{
		iH = pSprite->GetHeight();
	}
	glBindTexture(GL_TEXTURE_2D, pSprite->GetID());
	f32 iS1 = (f32) iX2 / pSprite->GetWidth();
	f32 iT1 = (f32) iY2 / pSprite->GetHeight();
    f32 iS2 = (f32) (iX2 + iW) / pSprite->GetWidth();
    f32 iT2 = (f32) (iY2 + iH) / pSprite->GetHeight();

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(iS1, iT1);
        glVertex3f((GLfloat) iX, (GLfloat) iY, (GLfloat) iZ);
        glTexCoord2f(iS2, iT1);
        glVertex3f((GLfloat) iX + iW * fScale, (GLfloat) iY, (GLfloat) iZ);
        glTexCoord2f(iS1, iT2);
        glVertex3f((GLfloat) iX, (GLfloat) iY + iH * fScale, (GLfloat) iZ);
        glTexCoord2f(iS2, iT2);
        glVertex3f((GLfloat) iX + iW * fScale, (GLfloat) iY + iH * fScale, (GLfloat) iZ);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OGLVideoDriver::DrawLine(int iX, int iY, int iX2, int iY2, int iZ, int iR, int iG, int iB, int iA)
{
    glDisable(GL_TEXTURE_2D);
	glColor4f(iR / 255.0f, iG / 255.0f, iB / 255.0f, iA / 255.0f);
	glBegin(GL_LINES);
        glVertex3f((GLfloat) iX, (GLfloat) iY, (GLfloat) iZ);
        glVertex3f((GLfloat) iX2, (GLfloat) iY2, (GLfloat) iZ);
	glEnd();
	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}

void OGLVideoDriver::FillRectangle(int iX, int iY, int iW, int iH, int iZ, int iR, int iG, int iB, int iA)
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(iR / 255.0f, iG / 255.0f, iB / 255.0f, iA / 255.0f);
	glBegin(GL_TRIANGLE_STRIP);
        glVertex3f((GLfloat) iX, (GLfloat) iY, (GLfloat) iZ);
        glVertex3f((GLfloat) iX + iW, (GLfloat) iY, (GLfloat) iZ);
        glVertex3f((GLfloat) iX, (GLfloat) iY + iH, (GLfloat) iZ);
        glVertex3f((GLfloat) iX + iW, (GLfloat) iY + iH, (GLfloat) iZ);
	glEnd();
	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}

void OGLVideoDriver::Resize(u32 iWidth, u32 iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, iWidth, iHeight, 0.0f, -65.0f, 0.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}