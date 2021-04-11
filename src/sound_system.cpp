#include "sound_system.hpp"


void adjust_volume(int channel)
{
    int channel_playing = Mix_Playing(-1);
    
    // multiple projectiles sounds stacked together may sound loud
    // decrease the volume for all channels
    //std::cout << "Channel playing: " << Mix_Playing(-1) << "\n";
    for (int i = 0; i < Mix_AllocateChannels(-1); i++)
    {
        int volume = 110 - 4 * (channel_playing);
        Mix_Volume(i, volume);
        std::cout << "Volume: " << volume << "\n";
    }
    
}

void play_sound(SoundRef& sound_ref)
{
    int channel_played = Mix_PlayChannel(-1, sound_ref.sound_reference, 0);
    // if no channels are free
    if (channel_played == -1) {
        // allocate new channels
        Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
        channel_played = Mix_PlayChannel(-1, sound_ref.sound_reference, 0);
    }
    adjust_volume(channel_played);
    sound_ref.play_delay_counter_ms = sound_ref.play_delay_ms;
    sound_ref.channel_num = channel_played;
    sound_ref.play_sound = false;
}

void sound_on_destroy(entt::basic_registry<entt::entity>& registry, entt::entity entity) {
    //std::cout << "SoundRef destoryed\n";
    auto& sound_ref = registry.get<SoundRef>(entity);
    if (!sound_ref.on_impact_destory)
    {
        Mix_HaltChannel(sound_ref.channel_num);
        // free memory
        if (sound_ref.sound_reference != nullptr)
            Mix_FreeChunk(sound_ref.sound_reference);
    }
    else
    {
        if (sound_ref.play_sound)
        {
            play_sound(sound_ref);
        }
    }

}

void sound_on_construct(entt::basic_registry<entt::entity>& registry, entt::entity entity) {
    std::cout << "SoundRef constructed\n";
}

// constructor for SoundSystem
SoundSystem::SoundSystem()
{
    volume = 110;
    // reserve 10 channels for world
    Mix_AllocateChannels(10);
    auto destroy_sink = registry.on_destroy<SoundRef>();
    destroy_sink.connect<&sound_on_destroy>();


}


SoundSystem::~SoundSystem()
{
    Mix_CloseAudio();
}


void SoundSystem::step(float elasped_ms)
{
    // resize channel_sound_ref
    auto sound_ref_view = registry.view<SoundRef>();

    // play all sounds continuously if the entity exists
	for (auto entity : sound_ref_view)
	{
        auto& sound_ref = registry.get<SoundRef>(entity);
        Mix_Chunk* chunk = registry.get<SoundRef>(entity).sound_reference;
        if (sound_ref.play_sound)
        {
            if (sound_ref.play_delay_counter_ms > 0)
            {
                sound_ref.play_delay_counter_ms -= elasped_ms;
            }
            else
            {
                play_sound(sound_ref);
            }
        }
	}
    deallocate_channel();
}

// de-allocate channels if no sound is playing 
void SoundSystem::deallocate_channel()
{
    if (Mix_Playing(-1) == 0 )
    {
        // reserve 10 channels for world
        Mix_AllocateChannels(10);
    }
}