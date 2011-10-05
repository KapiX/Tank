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

#include "SoundManager.h"

SoundManager::SoundManager()
{
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(96000, MIX_DEFAULT_FORMAT, 1, 1024);

	Mix_AllocateChannels(5);

	Mix_Volume(CNL_MOVING, 64);
	
	m_pSounds[SND_BONUS] = Mix_LoadWAV("sounds/bonus.ogg");
	m_pSounds[SND_BRICKHIT] = Mix_LoadWAV("sounds/brickhit.ogg");
	m_pSounds[SND_EEXPLOSION] = Mix_LoadWAV("sounds/eexplosion.ogg");
	m_pSounds[SND_FEXPLOSION] = Mix_LoadWAV("sounds/fexplosion.ogg");
	m_pSounds[SND_GAMEOVER] = Mix_LoadWAV("sounds/gameover.ogg");
	m_pSounds[SND_ICE] = Mix_LoadWAV("sounds/ice.ogg");
	m_pSounds[SND_LEVELSTARTING] = Mix_LoadWAV("sounds/levelstarting.ogg");
	m_pSounds[SND_LIFE] = Mix_LoadWAV("sounds/life.ogg");
	m_pSounds[SND_MOVING] = Mix_LoadWAV("sounds/moving.ogg");
	m_pSounds[SND_NMOVING] = Mix_LoadWAV("sounds/nmoving.ogg");
	m_pSounds[SND_PAUSE] = Mix_LoadWAV("sounds/pause.ogg");
	m_pSounds[SND_SHIELDHIT] = Mix_LoadWAV("sounds/shieldhit.ogg");
	m_pSounds[SND_SHOOT] = Mix_LoadWAV("sounds/shoot.ogg");
	m_pSounds[SND_STEELHIT] = Mix_LoadWAV("sounds/steelhit.ogg");
	m_pSounds[SND_TBONUSHIT] = Mix_LoadWAV("sounds/tbonushit.ogg");
}

void SoundManager::Free()
{
	for(int i = 1; i < SND_COUNT; i++)
	{
		Mix_FreeChunk(m_pSounds[i]);
		m_pSounds[i] = NULL;
	}

	Mix_CloseAudio();
	Mix_Quit();
}

void SoundManager::Play(SOUND sound)
{
	if(Mix_Playing(CNL_LEVELSTARTING))
	{
		Mix_Volume(CNL_MOVING, 0);
		Mix_Volume(CNL_EFFECTS, 0);
		Mix_Volume(CNL_EXPLOSIONS, 0);
	}
	else
	{
		Mix_Volume(CNL_MOVING, 64);
		Mix_Volume(CNL_EFFECTS, 128);
		Mix_Volume(CNL_EXPLOSIONS, 128);
	}

	switch(sound)
	{
	case SND_EEXPLOSION:
	case SND_FEXPLOSION:
		Mix_PlayChannel(CNL_EXPLOSIONS, m_pSounds[sound], 0);
		return;
	case SND_BONUS:
	case SND_BRICKHIT:
	case SND_GAMEOVER:
	case SND_ICE:
	case SND_LIFE:
	case SND_SHIELDHIT:
	case SND_SHOOT:
	case SND_STEELHIT:
	case SND_TBONUSHIT:
		Mix_PlayChannel(CNL_EFFECTS, m_pSounds[sound], 0);
		return;
	case SND_MOVING:
	case SND_NMOVING:
		if(!Mix_Playing(CNL_MOVING))
			Mix_PlayChannel(CNL_MOVING, m_pSounds[sound], 0);
		return;
	case SND_LEVELSTARTING:
		Mix_PlayChannel(CNL_LEVELSTARTING, m_pSounds[sound], 0);
		return;
	case SND_PAUSE:
		if(Mix_Paused(-1) > 0)
		{
			Mix_Resume(-1);
		}
		else
		{
			Mix_Pause(-1);
		}
		Mix_PlayChannel(CNL_PAUSE, m_pSounds[sound], 0);
		return;
	}
}

void SoundManager::Stop()
{
	Mix_HaltChannel(-1);
}