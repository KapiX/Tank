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

#ifndef _MENU_H_
#define _MENU_H_

#include "Types.h"
#include <vector>
#include <string>

class Animation;
class Texture;
class VideoDriver;

enum MENU_ORIENTATION
{
    MO_VERTICAL = 1,
    MO_HORIZONTAL,
};

class Menu
{
private:
    std::string m_szTitle;
    std::vector<std::string> m_Items;
    Texture *m_pPointer;
    Texture *m_pFont;
    u32 m_iPointerOffsetX;
    u32 m_iPointerOffsetY;
    u8 m_iCurrent;
    MENU_ORIENTATION m_Orient;
    VideoDriver *m_pVD;
    Animation *m_pPointerAnim;
    bool m_bDrawTitle;

public:
    Menu(VideoDriver *pVD, Texture *pFont, Texture *pPointer, u32 iPointerOffsetX = 0, u32 iPointerOffsetY = 0);
    ~Menu();

    // TODO color to u8
    void Render(int iX, int iY, int iZ = 64, int iR = 255, int iG = 255, int iB = 255);

    void NextItem();
    void PrevItem();
    void SetCurrentItem(u8 item);
    u8 GetCurrentItem() { return m_iCurrent; }
    void AddItem(std::string szText) { m_Items.push_back(szText); }
    void InsertItem(u8 item, std::string szText) { std::vector<std::string>::iterator i = m_Items.begin() + item; m_Items.insert(i, szText); }
    void ChangeItem(u8 item, std::string szText) { m_Items[item] = szText; }
    std::string GetItem(u8 item) const { return m_Items[item]; }
    void RemoveAllItems() { m_Items.clear(); }
    u32 ItemCount() { return m_Items.size(); }

    void SetTitle(std::string szTitle) { m_szTitle = szTitle; }
    char *GetTitle() { return (char *) m_szTitle.c_str(); }

    void SetDrawTitle(bool bDrawTitle) { m_bDrawTitle = bDrawTitle; }
    bool GetDrawTitle() const { return m_bDrawTitle; }

    void SetPointerAnim(Animation *pAnimation) { m_pPointerAnim = pAnimation; }

    void SetOrientation(MENU_ORIENTATION orient) { m_Orient = orient; }
};

#endif // _MENU_H_