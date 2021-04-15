#include <iostream>
#include <SDL_mixer.h>
#include <SDL.h>
#include "entt.hpp"
#include "world.hpp"
#include "common.hpp"

class SoundSystem
{
public:
	SoundSystem(WorldSystem* world);
	~SoundSystem();

	int volume;

	void step(float elapsed_ms);

private:
	// de-allocate channels if no sound is playing 
	void deallocate_channel();
	WorldSystem* world;
};

Mix_Chunk* cache_chunk(std::string key);
void play_sound(std::string file_path);