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
    SDLIndicator (unique_ptr<SDLTexture> indicatorOffTexture,
        unique_ptr<SDLTexture> indicatorOnTexture, State showIndicator);
    ~SDLIndicator ();

    // Definition of functions required for the Indicator interface
    void show (Indicator::State showIndicator) override;
    void render (GraphicsContext& context) override;

    bool isWithinBounds (Position position, float margin) const override;

private:
    // The textures to use for this indicator
    unique_ptr<SDLTexture> indicatorOnTexture_;
    unique_ptr<SDLTexture> indicatorOffTexture_;

    // Indication whether or not the indicator has to be shown
    Indicator::State showIndicator_;
};

#endif // _SDLINDICATOR_H_