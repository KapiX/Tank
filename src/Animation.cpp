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

#include "Animation.h"

f32 *Animation::m_pfTimer = NULL; 

void Animation::Animate() {
	if(m_bPlaying) {
		if(m_iOldTime + m_iFrameRate > *m_pfTimer)
			return;

		m_iOldTime = *m_pfTimer;
		if(m_iFrames == NULL)
		{
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
		else
		{
			m_iCurrentFrame = m_iFrames[m_iFrameIterator];
			m_iFrameIterator++;
			if(m_iFrameIterator >= m_iMaxFrames) m_iFrameIterator = 0;
		}
	}
}