#include "sdlaudiomixer.h"

SDLAudioMixer::SDLChannel::SDLChannel (string label)
    :
    label_ (label)
{}

void SDLAudioMixer::SDLChannel::play (AudioStream* stream)
{
    playlist_.assignTrack ({stream, PlaybackMode::Continuous});
    started_ = true;
}

void SDLAudioMixer::SDLChannel::play (Track track)
{
    track.audioStream->reset ();
    playlist_.assignTrack (track);
    started_ = true;
}

void SDLAudioMixer::SDLChannel::play (initializer_list<Track> tracks)
{
    playlist_.clear ();

    for (const auto& track : tracks)
    {
        track.audioStream->reset ();
        playlist_.addTrack (track);
    }
    started_ = true;
}

AudioStream* SDLAudioMixer::SDLChannel::source ()
{
    return playlist_.currentTrack () ?
        playlist_.currentTrack ()->audioStream :
        nullptr;
}

void SDLAudioMixer::SDLChannel::stop ()
{
    started_ = false;
}

bool SDLAudioMixer::SDLChannel::isPlaying ()
{
    return started_;
}

void SDLAudioMixer::SDLChannel::setVolume (float volume)
{
    volume_ = volume;
}

float SDLAudioMixer::SDLChannel::volume ()
{
    return volume_;
}

size_t SDLAudioMixer::SDLChannel::fill (AudioFrame* stream, size_t numberOfFrames)
{
    size_t framesFilled {0};

    while (framesFilled < numberOfFrames && started_)
    {
        const Track* track = playlist_.currentTrack ();

        if (track == nullptr || track->audioStream == nullptr)
            break;

        framesFilled += (track->playbackMode == PlaybackMode::Continuous) ?
            fillInContinuousMode (stream + framesFilled, numberOfFrames - framesFilled) :
            fillInOneShotMode (stream + framesFilled, numberOfFrames - framesFilled);
    }

    return framesFilled;
}

size_t SDLAudioMixer::SDLChannel::fillInContinuousMode (AudioFrame* stream,
    size_t numberOfFrames)
{
    size_t framesFilled {0};
    AudioStream* audioStream = playlist_.currentTrack ()->audioStream;

    while (framesFilled < numberOfFrames)
    {
        if (audioStream->eos ())
            audioStream ->reset ();

        framesFilled += audioStream->fill (stream + framesFilled,
            numberOfFrames - framesFilled);
    }

    return framesFilled;
}

size_t SDLAudioMixer::SDLChannel::fillInOneShotMode (AudioFrame* stream,
    size_t numberOfFrames)
{
    if (!started_)
        return 0;

    AudioStream* audioStream = playlist_.currentTrack ()->audioStream;

    size_t framesFilled = audioStream->fill (stream, numberOfFrames);

    if (audioStream->eos ())
    {
        // Switch to the next track in the playlist if available. If there are
        // no more tracks in the playlist, the channel will stop playing.
        playlist_.nextTrack ();
    }

    return framesFilled;
}

