#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../renderer/renderer.h"

#include <SDL.h>
#include <string>

//Texture wrapper class
// A SDL_Texture is a structure that contains an driver-specific
// representation of pixel data to be rendered by the GPU (by means
// of a renderer).
// The Texture class wrapsan SDL_Texture struct plus corresponding
// functions.
class Texture
{
    //The actual hardware texture
    SDL_Texture* sdlTtexture_;

    // The renderer for this texture
    Renderer &renderer_;

    //Image dimensions
    int width_;
    int height_;

public:
    // Constructor
    Texture (Renderer &renderer, std::string path);

    // Destructor
    ~Texture ();

    //Load image at specified path
    bool loadFromFile (std::string path);

    //Render texture at given point
    void render (int x, int y);
    void render (int x, int y, int width, int height);

    //Get image dimensions
    int getWidth ();
    int getHeight ();
};

#endif // _TEXTURE_H_