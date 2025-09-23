#include "sdlrenderer.h"
#include "../sdlwindow/sdlwindow.h"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;
using std::unique_ptr;

SDLRenderer::SDLRenderer (SDL_Window *sdlWindow, int index, unsigned long flags)
{
    // Create renderer for the given window
    sdl_Renderer_ = SDL_CreateRenderer (sdlWindow, index, flags);

    if (sdl_Renderer_ == NULL)
        throw runtime_error ("Renderer could not be created. SDL error: " +
            string (SDL_GetError ()));
}

SDLRenderer::~SDLRenderer ()
{
    SDL_DestroyRenderer (sdl_Renderer_);
}


// Create an empty texture with the given dimensions
unique_ptr<SDLTexture> SDLRenderer::createTexture (int textureWidth,
    int textureHeight)
{
    // The SDLTexture object has to be created using new() as make_unique()
    // has no access to the private constructor of SDLTexture
    return unique_ptr<SDLTexture> (new SDLTexture (this->sdl_Renderer_,
        textureWidth, textureHeight));
}

// Create a texture from the given image file
unique_ptr<SDLTexture> SDLRenderer::createTexture (string imageFile)
{
    return unique_ptr<SDLTexture> (new SDLTexture (this->sdl_Renderer_,
        imageFile));
}

void SDLRenderer::setTarget (SDLTexture& texture)
{
    if (SDL_SetRenderTarget (sdl_Renderer_, texture.sdlTexture_) != 0)
        throw runtime_error ("Unable to set render target: " +
            string (SDL_GetError ()));
}

void SDLRenderer::setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha)
{
    SDL_SetRenderDrawColor (sdl_Renderer_, red, green, blue, alpha);
}

void SDLRenderer::clear ()
{
    SDL_RenderClear (sdl_Renderer_);
}

// Draw (copy/render) the given texture to the frame buffer of the window
void SDLRenderer::copy (SDLTexture& texture)
{
    SDL_SetRenderTarget (sdl_Renderer_, nullptr);
    SDL_RenderCopy (sdl_Renderer_, texture.sdlTexture_, NULL, NULL);
}

void SDLRenderer::copy (SDL_Texture* texture, Frame<int> renderFrame)
{
    // Set rendering space and render texture
    SDL_Rect renderQuad {renderFrame.x, renderFrame.y, renderFrame.width,
        renderFrame.height};
    SDL_RenderCopy (sdl_Renderer_, texture, NULL, &renderQuad);
}

void SDLRenderer::update ()
{
    SDL_RenderPresent (sdl_Renderer_);
}

SDL_Renderer *SDLRenderer::getSDL_Renderer ()
{
    return sdl_Renderer_;
}