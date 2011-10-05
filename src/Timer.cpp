/*
	Copyright 2011 Kacper Kasper

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

#pragma once

#include "Timer.h"

Timer::Timer()
{
    m_iStartTicks	= 0;
    m_iPausedTicks	= 0;
    m_bPaused		= false;
    m_bStarted		= false;
}

void Timer::Start()
{
    m_bStarted = true;
    m_bPaused = false;
    m_iStartTicks = SDL_GetTicks();
}

void Timer::Stop()
{
    m_bStarted = false;
    m_bPaused = false;
}

void Timer::Pause()
{
    if(m_bStarted && !m_bPaused)
    {
        m_bPaused = true;
        m_iPausedTicks = SDL_GetTicks() - m_iStartTicks;
    }
}

void Timer::Unpause()
{
    if(m_bPaused)
    {
        m_bPaused = false;
        m_iStartTicks = SDL_GetTicks() - m_iPausedTicks;
        m_iPausedTicks = 0;
    }
}

u32 Timer::GetTicks() const
{
    if(m_bStarted)
    {
        if(m_bPaused)
        {
            return m_iPausedTicks;
        }
        else
        {
            return SDL_GetTicks() - m_iStartTicks;
        }    
    }
    return 0;
}