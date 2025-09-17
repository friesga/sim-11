#include "filepanelbuilder.h"

#include "sdl/sdlfront/sdlfront.h"

#include <string>
#include <memory>

using std::string;
using std::make_unique;
using std::make_pair;

FilePanelBuilder::FilePanelBuilder (unique_ptr<SDLRenderer>& sdlRenderer,
    SDL_Texture* texture, Cabinet::Position cabinetPosition, RackUnit unitHeight)
    :
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture},
    cabinetPosition_ {cabinetPosition}
{
    static const RackUnit h9642Height {20_ru};
    auto [textureWidth, textureHeight] = getTextureDimensions (targetTexture_);
    pixelsPerRackUnit_ = textureHeight / h9642Height;
    panelHeight_ = pixelsPerRackUnit_ * unitHeight;
}

void FilePanelBuilder::createFront (string imageFile,
    Frame<float> frame)
{
    unique_ptr<SDLTexture> frontTexture =
        make_unique<SDLTexture> (imageFile, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    fronts_.push_back (make_unique<SDLFront> (move (frontTexture)));

    // frontTexture no longer is a valid pointer to the SDLTexture but
    // that's ok since it's moved and SDLFront now has ownership of the
    // SDLTexture.
}

Indicator* FilePanelBuilder::createIndicator (string indicatorOffImage,
    string indicatorOnImage, Indicator::State showFigure,
    Frame<float> frame)
{
    unique_ptr<SDLTexture> indicatorOffTexture =
        make_unique<SDLTexture> (indicatorOffImage, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    unique_ptr<SDLTexture> indicatorOnTexture =
        make_unique<SDLTexture> (indicatorOnImage, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    indicators_.push_back (make_unique<SDLIndicator> (move (indicatorOffTexture),
        move (indicatorOnTexture), showFigure));

    // indicatorOffTexture and indicatorOnTexture are no longer valid pointers
    // to the SDLTexture's but that's ok since they are moved and SDLIndicator
    // now has ownership of the SDLTexture's.

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
    unique_ptr<SDLTexture> buttonDownTexture =
        make_unique<SDLTexture> (buttonDownImage, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    unique_ptr<SDLTexture> buttonUpTexture =
        make_unique<SDLTexture> (buttonUpImage, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    buttons_.push_back (make_unique<SDLLatchingButton> (move (buttonDownTexture),
        move (buttonUpTexture), initialState, buttonClicked));

    // buttonDownTexture and buttonUpTexture are no longer valid pointers
    // to the SDLTexture's but that's ok since they are moved and
    // SDLLatchingButton now has ownership of the SDLTexture's.

    return buttons_.back ().get ();
}

Button* FilePanelBuilder::createMomentaryButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
    Frame<float> frame)
{
    unique_ptr<SDLTexture> buttonDownTexture =
        make_unique<SDLTexture> (buttonDownImage, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    unique_ptr<SDLTexture> buttonUpTexture =
        make_unique<SDLTexture> (buttonUpImage, sdlRenderer_->getSDL_Renderer (),
            targetTexture_, placeFrameInTexture (frame));

    buttons_.push_back (make_unique<SDLMomentaryButton> (move (buttonDownTexture),
        move (buttonUpTexture), initialState, buttonClicked));

    // buttonDownTexture and buttonUpTexture are no longer valid pointers
    // to the SDLTexture's but that's ok since they are moved and
    // SDLMomentaryButton now has ownership of the SDLTexture's.

    return buttons_.back ().get ();
}

IndicatorButton* FilePanelBuilder::createSDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
    Button::TwoPositionsState initialState,
    Button::EventCallback buttonClicked, Indicator::State showIndicator,
    Frame<float> frame)
{
    SDLIndicatorLatchingButton::TextureGrid textures;

    textures[to_integral (Button::TwoPositionsState::Off)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTexture> (imageNames.buttonUpIndicatorOff,
            sdlRenderer_->getSDL_Renderer (), targetTexture_,
            placeFrameInTexture (frame));

    textures[to_integral (Button::TwoPositionsState::On)][to_integral (Indicator::State::Off)] =
        make_unique<SDLTexture> (imageNames.buttonDownIndicatorOff,
            sdlRenderer_->getSDL_Renderer (), targetTexture_,
            placeFrameInTexture (frame));

    textures[to_integral (Button::TwoPositionsState::Off)][to_integral (Indicator::State::On)] =
        make_unique<SDLTexture> (imageNames.buttonUpIndicatorOn,
            sdlRenderer_->getSDL_Renderer (), targetTexture_,
            placeFrameInTexture (frame));

    textures[to_integral (Button::TwoPositionsState::On)][to_integral (Indicator::State::On)] =
        make_unique<SDLTexture> (imageNames.buttonDownIndicatorOn,
            sdlRenderer_->getSDL_Renderer (), targetTexture_,
            placeFrameInTexture (frame));

    indicatorButtons_.push_back (make_unique<SDLIndicatorLatchingButton> (move (textures),
        initialState, buttonClicked, showIndicator));

    // The texture pointers in the TextureGrid array are no longer valid pointers
    // to the SDLTexture's but that's ok since the array is moved to 
    // SDLIndicatorLatchingButton and that function now has ownership of the
    // SDLTexture's.

    return indicatorButtons_.back ().get ();
}

Button* FilePanelBuilder::createFourPositionSwitch (array<string, 4> positionImages,
    Button::FourPositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    SDLFourPositionSwitch::PositionTextures positionTextures;

    for (auto imageName : positionImages)
    {
        positionTextures.emplace_back (make_unique<SDLTexture> (imageName,
            sdlRenderer_->getSDL_Renderer (), targetTexture_,
            placeFrameInTexture (frame)));
    }

    buttons_.push_back (make_unique<SDLFourPositionSwitch> (move (positionTextures),
        initialState, switchClicked));

    // The texture pointers in the PositionsTextures vector are no longer
    // valid pointers to the SDLTexture's but that's ok since the vector
    // is moved to SDLFourPositionSwitch and that function now has ownership
    // of the SDLTexture's.

    return buttons_.back ().get ();
}

Button* FilePanelBuilder::createThreePositionSwitch (array<string, 3> positionImages,
    Button::ThreePositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    SDLThreePositionSwitch::PositionTextures positionTextures;

    for (auto imageName : positionImages)
    {
        positionTextures.emplace_back (make_unique<SDLTexture> (imageName,
            sdlRenderer_->getSDL_Renderer (), targetTexture_,
            placeFrameInTexture (frame)));
    }

    buttons_.push_back (make_unique<SDLThreePositionSwitch> (move (positionTextures),
        initialState, switchClicked));

    // The texture pointers in the PositionsTextures vector are no longer
    // valid pointers to the SDLTexture's but that's ok since the vector
    // is moved to SDLThreePositionSwitch and that function now has ownership
    // of the SDLTexture's.

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
    auto [textureWidth, textureHeight] = getTextureDimensions (targetTexture_);
    int x = static_cast<int> (frame.x * textureWidth);
    int y = static_cast<int> (textureHeight -
        ((cabinetPosition_.height + 1) * pixelsPerRackUnit_) +
        (frame.y * panelHeight_));
    int width_ = static_cast<int> (frame.width * textureWidth);
    int height_ = static_cast<int> (frame.height * panelHeight_);
    return {x, y, width_, height_};
}

pair<int, int> FilePanelBuilder::getTextureDimensions (SDL_Texture* texture)
{
    Uint32 format;
    int access;
    int width, height;

    SDL_QueryTexture (texture, &format, &access, &width, &height);
    return make_pair (width, height);
}

PanelComposition FilePanelBuilder::getPanelComposition ()
{
    return PanelComposition
    {
        move (fronts_),
        move (indicators_),
        move (buttons_),
        move (indicatorButtons_)
    };
}