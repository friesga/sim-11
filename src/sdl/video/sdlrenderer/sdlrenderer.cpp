#include "sdlrenderer.h"
#include "../sdlwindow/sdlwindow.h"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;
using std::unique_ptr;

SDLRenderer::SDLRenderer (SDL_Window *sdlWindow)
{
    // Create renderer for the given window
    sdl2_Renderer_ = SDL_CreateRenderer (sdlWindow, NULL);

    if (sdl2_Renderer_ == NULL)
        throw runtime_error ("Renderer could not be created. SDL error: " +
            string (SDL_GetError ()));

    // WASM The renderer has to be created with the
    // SDL_RENDERER_PRESENTVSYNC flag to avoid SDL2 setting the frame
    // rate.
    // See: https://github.com/emscripten-core/emscripten/issues/11788
    // 
    // In SDL3 the SDL_RENDERER_PRESENTVSYNC flag is replaced by the
    // SDL_SetRenderVSync function.
    //
    SDL_SetRenderVSync (sdl2_Renderer_, SDL_RENDERER_VSYNC_ADAPTIVE);
}

SDLRenderer::~SDLRenderer ()
{
    SDL_DestroyRenderer (sdl2_Renderer_);
}


// Create an empty texture with the given dimensions
unique_ptr<SDLTexture> SDLRenderer::createTexture (int textureWidth,
    int textureHeight)
{
    // The SDLTexture object has to be created using new() as make_unique()
    // has no access to the private constructor of SDLTexture
    return unique_ptr<SDLTexture> (new SDLTexture (this->sdl2_Renderer_,
        textureWidth, textureHeight));
}

// Create a texture from the given image file
unique_ptr<SDLTexture> SDLRenderer::createTexture (string imageFile)
{
    return unique_ptr<SDLTexture> (new SDLTexture (this->sdl2_Renderer_,
        imageFile));
}

// Create a texture from the given image data
unique_ptr<SDLTexture> SDLRenderer::createTexture (Image& image)
{
    return unique_ptr<SDLTexture> (new SDLTexture (this->sdl2_Renderer_,
        image));
}

void SDLRenderer::setTarget (SDLTexture& texture)
{
    if (SDL_SetRenderTarget (sdl2_Renderer_, texture.sdl2_Texture_) != 0)
        throw runtime_error ("Unable to set render target: " +
            string (SDL_GetError ()));
}

void SDLRenderer::setDrawColor (unsigned char red, unsigned char green,
        unsigned char blue, unsigned char alpha)
{
    SDL_SetRenderDrawColor (sdl2_Renderer_, red, green, blue, alpha);
}

void SDLRenderer::clear ()
{
    SDL_RenderClear (sdl2_Renderer_);
}

// Draw (copy/render) the given texture to the frame buffer of the window
void SDLRenderer::copy (SDLTexture& texture)
{
    SDL_SetRenderTarget (sdl2_Renderer_, nullptr);
    SDL_RenderTexture (sdl2_Renderer_, texture.sdl2_Texture_, NULL, NULL);
}

void SDLRenderer::copy (SDLTexture& texture, Frame<int> renderFrame)
{
    // Set rendering space and render texture
    // Note: In an initializer_list an implicit conversion from int to float
    // is not allowed, hence the explicit conversion to float.
    //
    SDL_FRect renderQuad
    {
        static_cast<float> (renderFrame.x),
        static_cast<float> (renderFrame.y),
        static_cast<float> (renderFrame.width),
        static_cast<float> (renderFrame.height)
    };

    SDL_RenderTexture (sdl2_Renderer_, texture.sdl2_Texture_, NULL, &renderQuad);
}

void SDLRenderer::update ()
{
    SDL_RenderPresent (sdl2_Renderer_);
}
