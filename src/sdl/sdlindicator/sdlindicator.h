#ifndef _SDLINDICATOR_H_
#define _SDLINDICATOR_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <SDL.h>

using std::string;
using std::unique_ptr;

class SDLIndicator : public Indicator
{
public:
    SDLIndicator (string imageFile, unique_ptr<SDLRenderer> &sdlRenderer,
        State showIndicator, int x, int y, int width, int height);
    ~SDLIndicator ();
    void render ();
    void render (SDL_Texture* texture);

    // Definition of functions required for the Indicator interface
    void show (Indicator::State showIndicator) override;

private:
    // The texture to use for this indicator
    unique_ptr<SDLTexture> sdlTtexture_;

    // Indication whether or not the indicator has to be shown
    Indicator::State showIndicator_;

protected:
    // Image positon and dimensions
    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // _SDLINDICATOR_H_