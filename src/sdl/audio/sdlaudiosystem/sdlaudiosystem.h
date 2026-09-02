#ifndef _SDLAUDIOSYSTEM_H_
#define _SDLAUDIOSYSTEM_H_

#include "../include/audio.h"
#include "../sdlaudiomixer/sdlaudiomixer.h"

#include <memory>
#include <optional>
#include <vector>

using std::unique_ptr;
using std::optional;
using std::vector;

//
// The audio system provides the following interfaces:
// - AudioStream. An AudioStream contains a stream of AudioFragments that can
//   be played back by an AudioPlayer.
// 
// - AudioPlayer. An AudioPlayer provides all functions necessary to play
//   back an AudioStream, a WAV file, a WAV file in a zip file or a playlist.
//   All streams can be played continuous or in one shote mode.
// 
// - AudioSystem. The AudioSystem is the interface passed on to the devices
//   that need to play sounds. By means of this interface a device can create
//   an AudioPlayer which is then used to playback audio.
// 
// The SDL audio system implements the audio system interfaces and all necesssary
// functionality. The system uses the SDL2 audio functions to provide the
// functionality. It consists of the following classes:
//
// - SDLAudioStream. An audio stream consists of AudioFragments in a specific
//   AudioFormat.
// 
// - SDLAudioSystem. The audio system creates all objects to provide the
//   functionality, i.e. the SDLAudioDevice and the SDLAudioMixer and provides
//   the functionality to create an in the system integrated audio player.
// 
// - SDLAudioDevice. The audio device is an abstraction of the SDL2 audio
//   device and provides control functions (pause/unpause) and locking
//   functionality to synchronize reading and writing of the audio buffer.
//
// - SDLAudioMixer. The audio mixer contains a number of dynamically created
//   channels. To each channel an audio player is attached. The mixer combines
//   the audio of the players into one audio stream.
//
// - SDLAudioPlayer. The audio player reads WAV data from different sources
//   and delivers that data in an audio stream.
//
// The SDLAudioSystem was developed for SDL2 and has been migrated to use
// SDL3. For several reasons the audiosystem still uses the callback mechanism
// (now via SDL_AudioStreamCallback) and provides our own mixer class:
// - The callback mechanism is much better suited for the playback of
//   continuous sounds,
// - The mixer implements playback lists,
// - The mixer might be needed to implement cross fading.
//
class SDLAudioSystem : public AudioSystem
{
public:
    SDLAudioSystem ();

    // Functions required by the AudioControl interface
    void start () override;
    void stop () override;
    bool isPlaying () override;
    void setVolume (float volume) override;
    float volume () override;

    // Functions required by the AudioSystem interface
    AudioPlayer* createPlayer (optional<string> filename = nullopt) override;
    AudioFormat audioFormat () const override;

    // audioCallback is defined public to make it accessible to unit tests
    void audioCallback (void* userdata, SDL_AudioStream* audioStream,
        size_t numberOfFrames);

private:
    // Note that the initialization sequence of the members is determined by
    // the order of their declaration. The audioMixer_ and audioPlayers_ have
    // to be created before the audioDevice_ as the audioDevice_ callback uses
    // the audioMixer_ pointer and that callback can be called immediately after
    // the audioDevice_ is created.
    AudioFormat const audioFormat_ {48000, 2, AudioSampleFormat::Float32};
    unique_ptr<SDLAudioMixer> audioMixer_;
    vector<unique_ptr<AudioPlayer>> audioPlayers_ {};
    SDLAudioDevice audioDevice_;
};

#endif // _SDLAUDIOSYSTEM_H_