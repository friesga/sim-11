#include "datapanelbuilder.h"
#include "sdl/sdlpanel/sdlpanel.h"
#include "sdl/sdlnpositionswitch/sdlnpositionswitch.h"

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
    panelHeight_ = unitHeight * pixelsPerRackUnit ();

    // Calculated from the bottom of the texture, the top position of the
    // panel is located at the cabinet position (in rack units) multiplied
    // by the number of pixels per RU. Cabinet positions start at 0, hence
    // the addition by 1.
    panelPosition_.y = static_cast<int> (textureHeight (targetTexture_) -
        ((cabinetPosition_.height + 1) * pixelsPerRackUnit ()));
}

// The PanelBuilder::createFront function has an optional frame parameter.
// For a DataPanelBuilder the frame parameter is ignored and the frame
// is determined from the image metadata in the image container.
//
void DataPanelBuilder::createFront (string imageFile,
    Frame<float> frame)
{
    unique_ptr<Image> pngImage =
        imageContainer_.getImage (imageContainer_.getFileName (imageFile));

    unique_ptr<SDLTile> frontTile =
        make_unique<SDLTile> (*pngImage, *sdlRenderer_, targetTexture_, 
            placeFrameInTexture (getFrameFromImage (imageFile)));

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
    Button::MomentaryTwoPositionsState initialState, Button::EventCallback buttonClicked,
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

// The frame parameter is ignored as the frame is determined from the image
// metadata in the image container.
//
Button* DataPanelBuilder::createThreePositionSwitch (array<string, 3> positionImages,
    Button::MomentaryThreePositionsState initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    SDLNPositionSwitch<Button::MomentaryThreePositionsState>::PositionTiles positionTiles;

    for (auto imageName : positionImages)
    {
        unique_ptr<Image> pngImage =
            imageContainer_.getImage (imageContainer_.getFileName (imageName));

        positionTiles.emplace_back (make_unique<SDLTile> (*pngImage,
            *sdlRenderer_, targetTexture_,
            placeFrameInTexture (getFrameFromImage (imageName))));
    }

    buttons_.push_back (make_unique<SDLNPositionSwitch<Button::MomentaryThreePositionsState>> (move (positionTiles),
        initialState, switchClicked));

    // The tile pointers in the PositionsTiles vector are no longer
    // valid pointers to the SDLTile's but that's ok since the vector
    // is moved to SDLThreePositionSwitch and that function now has ownership
    // of the SDLTile's.

    return buttons_.back ().get ();
}

Button* DataPanelBuilder::createNPositionSwitch (vector<string> positionImages,
    Button::State initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    return nullptr;
}

// Determine the relative position and dimension of the given image layer
// in the image container.
// 
Frame<float> DataPanelBuilder::getFrameFromImage (string layerName)
{
    ImageContainer::LayerDescription metadata = 
        imageContainer_.getLayerMetadata (layerName);

    Image::Dimensions imageDimensions = 
        imageContainer_.imageDimensions ();

    return {static_cast<float> (metadata.position.x) / imageDimensions.width,
        static_cast<float> (metadata.position.y) / imageDimensions.height,
        static_cast<float> (metadata.dimensions.width) / imageDimensions.width,
        static_cast<float> (metadata.dimensions.height) / imageDimensions.height};
}

int DataPanelBuilder::textureHeight (SDLTexture& texture) const
{
    auto [textureWidth, textureHeight] = targetTexture_.dimensions ();
    return textureHeight;
}

// For now Panels are placed in a H9642 cabinet which has a height of 20RU.
// From this the height in pixels of one RU can be calculated and with
// that value the height of the panel in pixels.
//
int DataPanelBuilder::pixelsPerRackUnit () const
{
    static const RackUnit h9642Height {20_ru};
    return textureHeight (targetTexture_) / h9642Height;
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
    int y = panelPosition_.y + frame.y * panelHeight_;

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