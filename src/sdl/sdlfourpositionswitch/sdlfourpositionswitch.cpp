#include "sdlfourpositionswitch.h"

using std::make_unique;
using std::get;

SDLFourPositionSwitch::SDLFourPositionSwitch (array<string, 4> positionImageNames,
    Button::FourPositionsState initialState,
    unique_ptr<SDLRenderer>& sdlRenderer,
    EventCallback switchClicked,
    SDL_Texture* targetTexture,
    Frame<int> frame)
    :
    switchPosition_ {initialState},
    sdlRenderer_ {sdlRenderer},
    switchClicked_ {switchClicked},
    frame_ {frame}
{
    for (auto imageName : positionImageNames)
    {
        positionTextures_.emplace_back (make_unique<SDLTexture> (imageName,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame));
    }
}

void SDLFourPositionSwitch::setState (State newState)
{
    switchPosition_ = get<Button::FourPositionsState> (newState);
}

void SDLFourPositionSwitch::render ()
{
    // positionTextures_[static_cast<int> (state_)]->render ();
    positionTextures_[+switchPosition_]->render ();
}

void SDLFourPositionSwitch::handleEvent (InputEvent const* event)
{
    if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition (), 0.75f))
    {
        switchPosition_ = nextPosition (switchPosition_);
        switchClicked_ (switchPosition_);
    }
}

bool SDLFourPositionSwitch::isWithinBounds (Position position, float margin) const
{ 
    return positionTextures_[+switchPosition_]->isWithinBounds (position, margin);
}

Button::FourPositionsState SDLFourPositionSwitch::nextPosition (Button::FourPositionsState position)
{
    // return static_cast<Button::FourPositionsState> (static_cast<int> (position) + 1);
    if (switchPosition_ != Button::FourPositionsState::P3)
        return static_cast<Button::FourPositionsState> ((+position) + 1);

    return Button::FourPositionsState::P3;
}