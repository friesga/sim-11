#include "filepanelbuilder.h"
#include "sdl/sdlpanel/sdlpanel.h"

#include <algorithm>
#include <utility>

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;
using std::move;
using std::string;
using std::ranges::find_if;
using std::pair;
using std::make_pair;

FilePanelBuilder::FilePanelBuilder (unique_ptr<SDLRenderer>& sdlRenderer,
    SDLTexture& texture, Cabinet::Position cabinetPosition, RackUnit unitHeight)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture},
    cabinetPosition_ {cabinetPosition},
    unitHeight_ {unitHeight}
{
    static const RackUnit h9642Height {20_ru};
    auto [textureWidth, textureHeight] = targetTexture_.dimensions ();
    pixelsPerRackUnit_ = textureHeight / h9642Height;
    panelHeight_ = pixelsPerRackUnit_ * unitHeight;
}

void FilePanelBuilder::createFront (string imageFile,
    Frame<float> frame)
{
    unique_ptr<SDLTile> frontTile =
        make_unique<SDLTile> (imageFile, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    fronts_.push_back (make_unique<SDLFront> (move (frontTile)));

    // frontTile no longer is a valid pointer to the SDLTile but
    // that's ok since it's moved and SDLFront now has ownership of the
    // SDLTile.
}

Indicator* FilePanelBuilder::createIndicator (string indicatorOffImage,
    string indicatorOnImage, Indicator::State showFigure, Frame<float> frame)
{
    unique_ptr<SDLTile> indicatorOffTile =
        make_unique<SDLTile> (indicatorOffImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    unique_ptr<SDLTile> indicatorOnTile =
        make_unique<SDLTile> (indicatorOnImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    indicators_.push_back (make_unique<SDLIndicator> (move (indicatorOffTile),
        move (indicatorOnTile), showFigure));

    // indicatorOffTile and indicatorOnTile are no longer valid pointers
    // to the SDLTile's but that's ok since they are moved and SDLIndicator
    // now has ownership of the SDLTile's.

    return indicators_.back ().get ();
}

// Add a Button to the Panel, returning a pointer to the added Button.
//
// It's the user's responsibility not to use this pointer when the Window
// the Panel and Button belong to is destroyed. Not following this instruction
// will lead to undefined behaviour.
//
Button* FilePanelBuilder::createLatchingButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
    Frame<float> frame)
{
    unique_ptr<SDLTile> buttonDownTile =
        make_unique<SDLTile> (buttonDownImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    unique_ptr<SDLTile> buttonUpTile =
        make_unique<SDLTile> (buttonUpImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    buttons_.push_back (make_unique<SDLLatchingButton> (move (buttonDownTile),
        move (buttonUpTile), initialState, buttonClicked));

    // buttonDownTile and buttonUpTile are no longer valid pointers
    // to the SDLTile's but that's ok since they are moved and
    // SDLLatchingButton now has ownership of the SDLTile's.

    return buttons_.back ().get ();
}

Button* FilePanelBuilder::createMomentaryButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
    Frame<float> frame)
{
    unique_ptr<SDLTile> buttonDownTile =
        make_unique<SDLTile> (buttonDownImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    unique_ptr<SDLTile> buttonUpTile =
        make_unique<SDLTile> (buttonUpImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    buttons_.push_back (make_unique<SDLMomentaryButton> (move (buttonDownTile),
        move (buttonUpTile), initialState, buttonClicked));

    // buttonDownTile and buttonUpTile are no longer valid pointers
    // to the SDLTile's but that's ok since they are moved and
    // SDLMomentaryButton now has ownership of the SDLTile's.

    return buttons_.back ().get ();
}

IndicatorButton* FilePanelBuilder::createSDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
    Button::TwoPositionsState initialState,
    Button::EventCallback buttonClicked, Indicator::State showIndicator,
    Frame<float> frame)
{
    SDLIndicatorLatchingButton::TileGrid tiles;

    tiles[to_integral (Button::TwoPositionsState::Off)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTile> (imageNames.buttonUpIndicatorOff,
            *sdlRenderer_, targetTexture_, placeFrameInTexture (frame));

    tiles[to_integral (Button::TwoPositionsState::On)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTile> (imageNames.buttonDownIndicatorOff,
            *sdlRenderer_, targetTexture_, placeFrameInTexture (frame));

    tiles[to_integral (Button::TwoPositionsState::Off)][to_integral (Indicator::State::On)] =
        make_unique<SDLTile> (imageNames.buttonUpIndicatorOn,
            *sdlRenderer_, targetTexture_, placeFrameInTexture (frame));

    tiles[to_integral (Button::TwoPositionsState::On)][to_integral (Indicator::State::On)] =
        make_unique<SDLTile> (imageNames.buttonDownIndicatorOn,
            *sdlRenderer_, targetTexture_, placeFrameInTexture (frame));

    indicatorButtons_.push_back (make_unique<SDLIndicatorLatchingButton> (move (tiles),
        initialState, buttonClicked, showIndicator));

    // The tile pointers in the TileGrid array are no longer valid pointers
    // to the SDLTile's but that's ok since the array is moved to 
    // SDLIndicatorLatchingButton and that function now has ownership of the
    // SDLTile's.

    return indicatorButtons_.back ().get ();
}

Button* FilePanelBuilder::createFourPositionSwitch (array<string, 4> positionImages,
    Button::FourPositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    SDLFourPositionSwitch::PositionTiles positionTiles;

    for (auto imageName : positionImages)
    {
        positionTiles.emplace_back (make_unique<SDLTile> (imageName,
            *sdlRenderer_, targetTexture_, placeFrameInTexture (frame)));
    }

    buttons_.push_back (make_unique<SDLFourPositionSwitch> (move (positionTiles),
        initialState, switchClicked));

    // The tile pointers in the PositionsTiles vector are no longer
    // valid pointers to the SDLTile's but that's ok since the vector
    // is moved to SDLFourPositionSwitch and that function now has ownership
    // of the SDLTile's.

    return buttons_.back ().get ();
}

Button* FilePanelBuilder::createThreePositionSwitch (array<string, 3> positionImages,
    Button::ThreePositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    SDLThreePositionSwitch::PositionTiles positionTiles;

    for (auto imageName : positionImages)
    {
        positionTiles.emplace_back (make_unique<SDLTile> (imageName,
            *sdlRenderer_, targetTexture_, placeFrameInTexture (frame)));
    }

    buttons_.push_back (make_unique<SDLThreePositionSwitch> (move (positionTiles),
        initialState, switchClicked));

    // The tile pointers in the PositionsTiles vector are no longer
    // valid pointers to the SDLTile's but that's ok since the vector
    // is moved to SDLThreePositionSwitch and that function now has ownership
    // of the SDLTile's.

    return buttons_.back ().get ();
}

// Place the given frame, whith positions and dimensions relative to the
// panel, in the target texture. The given panel frame has position and
// dimensions as fractions relative to the panel dimensions; the target
// texture frame has absolute values.
Frame<int> FilePanelBuilder::placeFrameInTexture (Frame<float> frame)
{
    // The passed frame contains positions relative to the target
    // texture. Calculate the frame's position and dimensions in pixels
    // from these relative values.
    // The cabinet's height is a rack unit, starting from zero, hence
    // the addition by one.
    auto [textureWidth, textureHeight] = targetTexture_.dimensions ();
    int x = static_cast<int> (frame.x * textureWidth);
    int y = static_cast<int> (textureHeight -
        ((cabinetPosition_.height + 1) * pixelsPerRackUnit_) +
        (frame.y * panelHeight_));
    int width_ = static_cast<int> (frame.width * textureWidth);
    int height_ = static_cast<int> (frame.height * panelHeight_);
    return {x, y, width_, height_};
}

unique_ptr<Panel> FilePanelBuilder::getPanel ()
{
    return make_unique<SDLPanel> (sdlRenderer_,
        targetTexture_, cabinetPosition_, unitHeight_,
        PanelComposition {move (fronts_), move (indicators_),
        move (buttons_), move (indicatorButtons_)});
}
