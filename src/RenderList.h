#ifndef _RENDERLIST_H_
#define _RENDERLIST_H_

#include "Types.h"
#include "Texture.h"

struct Sprite
{
    f32 iX, iY, iW, iH;
    f32 iZ;
    f32 iS1, iT1, iS2, iT2;
};

struct Color
{
    f32 iR, iG, iB, iA;
};

struct UpdateData
{
    u32 iNum;
    f32 iX, iY, iW, iH;
    f32 iZ;
    f32 iS1, iT1, iS2, iT2;
};

enum RENDERLIST_TYPE
{
    RLT_LINKED = 1,
    RLT_NORMAL,

    RLT_FORCE32 = 0xffffffff
};

class RenderList
{
protected:
    u32 m_iElements;
    RENDERLIST_TYPE m_kRLT;
    Texture *m_pTexture;

public:
    RenderList(RENDERLIST_TYPE rlt, u32 iElements): m_kRLT(rlt), m_iElements(iElements) {}
	virtual ~RenderList(void) {}

    virtual void AddElement(Sprite *pSprite, Color *pColor = 0) = 0;

    virtual void FillBuffer() = 0;
    virtual void UpdateBuffer(u32 iCount, UpdateData *aUpdateData, Color *pUpdateColor = 0) = 0;

    void SetTexture(Texture *pTexture) { m_pTexture = pTexture; }

    virtual void Render() = 0;
};

#endif