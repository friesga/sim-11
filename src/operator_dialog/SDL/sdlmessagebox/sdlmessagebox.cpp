#include "sdlmessagebox.h"

#include <SDL3/SDL.h>
#include <string>
#include <stdexcept>
#include <iostream>

using std::string;
using std::runtime_error;

void SDLMessageBox::show (string message)
{
    const Uint32 NO_DIALOG_TYPE {0};

    if (!SDL_ShowSimpleMessageBox (NO_DIALOG_TYPE, "sim-11",
            message.c_str (), NULL))
    {
        throw runtime_error (string ("SDL_ShowSimpleMessageBox: ") + SDL_GetError ());
    }
}