#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Types.h"

class Texture
{
protected:
    u32 m_iID;

public:
    Texture(const char *szFilename) {}
    virtual ~Texture() {}

    u32 GetID() { return m_iID; }

    virtual u32 GetWidth() = 0;
    virtual u32 GetHeight() = 0;
};

#endif