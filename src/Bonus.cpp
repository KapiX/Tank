#include "Bonus.h"

Texture *Bonus::m_pkBonusTexture = NULL;

Bonus::Bonus(BONUS_TYPE bt)
{
	m_bt = bt;
	m_pkAnim = new Animation();
	m_pkAnim->SetFrameRate(0.7f);
	m_pkAnim->SetMaxFrames(2);
	m_pkAnim->SetOscillate(true);
	m_pkAnim->SetPlaying(true);
	m_bIsAlive = false;
}

Bonus::~Bonus()
{
	if(m_pkAnim) {
		delete m_pkAnim;
		m_pkAnim = NULL;
	}
}

void Bonus::Randomize(u16 iX, u16 iY, u16 iW, u16 iH)
{
	m_iX = rand() % iW + iX;
	m_iY = rand() % iH + iY;
	m_bt = static_cast<BONUS_TYPE>((rand() % BONUS_PISTOL) + 1);
	m_bIsAlive = true;
}

void Bonus::Update()
{
	m_pkAnim->Animate();
}

void Bonus::Render(VideoDriver *pVD)
{
	if(m_bIsAlive)
    {
        u32 iX2, iY2;
        iX2 = m_bt * 32;
        iY2 = m_pkAnim->GetCurrentFrame() * 32;
        pVD->DrawSprite(m_pkBonusTexture, m_iX, m_iY, 58.0f, iX2, iY2, 32, 32);
	}
}