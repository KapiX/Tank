#include "Animation.h"

f32 *Animation::m_pfTimer = NULL; 

void Animation::Animate() {
	if(m_bPlaying) {
		if(m_iOldTime + m_iFrameRate > *m_pfTimer)
			return;

		m_iOldTime = *m_pfTimer;
		m_iCurrentFrame += m_iFrameInc;

		if(m_bOscillate) {
			if(m_iFrameInc > 0) 
            {
				if(m_iCurrentFrame >= m_iMaxFrames - 1)
					m_iFrameInc = -m_iFrameInc;
			}
            else
            {
				if(m_iCurrentFrame <= 0)
					m_iFrameInc = -m_iFrameInc;
			}
		} else
			if(m_iCurrentFrame >= m_iMaxFrames)
				m_iCurrentFrame = 0;
	}
}