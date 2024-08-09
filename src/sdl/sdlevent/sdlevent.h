#ifndef _SDLEVENT_H_
#define _SDLEVENT_H_

#include <SDL_events.h>

// This structure wraps an SDL_Event an adds additional data to it
class SDLEvent
{
public:
    SDLEvent (SDL_Event* event,
        int mouseTexturePositionX, int mouseTexturePositionY);
    SDL_Event* getSDL_Event () const;
    int mouseTexturePositionX () const;
    int mouseTexturePositionY () const;

private:
    SDL_Event* event_;
    int mouseTexturePositionX_;
    int mouseTexturePositionY_;
};

#endif // _SDLEVENT_H_