#ifndef _TIMER_H_
#define _TIMER_H_

#include <SDL/SDL.h>
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

    inline bool IsStarted() const { return m_bStarted; }
    inline bool IsPaused() const { return m_bPaused; }
};

#endif