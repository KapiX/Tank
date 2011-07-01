#ifndef _OGLTEXTURE_H_
#define _OGLTEXTURE_H_

#include "Texture.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

class OGLTexture :
    public Texture
{
private:
    SDL_Surface *m_pTextureData;

public:
    OGLTexture(const char *szFilename);
    ~OGLTexture();

    u32 GetWidth() { return m_pTextureData->w; }
    u32 GetHeight() { return m_pTextureData->h; }
};

#endif