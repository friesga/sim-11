#include "sdl/audio/sdlaudiomixer/sdlaudiomixer.h"
#include "sdl/audio/sdlaudiostream/sdlaudiostream.h"

#include <gtest/gtest.h>

// Definition of a test fixture to initialize SDL audio.
class PlaylistTest : public ::testing::Test
{
public:
    PlaylistTest ()
    {
        SDL_Init (SDL_INIT_AUDIO);
    }
};

TEST_F (PlaylistTest, trackCanBeAssignedTo)
{
    SDLAudioStream audioStream0 {{}};
    SDLAudioMixer::Playlist playlist {};

    playlist.assignTrack ({&audioStream0, PlaybackMode::Continuous});

    EXPECT_EQ (playlist.currentTrack ()->audioStream, &audioStream0);
}

TEST_F (PlaylistTest, assignTrackInitializesPlaylist)
{
    SDLAudioStream audioStream0 {{}};
    SDLAudioStream audioStream1 {{}};

    SDLAudioMixer::Playlist playlist {};

    playlist.addTrack ({&audioStream0, PlaybackMode::Continuous});
    playlist.assignTrack ({&audioStream1, PlaybackMode::Continuous});

    EXPECT_EQ (playlist.currentTrack ()->audioStream, &audioStream1);
}

TEST_F (PlaylistTest, currentTrackIsFirstInPlaylist)
{
    SDLAudioStream audioStream0 {{}};
    SDLAudioStream audioStream1 {{}};

    SDLAudioMixer::Playlist playlist {};

    playlist.addTrack ({&audioStream0, PlaybackMode::Continuous});
    playlist.addTrack ({&audioStream1, PlaybackMode::Continuous});

    EXPECT_EQ (playlist.currentTrack ()->audioStream, &audioStream0);
}

TEST_F (PlaylistTest, nextTrackSelectsSecondTrackInPlaylist)
{
    SDLAudioStream audioStream0 {{}};
    SDLAudioStream audioStream1 {{}};

    SDLAudioMixer::Playlist playlist {};

    playlist.addTrack ({&audioStream0, PlaybackMode::Continuous});
    playlist.addTrack ({&audioStream1, PlaybackMode::Continuous});
    playlist.nextTrack ();

    EXPECT_EQ (playlist.currentTrack ()->audioStream, &audioStream1);
}

TEST_F (PlaylistTest, nextTrackRemovesTrackFromPlsylist)
{
    SDLAudioStream audioStream0 {{}};
    SDLAudioStream audioStream1 {{}};

    SDLAudioMixer::Playlist playlist {};

    playlist.addTrack ({&audioStream0, PlaybackMode::Continuous});
    playlist.addTrack ({&audioStream1, PlaybackMode::Continuous});
    playlist.nextTrack ();
    playlist.nextTrack ();

    EXPECT_EQ (playlist.currentTrack (), nullptr);
}

TEST_F (PlaylistTest, playlistCanBeCleared)
{
    SDLAudioStream audioStream0 {{}};
    SDLAudioStream audioStream1 {{}};

    SDLAudioMixer::Playlist playlist {};

    playlist.addTrack ({&audioStream0, PlaybackMode::Continuous});
    playlist.addTrack ({&audioStream1, PlaybackMode::Continuous});
    playlist.clear ();

    EXPECT_EQ (playlist.currentTrack (), nullptr);
}