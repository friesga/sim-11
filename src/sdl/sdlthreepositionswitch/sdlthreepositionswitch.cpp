#include "sdlthreepositionswitch.h"

using std::get;

SDLThreePositionSwitch::SDLThreePositionSwitch (array<string, 3> positionImageNames,
    Button::ThreePositionsState initialState,
    unique_ptr<SDLRenderer>& sdlRenderer,
    EventCallback switchClicked,
    SDL_Texture* targetTexture,
    Frame<int> frame)
    :
    switchClicked_ {switchClicked},
    frame_ {frame},
    switchPosition_ {initialState}
{
    for (auto imageName : positionImageNames)
    {
        positionTextures_.emplace_back (make_unique<SDLTexture> (imageName,
            sdlRenderer->getSDL_Renderer (), targetTexture, frame));
    }
}

// Functions required for the Button interface
void SDLThreePositionSwitch::setState (State newState)
{
    switchPosition_ = get<Button::ThreePositionsState> (newState);
}

void SDLThreePositionSwitch::render ()
{
    positionTextures_[+switchPosition_]->render ();
}

void SDLThreePositionSwitch::handleEvent (InputEvent const* event)
{
    if (event->type () == InputEvent::Type::MouseButtonDown &&
        event->button () == InputEvent::Button::Left &&
        isWithinBounds (event->mousePosition (), 0.75f))
    {
        if (isRightOfCenter (event->mousePosition (), 0.75f))
        {
            switchPosition_ = nextPosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }

        if (isLeftOfCenter (event->mousePosition (), 0.75f))
        {
            switchPosition_ = previousPosition (switchPosition_);
            switchClicked_ (switchPosition_);
            return;
        }
    }

    if (event->type () == InputEvent::Type::MouseButtonUp &&
        event->button () == InputEvent::Button::Left &&
        switchPosition_ == Button::ThreePositionsState::Right)
    {
        switchPosition_ = previousPosition (switchPosition_);
        switchClicked_ (switchPosition_);
    }
}

bool SDLThreePositionSwitch::isWithinBounds (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isWithinBounds (position, margin);
}

bool SDLThreePositionSwitch::isRightOfCenter (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isRightOfCenter (position, margin);
}

bool SDLThreePositionSwitch::isLeftOfCenter (Position position, float margin) const
{
    return positionTextures_[+switchPosition_]->isLeftOfCenter (position, margin);
}

Button::ThreePositionsState SDLThreePositionSwitch::nextPosition (Button::ThreePositionsState position)
{
    if (switchPosition_ != Button::ThreePositionsState::Right)
        return static_cast<Button::ThreePositionsState> ((+position) + 1);

    return Button::ThreePositionsState::Right;
}

Button::ThreePositionsState SDLThreePositionSwitch::previousPosition (Button::ThreePositionsState position)
{
    if (switchPosition_ != Button::ThreePositionsState::Left)
        return static_cast<Button::ThreePositionsState> ((+position) - 1);

    return Button::ThreePositionsState::Left;
}
