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

#ifndef _TIMER_H_
#define _TIMER_H_

#include "Types.h"

class Timer
{
private:
    u32 m_iStartTicks;
    u32 m_iPausedTicks;

    bool m_bStarted;
    bool m_bPaused;

public:
    Timer();

    void Start();
    void Stop();
    void Pause();
    void Unpause();

    u32 GetTicks() const;

    bool IsStarted() const { return m_bStarted; }
    bool IsPaused() const { return m_bPaused; }
};

#endif // _TIMER_H_