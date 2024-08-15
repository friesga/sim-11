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
        State showIndicator, SDL_Texture* targetTexture,
        Panel::Frame<float> boundingBox);
    ~SDLIndicator ();
    void render ();

    // Definition of functions required for the Indicator interface
    void show (Indicator::State showIndicator) override;

private:
    // The texture to use for this indicator
    unique_ptr<SDLTexture> sdlTtexture_;

    // Indication whether or not the indicator has to be shown
    Indicator::State showIndicator_;
};

#endif // _SDLINDICATOR_H_