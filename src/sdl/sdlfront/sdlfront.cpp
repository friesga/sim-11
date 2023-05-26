#include "sdlfront.h"

#include <SDL_image.h>

using std::make_unique;

SDLFront::SDLFront (string imageFile, unique_ptr<SDLRenderer> &sdlRenderer,
    int x, int y, int width, int height)
{
    sdlTtexture_ = make_unique<SDLTexture> (imageFile,
        sdlRenderer->getSDL_Renderer (), x, y, width, height);
}

SDLFront::~SDLFront ()
{}

void SDLFront::render ()
{
    sdlTtexture_->render ();
}
