#include "datapanelbuilder.h"
#include "sdl/sdlpanel/sdlpanel.h"

DataPanelBuilder::DataPanelBuilder (ImageContainer& imageContainer,
    unique_ptr<SDLRenderer>& sdlRenderer, SDLTexture& texture,
    Cabinet::Position cabinetPosition, RackUnit unitHeight)
    :
    imageContainer_ {imageContainer},
    sdlRenderer_ {sdlRenderer},
    targetTexture_ {texture},
    cabinetPosition_ {cabinetPosition},
    unitHeight_ {unitHeight}
{
}

void DataPanelBuilder::createFront (string imageFile,
    Frame<float> frame)
{
    unique_ptr<Image> pngImage =
        imageContainer_.getImage (imageContainer_.getFileName (imageFile));

    unique_ptr<SDLTile> frontTile =
        make_unique<SDLTile> (*pngImage, *sdlRenderer_,
            targetTexture_, placeFrameInTexture (frame));

    fronts_.push_back (make_unique<SDLFront> (move (frontTile)));

    // frontTile no longer is a valid pointer to the SDLTile but
    // that's ok since it's moved and SDLFront now has ownership of the
    // SDLTile.
}

Indicator* DataPanelBuilder::createIndicator (string indicatorOffImage,
    string indicatorOnImage, Indicator::State showFigure, Frame<float> frame)
{
    // To be implemented

    return indicators_.back ().get ();
}

// Add a Button to the Panel, returning a pointer to the added Button.
//
// It's the user's responsibility not to use this pointer when the Window
// the Panel and Button belong to is destroyed. Not following this instruction
// will lead to undefined behaviour.
//
Button* DataPanelBuilder::createLatchingButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
    Frame<float> frame)
{
    // To be implemented

    return buttons_.back ().get ();
}

Button* DataPanelBuilder::createMomentaryButton (string buttonDownImage, string buttonUpImage,
    Button::TwoPositionsState initialState, Button::EventCallback buttonClicked,
    Frame<float> frame)
{
    // To be implemented.

    return buttons_.back ().get ();
}

IndicatorButton* DataPanelBuilder::createSDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
    Button::TwoPositionsState initialState,
    Button::EventCallback buttonClicked, Indicator::State showIndicator,
    Frame<float> frame)
{
    // To be implemented

    return indicatorButtons_.back ().get ();
}

Button* DataPanelBuilder::createFourPositionSwitch (array<string, 4> positionImages,
    Button::FourPositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    // To be implemented

    return buttons_.back ().get ();
}

Button* DataPanelBuilder::createThreePositionSwitch (array<string, 3> positionImages,
    Button::ThreePositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    // To be implemented

    return buttons_.back ().get ();
}

Frame<int> DataPanelBuilder::getFrameFromImage (string layerName)
{
    ImageContainer::Layer metadata = 
        imageContainer_.getLayerMetadata (layerName);

    return {metadata.position.x, metadata.position.y,
        metadata.dimensions.width, metadata.dimensions.height};
}

// Place the given frame, whith positions and dimensions relative to the
// panel, in the target texture. The given panel frame has position and
// dimensions as fractions relative to the panel dimensions; the target
// texture frame has absolute values.
Frame<int> DataPanelBuilder::placeFrameInTexture (Frame<float> frame)
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

unique_ptr<Panel> DataPanelBuilder::getPanel ()
{
    return make_unique<SDLPanel> (sdlRenderer_,
        targetTexture_, cabinetPosition_, unitHeight_,
        PanelComposition {move (fronts_), move (indicators_),
        move (buttons_), move (indicatorButtons_)});
}