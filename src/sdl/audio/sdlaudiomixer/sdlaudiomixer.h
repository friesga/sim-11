#ifndef _SDLAUDIOMIXER_H_
#define _SDLAUDIOMIXER_H_

#include "../include/audio.h"
#include "../sdlaudiodevice/sdlaudiodevice.h"

#include <memory>
#include <string>
#include <vector>
#include <queue>

using std::unique_ptr;
using std::string;
using std::vector;
using std::queue;

class SDLAudioMixer : public AudioMixer
{
public:
    class SDLChannel;
    class Playlist;

    SDLAudioMixer (const AudioFormat& desiredAudioFormat);

    // Functions required by the AudioMixer interface
    void start () override;
    void stop () override;
    bool isPlaying () override;
    size_t createChannel (string label) override;
    AudioMixer::Channel& channel (size_t id) override;
    void setVolume (float volume) override;
    float volume () override;

    // audioCallback is defined public to make it accessible to unit tests
    void audioCallback (void* userdata, AudioFrame* stream,
        size_t numberOfFrames);

    AudioFormat audioFormat () const;

private:
    SDLAudioDevice audioDevice_;
    vector<unique_ptr<SDLChannel>> channels_;
    vector<AudioFrame> channelBuffer_;
    float volume_ {1.0};

    size_t fill (AudioFrame* stream, size_t numberOfFrames);
    void adjustVolume (AudioFrame* stream, size_t numberOfFrames);
    size_t mixInto (const AudioFrame* source, AudioFrame* destination,
        size_t numberOfFrames, float volume);
};

// The class SDLAudioMixer::Playlist is a concrete implementation
// of the Playlist interface as defined in the global namespace.
//
class SDLAudioMixer::Playlist : public ::Playlist
{
public:
    void assignTrack (Track track) override;
    void addTrack (Track track) override;
    const Track* currentTrack () override;
    const Track* nextTrack () override;
    void clear () override;

private:
    queue<Track> tracks_ {};
};


class SDLAudioMixer::SDLChannel : public AudioMixer::Channel
{
public:
    SDLChannel (string label);

    // Functions required by the AudioMixer::Channel interface
    void play (Track track) override;
    void play (initializer_list<Track> tracks) override;
    size_t fill (AudioFrame* stream, size_t numberOfFrames) override;

    // Functions required by the AudioPlayer interface
    void play (AudioStream* stream) override;
    AudioStream* source () override;
    void stop () override;
    bool isPlaying () override;
    void setVolume (float volume) override;
    float volume () override;
    
private:
    size_t id_ {0};
    string label_ {};
    Playlist playlist_;
    // AudioStream* audioStream_ {nullptr};
    float volume_ {1.0f};
    // PlaybackMode playbackMode_ {PlaybackMode::Continuous};
    bool started_ {false};

    size_t fillInContinuousMode (AudioFrame* stream, size_t numberOfFrames);
    size_t fillInOneShotMode (AudioFrame* stream, size_t numberOfFrames);
};



#endif // _SDLAUDIOMIXER_H_