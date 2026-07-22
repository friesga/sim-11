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
// - SDLAudioStream. An audio streams consists of AudioFragments in a specific
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
    void audioCallback (void* userdata, AudioFrame* stream,
        size_t numberOfFrames);

private:
    AudioFormat const audioFormat_ {48000, 2, AudioSampleFormat::Float32};
    SDLAudioDevice audioDevice_;
    unique_ptr<SDLAudioMixer> audioMixer_;
    vector<unique_ptr<AudioPlayer>> audioPlayers_ {};
};

#endif // _SDLAUDIOSYSTEM_H_