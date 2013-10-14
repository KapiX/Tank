/*
    Copyright 2011-2013 Kacper Kasper <kacperkasper@gmail.com>

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

#include "7ZipArchive.h"
#include "SoundManager.h"
#include "Types.h"

SoundManager::SoundManager() {
    const char *astrSoundNames[SND_COUNT] = {
        "bonus.ogg",
        "brickhit.ogg",
        "eexplosion.ogg",
        "fexplosion.ogg",
        "gameover.ogg",
        "ice.ogg",
        "levelstarting.ogg",
        "life.ogg",
        "moving.ogg",
        "nmoving.ogg",
        "pause.ogg",
        "shieldhit.ogg",
        "shoot.ogg",
        "steelhit.ogg",
        "tbonushit.ogg"
    };
    
    Mix_OpenAudio(96000, MIX_DEFAULT_FORMAT, 1, 1024);

    Mix_AllocateChannels(5);

    Mix_Volume(CNL_MOVING, 64);
    
    SevenZipArchive *pkSoundPack;
    pkSoundPack = new SevenZipArchive();
    pkSoundPack->Open("sounds/sounds.7z");
    
    u8 *pBuffer;
    size_t iSoundSize;
    SDL_RWops *pRW;
    
    // Load all the sounds into memory
    for(u32 i = 0; i < SND_COUNT; i++)
    {
        pkSoundPack->Extract(astrSoundNames[i], &pBuffer, &iSoundSize);
        pRW = SDL_RWFromMem(pBuffer, iSoundSize);
        m_pSounds[i] = Mix_LoadWAV_RW(pRW, 1);
    }
    
    pkSoundPack->Close();
    delete pkSoundPack;
}

void SoundManager::Free()
{
    for(u32 i = 0; i < SND_COUNT; i++)
    {
        Mix_FreeChunk(m_pSounds[i]);
        m_pSounds[i] = NULL;
    }

    Mix_CloseAudio();
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