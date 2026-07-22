#include "sdlaudiosystem.h"
#include "../sdlaudioplayer/sdlaudioplayer.h"
#include "../sdlaudiodevicelock/sdlauiodevicelock.h"

using std::make_unique;

// Create the audio mixer and the audio device with the callback pointer
// point at the audioCallback function of the audio mixer.
SDLAudioSystem::SDLAudioSystem ()
    :
    audioMixer_ {make_unique<SDLAudioMixer> ()},
    audioDevice_ (audioFormat_, [] (void* userdata, Uint8* stream, int length) {
    static_cast<SDLAudioSystem*>(userdata)->audioCallback (userdata,
        reinterpret_cast<AudioFrame*> (stream),
        length / static_cast<int>(sizeof (AudioFrame)));
        }, this)
{ }

void SDLAudioSystem::start ()
{
    audioDevice_.start ();
}

void SDLAudioSystem::stop ()
{
    audioDevice_.pause ();
}

bool SDLAudioSystem::isPlaying ()
{
    return audioDevice_.isPlaying ();
}

void SDLAudioSystem::setVolume (float volume)
{
    audioMixer_->setOutputVolume (volume);
}

float SDLAudioSystem::volume ()
{
    return audioMixer_->volume ();
}

AudioFormat SDLAudioSystem::audioFormat () const
{
    return audioDevice_.audioFormat ();
}

//
// This function is called by devices that need to play audio. The function 
// returns a pointer to an AudioPlayer object that can be used to play audio
// from the specified audio library.
//
AudioPlayer* SDLAudioSystem::createPlayer (optional<string> filename)
{
    if (filename.has_value ())
        audioPlayers_.push_back (make_unique<SDLAudioPlayer> (filename.value (), audioFormat_));
    else
        audioPlayers_.push_back (make_unique<SDLAudioPlayer> ());

    audioMixer_->createChannel (audioPlayers_.back ().get ());

    return audioPlayers_.back ().get ();
}

void SDLAudioSystem::audioCallback (void* userdata, AudioFrame* stream,
    size_t numberOfFrames)
{
    SDLAudioDeviceLock lock {audioDevice_};

    audioMixer_->fill (stream, numberOfFrames);

}