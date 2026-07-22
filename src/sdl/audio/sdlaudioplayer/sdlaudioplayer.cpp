#include "audio.h"
#include "sdlaudioplayer.h"
#include "imagedata/zipfilereader/zipfile_reader.h"
#include "../playlist/playlist.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <stdexcept>

using std::filesystem::file_size;
using std::filesystem::path;
using std::vector;
using std::ifstream;
using std::unique_ptr;
using std::make_unique;


// The constructor performs the following steps:
// - Open the zip-file specified by the filename parameter,
// - For all files in the zip-file:
//   - Check if the file is a WAV file,
//   - If it is, read the WAV file into a buffer,
//   - Create an AudioStream object from the buffer and store it in a map,
//     using the filename as the key.
//
// Playback by this player is performed on one AudioMixer Channel. If a
// device needs to playback multiple sounds simultaneously this can be 
// accomplished by creating multiple players.
//
SDLAudioPlayer::SDLAudioPlayer (string filename, AudioFormat audioFormat)
{
    vector<unsigned char> buffer {readFileData (filename)};

    unique_ptr<ZipFileReader> zipReader = 
        make_unique<ZipFileReader> (buffer.data (), buffer.data () + buffer.size ());

    for (string wavfile : zipReader->filenames ())
    {
        vector<uint8_t> wavData = zipReader->read (wavfile);

        audioStreams_[wavfile] = make_unique<SDLAudioStream> (wavData, audioFormat);
    }
}

// The function file_size will thrown an exception if the file with the
// given name does not exist.
//
vector<unsigned char> SDLAudioPlayer::readFileData (const string& name)
{
    path inputFilePath {name};
    auto length = file_size (inputFilePath);

    vector<unsigned char> buffer (length);
    ifstream inputFile (name, std::ios_base::binary);
    inputFile.read (reinterpret_cast<char*> (buffer.data ()), length);
    inputFile.close ();
    return buffer;
}

void SDLAudioPlayer::play (AudioStream* stream, PlaybackMode playbackMode)
{
    stream->reset ();
    playlist_.assignTrack ({stream, playbackMode});
    started_ = true;
}

void SDLAudioPlayer::play (Playlist<Track> playlist)
{
    for (const Track& track : playlist.tracks ())
        track.audioStream->reset ();

    playlist_ = playlist;
    started_ = true;
}

void SDLAudioPlayer::play (string filename, PlaybackMode playbackMode)
{
    play (audioStreams_[filename].get (), playbackMode);
}

void SDLAudioPlayer::play (Playlist<FileTrack> fileTracks)
{
    play (convertPlaylist (fileTracks));
}

bool SDLAudioPlayer::isPlaying ()
{
    return started_;
}

void SDLAudioPlayer::start ()
{
    started_ = true;
}

void SDLAudioPlayer::stop ()
{
    started_ = false;
}

void SDLAudioPlayer::setVolume (float volume)
{
    volume_ = volume;
}

float SDLAudioPlayer::volume ()
{
    return volume_;
}

size_t SDLAudioPlayer::fill (AudioFrame* stream, size_t numberOfFrames)
{
    size_t framesFilled = fillCurrentTrack (stream, numberOfFrames);

    adjustVolume (stream, framesFilled);

    return framesFilled;
}

size_t SDLAudioPlayer::fillCurrentTrack (AudioFrame* stream, size_t numberOfFrames)
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

size_t SDLAudioPlayer::fillInContinuousMode (AudioFrame* stream,
    size_t numberOfFrames)
{
    size_t framesFilled {0};
    AudioStream* audioStream = playlist_.currentTrack ()->audioStream;

    while (framesFilled < numberOfFrames)
    {
        if (audioStream->eos ())
            audioStream->reset ();

        framesFilled += audioStream->fill (stream + framesFilled,
            numberOfFrames - framesFilled);
    }

    return framesFilled;
}

size_t SDLAudioPlayer::fillInOneShotMode (AudioFrame* stream,
    size_t numberOfFrames)
{
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

void SDLAudioPlayer::adjustVolume (AudioFrame* stream, size_t numberOfFrames)
{
    for (size_t frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex)
        stream[frameIndex] = (stream[frameIndex] * volume_).clampVolume ();
}

// This functions converts a Playlist of FileTracks to a Playlist of Tracks,
// using the audioStreams_ map which contains the mapping from the filename
// in the FileTrack to an audioStream*.
//
// Disclaimer: the function could be written more compact but all types are
// defined explicitly to make the intention of the code clear.
//
Playlist<Track> SDLAudioPlayer::convertPlaylist (Playlist<FileTrack> fileTracks)
{
    Playlist<Track> audioTracks {};

    // Convert FileTrack to Track
    for (const FileTrack& fileTrack : fileTracks.tracks ())
    {
        std::map<std::string, std::unique_ptr<AudioStream>>::iterator iterator =
            audioStreams_.find (fileTrack.filename);

        if (iterator == audioStreams_.end ())
            throw std::runtime_error ("Non-existing filename in playlist: " +
                fileTrack.filename);

        AudioStream* audioStream = iterator->second.get ();

        audioTracks.addTrack ({audioStream, fileTrack.playbackMode});
    }

    return audioTracks;
}