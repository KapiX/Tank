#ifndef _OGLRENDERLIST_H_
#define _OGLRENDERLIST_H_

#define GLEW_STATIC

#include "RenderList.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

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

#endif