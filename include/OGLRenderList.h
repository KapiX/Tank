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

#ifndef _OGLRENDERLIST_H_
#define _OGLRENDERLIST_H_

#include "buildconfig.h"
#include "RenderList.h"
#include "Types.h"
#ifdef WITH_GLEW
#    include <GL/glew.h>
#else
#    include "SDL_opengl.h"
#endif

class OGLRenderList :
    public RenderList
{
private:
    GLuint m_iVBOVCID;
    GLuint m_iVBOTCID;
    GLuint m_iVBOCID;

    float *m_aVerts;
    float *m_aTexCoords;
    float *m_aColors;

    u32 m_iCount;

    bool m_bUseColors;

public:
    OGLRenderList(RENDERLIST_TYPE rlt, u32 iElements);
    ~OGLRenderList(void);

    void AddElement(Sprite *pSprite, Color *pColor = 0);

    void FillBuffer();
    void UpdateBuffer(u32 iCount, UpdateData *aUpdateData, Color *pUpdateColor = 0);

    void Render();
};

#endif // _OGLRENDERLIST_H_