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