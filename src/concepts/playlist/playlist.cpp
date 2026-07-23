#include "../include/audio.h"
#include "playlist.h"

using std::queue;
using std::deque;

template <typename TSource>
Playlist<TSource>::Playlist ()
    :
    tracks_ {}
{}

template <typename TSource>
Playlist<TSource>::Playlist (initializer_list<TSource> tracks)
{
    tracks_ = deque<TSource> {tracks.begin (), tracks.end ()};
}

// The functions fills the playlist with the given track. It will clear the
// existing playlist and add the new track as the only track in the playlist. 
//
template <typename TSource>
void Playlist<TSource>::assignTrack (TSource track)
{
    tracks_ = {};
    tracks_.push_back (track);
}

template <typename TSource>
void Playlist<TSource>::addTrack (TSource track)
{
    tracks_.push_back (track);
}

template <typename TSource>
const TSource* Playlist<TSource>::currentTrack ()
{
    if (tracks_.empty ())
        return nullptr;

    return &tracks_.front ();
}

template <typename TSource>
const std::deque<TSource>& Playlist<TSource>::tracks () const
{
    return tracks_;
}

template <typename TSource>
void Playlist<TSource>::clear ()
{
    tracks_ = {};
}

// This functions switches to the next track in the playlist. It removes the
// current track from the playlist and returns a pointer to the next track.
// If there are no (more) tracks in the playlist a nullptr is returned.
//
template <typename TSource>
const TSource* Playlist<TSource>::nextTrack ()
{
      if (!tracks_.empty ())
        // Remove the current track from the playlist
        tracks_.pop_front ();

    if (tracks_.empty ())
        return nullptr;

    return &tracks_.front ();
}

// As the Playlist member functions are defined outside of the header file,
// all template instantiations have to be defined explicitly.
template class Playlist<Track>;
template class Playlist<FileTrack>;