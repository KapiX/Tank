#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "Singleton.h"

#pragma comment(lib, "SDL_mixer.lib")

enum SOUND
{
	SND_BONUS = 1,
	SND_BRICKHIT,
	SND_EEXPLOSION,
	SND_FEXPLOSION,
	SND_GAMEOVER,
	SND_ICE,
	SND_LEVELSTARTING,
	SND_LIFE,
	SND_MOVING,
	SND_NMOVING,
	SND_PAUSE,
	SND_SHIELDHIT,
	SND_SHOOT,
	SND_STEELHIT,
	SND_TBONUSHIT,

	SND_COUNT = SND_TBONUSHIT,
};

enum CHANNEL
{
	CNL_LEVELSTARTING = 0,
	CNL_EXPLOSIONS,
	CNL_MOVING,
	CNL_PAUSE,
	CNL_EFFECTS,
};

class SoundManager :
	public Singleton<SoundManager>
{
private:
	Mix_Chunk *m_pSounds[SND_COUNT];

public:
	SoundManager();
	~SoundManager();

	void Play(SOUND sound);
};

#endif