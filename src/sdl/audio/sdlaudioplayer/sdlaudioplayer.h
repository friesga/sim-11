#ifndef _SDLAUDIOPLAYER_H_
#define _SDLAUDIOPLAYER_H_

#include "../include/audio.h"
#include "../sdlaudiostream/sdlaudiostream.h"

#include <vector>
#include <map>
#include <string>
#include <memory>

using std::vector;
using std::map;
using std::string;
using std::unique_ptr;

// Objects of this class are created by the SDLAudioSystem class. The class
// implements the AudioPlayer interface, which is then used by devices to play
// audio. The sound fragments to be played by this player are stored in an
// audio library, which is passed in the constructor.
//
class SDLAudioPlayer : public AudioPlayer
{
public:
    SDLAudioPlayer () = default;
    SDLAudioPlayer (string filename, AudioFormat audioFormat);

    // Functions required by the AudioPlayer interface
    void play (AudioStream* stream,
        PlaybackMode playbackMode = PlaybackMode::Continuous) override;
    void play (Playlist<Track> playlist) override;
    void play (string filename,
        PlaybackMode playbackMode = PlaybackMode::Continuous) override;
    void play (Playlist<FileTrack> fileTracks) override;
    bool isPlaying () override;
    void start () override;
    void stop () override;
    void setVolume (float volume) override;
    float volume () override;
    size_t fill (AudioFrame* stream, size_t numberOfFrames) override;

private:
    map<string, unique_ptr<AudioStream>> audioStreams_ {};
    Playlist<Track> playlist_;
    bool started_ {false};
    float volume_ {1.0f};

    vector<unsigned char> readFileData (const string& name);
    Playlist<Track> convertPlaylist (Playlist<FileTrack> fileTracks);

    size_t fillCurrentTrack (AudioFrame* stream, size_t numberOfFrames);
    size_t fillInContinuousMode (AudioFrame* stream, size_t numberOfFrames);
    size_t fillInOneShotMode (AudioFrame* stream, size_t numberOfFrames);
    void adjustVolume (AudioFrame* stream, size_t numberOfFrames);
};

#endif // _SDLAUDIOPLAYER_H_