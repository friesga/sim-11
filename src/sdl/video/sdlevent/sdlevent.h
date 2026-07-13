#ifndef _SDLEVENT_H_
#define _SDLEVENT_H_

#include <panel.h>

#include <SDL_events.h>

// This structure wraps an SDL_Event an adds additional data to it
class SDLEvent : public InputEvent
{
public:
    SDLEvent (SDL_Event* event, Position mouseTexturePosition);

    // Functions required for the Event interface
    InputEvent::Type type () const;
    InputEvent::Button button () const;
    Position mousePosition () const;

private:
    SDL_Event* event_;
    Position mouseTexturePosition_;
};

#endif // _SDLEVENT_H_