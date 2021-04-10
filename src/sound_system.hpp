#include <iostream>
#include <SDL_mixer.h>
#include <SDL.h>
#include "entt.hpp"
#include "common.hpp"

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	int volume;

	void step(float elapsed_ms);
	
private:
	Mix_Chunk* laser_attack_sound = nullptr;
	// de-allocate channels if no sound is playing 
	void deallocate_channel();
};