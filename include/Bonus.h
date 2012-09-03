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

#ifndef _BONUS_H_
#define _BONUS_H_

#include "Singleton.h"
#include "Types.h"
#include <cmath>
#include <ctime>

class Animation;
class Texture;
class VideoDriver;

enum BONUS_TYPE
{
    BONUS_NONE = 0,
    BONUS_GRENADE,
    BONUS_HELMET,
    BONUS_TIME,
    BONUS_DIG,
    BONUS_LIFE,
    BONUS_STAR,
    BONUS_PISTOL,
    BONUS_BOAT
};

class Bonus : public Singleton<Bonus>
{
private:
    Animation *m_pkAnim;
    static Texture *m_pkBonusTexture;
    BONUS_TYPE m_bt;

    int m_iX;
    int m_iY;

    bool m_bIsAlive;

public:
    Bonus(BONUS_TYPE bt = BONUS_NONE);
    ~Bonus();

    void Randomize(u16 iX, u16 iY, u16 iW, u16 iH);
    void Update();
    void Render(VideoDriver *pVD);

    void SetBonusType(BONUS_TYPE bt) { m_bt = bt; }
    void SetIsAlive(bool bIsAlive) { m_bIsAlive = bIsAlive; }

    static void SetTexture(Texture *pTexture) { m_pkBonusTexture = pTexture; }

    BONUS_TYPE GetBonusType() const { return m_bt; }
    int GetX() const { return m_iX; }
    int GetY() const { return m_iY; }
    bool IsAlive() const { return m_bIsAlive; }
    Animation *GetAnimation() { return m_pkAnim; }
};

#endif // _BONUS_H_