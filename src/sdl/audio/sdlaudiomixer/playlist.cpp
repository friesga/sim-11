#include "sdlaudiomixer.h"

// The functions fills the playlist with the given track. It will clear the
// existing playlist and add the new track as the only track in the playlist. 
//
void SDLAudioMixer::Playlist::assignTrack (Track track)
{
    tracks_ = {};
    tracks_.push (track);
}

void SDLAudioMixer::Playlist::addTrack (Track track)
{
    tracks_.push (track);
}

const Track* SDLAudioMixer::Playlist::currentTrack ()
{
    if (tracks_.empty ())
        return nullptr;

    return &tracks_.front ();
}

void SDLAudioMixer::Playlist::clear ()
{
    tracks_ = {};
}

// This functions switches to the next track in the playlist. It removes the
// current track from the playlist and returns a pointer to the next track.
// If there are no (more) tracks in the playlist a nullptr is returned.
//
const Track* SDLAudioMixer::Playlist::nextTrack ()
{
      if (!tracks_.empty ())
        // Remove the current track from the playlist
        tracks_.pop ();

    if (tracks_.empty ())
        return nullptr;

    return &tracks_.front ();
}
