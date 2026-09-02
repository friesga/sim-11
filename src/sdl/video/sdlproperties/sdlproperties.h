#ifndef _SDL_PROPERTIES_H_
#define _SDL_PROPERTIES_H_

#include <SDL3/SDL.h>

// This class is wrapper around SDL_Properties. It takes the properties 
// we need in the constructor and can return an SDL_PropertiesID to be used
// in the SDL_CreateWindowWithProperties function. The destructor will free
// the SDL_PropertiesID when it is no longer needed.
//
class SDLProperties
{
public:
    enum class Window
    {
        Hidden,
        Shown
    };

    SDLProperties (const char* title, int x, int y,
        int width, int height, Window flag);

    // Disallow copy and assignment to avoid double use and free of
    // SDL_PropertiesID.
    SDLProperties (const SDLProperties&) = delete;
    SDLProperties& operator=(const SDLProperties&) = delete;
    ~SDLProperties ();
    operator SDL_PropertiesID () const;

private:
    SDL_PropertiesID properties_;
};


#endif // _SDL_PROPERTIES_H_