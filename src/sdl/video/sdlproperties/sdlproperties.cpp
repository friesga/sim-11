#include "sdlproperties.h"


SDLProperties::SDLProperties (const char* title, int x, int y,
    int width, int height, Window flag)
{
    properties_ = SDL_CreateProperties ();
    SDL_SetStringProperty (properties_, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
    SDL_SetNumberProperty (properties_, SDL_PROP_WINDOW_CREATE_X_NUMBER, x);
    SDL_SetNumberProperty (properties_, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y);
    SDL_SetNumberProperty (properties_, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
    SDL_SetNumberProperty (properties_, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
    
    if (flag == Window::Hidden)
        SDL_SetBooleanProperty (properties_, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER,
            SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN);
}

SDLProperties::~SDLProperties ()
{
    SDL_DestroyProperties (properties_);
}

SDLProperties::operator SDL_PropertiesID () const
{
    return properties_;
}