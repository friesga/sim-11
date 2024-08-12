#ifndef _SDLEVENT_H_
#define _SDLEVENT_H_

#include <panel.h>

#include <SDL_events.h>

// This structure wraps an SDL_Event an adds additional data to it
class SDLEvent
{
public:
    SDLEvent (SDL_Event* event, Position mouseTexturePosition);
    SDL_Event* getSDL_Event () const;
    Position mouseTexturePosition () const;

private:
    SDL_Event* event_;
    Position mouseTexturePosition_;
};

#endif // _SDLEVENT_H_