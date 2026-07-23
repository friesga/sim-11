#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

#include <queue>
#include <initializer_list>

using std::deque;
using std::initializer_list;

template <typename TSource>
class Playlist
{
public:
    Playlist ();
    Playlist (initializer_list<TSource> tracks);

    void assignTrack (TSource track);
    void addTrack (TSource track);
    const TSource* currentTrack ();
    const TSource* nextTrack ();
    const std::deque<TSource>& tracks () const;
    void clear ();

private:
    // We use a std::dequeue instead of std::queue as deque supports an
    // iterator and the default underlying container of std:queue is a
    // deque anyway.
    deque<TSource> tracks_ {};
};

#endif // _PLAYLIST_H_