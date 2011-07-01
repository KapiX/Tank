#include "OGLTexture.h"

OGLTexture::OGLTexture(const char *szFilename)
    : Texture(szFilename)
{
    m_pTextureData = IMG_Load(szFilename);

    glGenTextures(1, &m_iID);
    glBindTexture(GL_TEXTURE_2D, m_iID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, m_pTextureData->format->BytesPerPixel,
                 m_pTextureData->w, m_pTextureData->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_pTextureData->pixels);
}

OGLTexture::~OGLTexture()
{
    glDeleteTextures(1, &m_iID);
    if(m_pTextureData != NULL)
    {
        SDL_FreeSurface(m_pTextureData);
        m_pTextureData = NULL;
    }
}