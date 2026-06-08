#include "datapanelbuilder.h"
#include "sdl/sdlpanel/sdlpanel.h"
#include "sdl/sdlmultipositionswitch/sdlmultipositionswitch.h"
#include "overloaded.h"

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
    unique_ptr<Image> pngImage =
        imageContainer_.getImage (imageContainer_.getFileName (indicatorOffImage));

    unique_ptr<SDLTile> indicatorOffTile =
        make_unique<SDLTile> (*pngImage, *sdlRenderer_, targetTexture_,
            placeFrameInTexture (getFrameFromImage (indicatorOffImage)));

    pngImage =
        imageContainer_.getImage (imageContainer_.getFileName (indicatorOnImage));

    unique_ptr<SDLTile> indicatorOnTile =
        make_unique<SDLTile> (*pngImage, *sdlRenderer_, targetTexture_,
            placeFrameInTexture (getFrameFromImage (indicatorOnImage)));

    indicators_.push_back (make_unique<SDLIndicator> (move (indicatorOffTile),
        move (indicatorOnTile), showFigure));

    // indicatorOffTile and indicatorOnTile are no longer valid pointers
    // to the SDLTile's but that's ok since they are moved and SDLIndicator
    // now has ownership of the SDLTile's.

    return indicators_.back ().get ();
}


IndicatorButton* DataPanelBuilder::createIndicatorLatchingButton (Button::ImageNames const& imageNames,
    Button::TwoPositionsState initialState,
    Button::EventCallback buttonClicked, Indicator::State showIndicator,
    Frame<float> frame)
{
    SDLIndicatorLatchingButton::TileGrid tiles;

    tiles[to_integral (Button::TwoPositionsState::Up)][to_integral (Indicator::State::Off)] =
        createTile (imageNames.buttonUpIndicatorOff);

    tiles[to_integral (Button::TwoPositionsState::Down)][to_integral (Indicator::State::Off)] =
        createTile (imageNames.buttonDownIndicatorOff);

    tiles[to_integral (Button::TwoPositionsState::Up)][to_integral (Indicator::State::On)] =
        createTile (imageNames.buttonUpIndicatorOn);

    tiles[to_integral (Button::TwoPositionsState::Down)][to_integral (Indicator::State::On)] =
        createTile (imageNames.buttonDownIndicatorOn);

    indicatorButtons_.push_back (make_unique<SDLIndicatorLatchingButton> (move (tiles),
        initialState, buttonClicked, showIndicator));

    // The tile pointers in the TileGrid array are no longer valid pointers
    // to the SDLTile's but that's ok since the array is moved to 
    // SDLIndicatorLatchingButton and that function now has ownership of the
    // SDLTile's.

    return indicatorButtons_.back ().get ();
}

// This function create a MultiPositionSwitch button with the given
// position images, initial state and callback function, pushes it to the
// buttons_ vector and returns a raw pointer to the created button.
// 
// This function is partly doubled with the one in FilePanelBuilder. This is
// acceptable as FilePanelBuilder is deprecated and will be removed.
//
Button* DataPanelBuilder::createMultiPositionSwitch (vector<string> positionImages,
    Button::State initialState,
    Button::EventCallback switchClicked,
    Frame<float> frame)
{
    vector<unique_ptr<SDLTile>> positionTiles = createTiles (positionImages);

    buttons_.push_back (createButton (move (positionTiles), initialState,
        switchClicked));

    return buttons_.back ().get ();
}

// This function creates a vector of SDLTiles from the given image names by
// looking up the image name in the image container and loading the image
// with the retrieved file name.
//
vector<unique_ptr<SDLTile>> DataPanelBuilder::createTiles (vector<string> imageNames)
{
    vector<unique_ptr<SDLTile>> positionTiles;

    for (auto imageName : imageNames)
    {
        unique_ptr<SDLTile> switchPositionTile = createTile (imageName);

        positionTiles.emplace_back (move (switchPositionTile));
    }

    return positionTiles;
}

// This function creates a unique pointer to an SDLTile from the given
// image name by looking up the image name in the image container, 
// loading the image with the retrieved file name and creating an SDLTile
// from the loaded image.
unique_ptr<SDLTile> DataPanelBuilder::createTile (string imageName)
{ 
    unique_ptr<Image> pngImage =
        imageContainer_.getImage (imageContainer_.getFileName (imageName));

    return make_unique<SDLTile> (*pngImage, *sdlRenderer_, targetTexture_,
        placeFrameInTexture (getFrameFromImage (imageName)));
}


// This function creates a unique pointer to a Button object of the correct
// type based on the type of the initialState parameter. The function uses
// an overloaded visitor to determine the type of the initialState parameter.
// 
// Note the explcicit return type of the lambdas in the visitor to make the
// return type equal for all lambda's as required by std::visit.
//
unique_ptr<Button> DataPanelBuilder::createButton (vector<unique_ptr<SDLTile>> positionTiles,
    Button::State initialState, Button::EventCallback switchClicked)
{
    auto switchVisitor = overloaded
    {
        [&] (Button::TwoPositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::TwoPositionsState>> (move (positionTiles),
                initialState, switchClicked);
        },

        [&] (Button::MomentaryUpTwoPositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::MomentaryUpTwoPositionsState>> (move (positionTiles),
                initialState, switchClicked);
        },

        [&] (Button::MomentaryDownTwoPositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::MomentaryDownTwoPositionsState>> (move (positionTiles),
                initialState, switchClicked);
        },

        [&] (Button::CenteredTwoPositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::CenteredTwoPositionsState>> (move (positionTiles),
                initialState, switchClicked);
        },

        [&] (Button::ThreePositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::ThreePositionsState>> (move (positionTiles),
                initialState, switchClicked);
        },

        [&] (Button::MomentaryThreePositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::MomentaryThreePositionsState>> (move (positionTiles),
                initialState, switchClicked);
        },

        [&] (Button::FourPositionsState initialState) -> unique_ptr<Button>
        {
            return make_unique<SDLMultiPositionSwitch<Button::FourPositionsState>> (move (positionTiles),
                initialState, switchClicked);
        }
    };

    return visit (switchVisitor, initialState);
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

int DataPanelBuilder::textureWidth (SDLTexture& texture) const
{
    auto [textureWidth, textureHeight] = targetTexture_.dimensions ();
    return textureWidth;
}

int DataPanelBuilder::textureHeight (SDLTexture& texture) const
{
    auto [textureWidth, textureHeight] = targetTexture_.dimensions ();
    return textureHeight;
}

// The height of a texture is determined by the number of pixels per rack
// unit. As the width of the devices, and thus the width of the textures is
// fixed (19"), the number of pixels per rack unit has to be calculated keeping
// the aspect ratio of the original image intact. As one rack unit is 1.75"
// and the width of the device is 19", the height of a rack unit in pixels can
// be calculated as (1.75/19) times the width of the texture in pixels.
// This calculation ensures that the devices are rendered with the correct
// proportions on the panel.
//
int DataPanelBuilder::pixelsPerRackUnit () const
{
    return textureWidth (targetTexture_) * (1.75 / 19);
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