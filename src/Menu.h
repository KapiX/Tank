#ifndef _MENU_H_
#define _MENU_H_

#include "Animation.h"
#include "List.h"
#include "VideoDriver.h"
#include "Texture.h"
#include <string.h>

enum MENU_ORIENTATION
{
	MO_VERTICAL = 1,
	MO_HORIZONTAL,
};

class Menu
{
private:
	char *m_szTitle;
	List<const char *> m_Items;
	Texture *m_pPointer;
	Texture *m_pFont;
	u8 m_iCurrent;
	MENU_ORIENTATION m_Orient;
	VideoDriver *m_pVD;
	Animation *m_pPointerAnim;

public:
	Menu(VideoDriver *pVD, Texture *pFont, Texture *pPointer);
	~Menu();

	void Render(int iX, int iY);

	void NextItem();
	void PrevItem();
	void SetCurrentItem(u8 item);
	inline u8 GetCurrentItem() { return m_iCurrent; }
	void AddItem(const char *szText);
	//void ChangeItem(u8 item, const char *szText);

	inline void SetTitle(const char *szTitle) { m_szTitle = new char[strlen(szTitle)]; strcpy(m_szTitle, szTitle); }
	inline char *GetTitle() { return m_szTitle; }

	inline void SetPointerAnim(Animation *pAnimation) { m_pPointerAnim = pAnimation; }

	inline void SetOrientation(MENU_ORIENTATION orient) { m_Orient = orient; }
};

#endif