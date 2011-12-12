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

    Contains code from:
    http://www.sdltutorials.com/sdl-animation/
*/

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <SDL/SDL.h>
#include "Types.h"

class Animation
{
private:
	u8 m_iCurrentFrame;
	s8 m_iFrameInc;
	f32 m_iFrameRate; // in seconds
	f32 m_iOldTime;

	bool m_bPlaying;
	bool m_bOscillate; // 1, 2, 3, 2, 1, 2, ...
	u8 m_iMaxFrames;

	u8 *m_iFrames;
	u8 m_iFrameIterator;

    static f32 *m_pfTimer;

public:
	Animation():
		m_iCurrentFrame(0), m_iMaxFrames(0), m_iFrameInc(1), m_iFrameIterator(0),
		m_iFrameRate(100), m_iOldTime(0),
		m_bOscillate(false), m_bPlaying(true) { m_iFrames = NULL; }
	~Animation() {
		if(m_iFrames != NULL)
		{
			delete [] m_iFrames;
			m_iFrames = NULL;
		}
	}

	void Animate();

	inline void SetPlaying(bool bPlaying) { m_bPlaying = bPlaying; }
	inline void SetOscillate(bool bOscillate) { m_bOscillate = bOscillate; }
	inline void SetMaxFrames(u8 iMaxFrames) { m_iMaxFrames = iMaxFrames; }
	inline void SetCurrentFrame(u8 iCurrentFrame) { m_iCurrentFrame = iCurrentFrame; }
	inline void SetFrameRate(f32 iFrameRate) { m_iFrameRate = iFrameRate; }
	inline void SetFrames(u8 *iFrames) { m_iFrames = iFrames; }

	inline bool GetPlaying() const { return m_bPlaying; }
	inline bool GetOscillate() const { return m_bOscillate; }
	inline u8 GetMaxFrames() const { return m_iMaxFrames; }
	inline u8 GetCurrentFrame() const { return m_iCurrentFrame; }
	inline u8 GetFrameIterator() const { return m_iFrameIterator; }

    static inline void SetTimer(f32 *pfTimer) { m_pfTimer = pfTimer; }

	inline void Reset() { m_iOldTime = 0; m_iCurrentFrame = 0; m_iFrameIterator = 0; m_iFrameInc = 1; }
};

#endif