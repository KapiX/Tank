// Code from:
// http://www.sdltutorials.com/sdl-animation/

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

    static f32 *m_pfTimer;

public:
	Animation():
		m_iCurrentFrame(0), m_iMaxFrames(0), m_iFrameInc(1),
		m_iFrameRate(100), m_iOldTime(0),
		m_bOscillate(false), m_bPlaying(true) {}
	~Animation() {}

	void Animate();

	inline void SetPlaying(bool bPlaying) { m_bPlaying = bPlaying; }
	inline void SetOscillate(bool bOscillate) { m_bOscillate = bOscillate; }
	inline void SetMaxFrames(u8 iMaxFrames) { m_iMaxFrames = iMaxFrames; }
	inline void SetCurrentFrame(u8 iCurrentFrame) { m_iCurrentFrame = iCurrentFrame; }
	inline void SetFrameRate(f32 iFrameRate) { m_iFrameRate = iFrameRate; }

	inline bool GetPlaying() const { return m_bPlaying; }
	inline bool GetOscillate() const { return m_bOscillate; }
	inline u8 GetMaxFrames() const { return m_iMaxFrames; }
	inline u8 GetCurrentFrame() const { return m_iCurrentFrame; }

    static inline void SetTimer(f32 *pfTimer) { m_pfTimer = pfTimer; }

	inline void Reset() { m_iOldTime = 0; m_iCurrentFrame = 0; m_iFrameInc = 1; }
};

#endif