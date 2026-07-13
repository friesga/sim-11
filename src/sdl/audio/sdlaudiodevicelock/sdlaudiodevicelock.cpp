#include "sdlauiodevicelock.h"

SDLAudioDeviceLock::SDLAudioDeviceLock (SDLAudioDevice& device)
    : device_ {device}
{
    device_.lock ();
}

SDLAudioDeviceLock::~SDLAudioDeviceLock ()
{
    device_.unlock ();
}

