#include "ba11_n.h"
#include "../sdl/sdlwrap/sdlwrap.h"
#include "../sdl/texture/texture.h"
#include "../sdl/window/window.h"
#include "../sdl/renderer/renderer.h"

//
// Support for the BA11-N Mounting Box
//
#include <iostream>
#include <thread>

// Constructor
// Create a window showing the BA11-N and devices and then start a thread
// handling the events and render the lamps and switches.
BA11_N::BA11_N ()
{
    // Create a thread running the event handler and rendering
    ba11_nThread_ = std::thread (&BA11_N::bezel, this);
    running_ = true;
}

// Destructor
BA11_N::~BA11_N ()
{
    SDL_DestroyWindow (window_);
    if (renderer_ != nullptr)
        SDL_DestroyRenderer (renderer_);
    if (texture_ != nullptr)
        SDL_DestroyTexture (texture_);
    SDL_Quit ();

    // Signal thread to stop running
    running_ = false;

    // Wait till the thread has finished
    ba11_nThread_.join ();
}

void BA11_N::bezel ()
{
    // Create the BA11-N bezel at the specified position, width and height
    // and then start a loop handling the events and rendering lamps and
    // switches.
	// Width and height are dependent on the image dimensions and should
	// be retrieved before the window is created. At least for Windows, event
    // handling has to be performed in the same thread as in which the window
    // has been created.

    // init ("BA11-N", 100, 100, 560, 149, false);
    // Start up SDL
    SDLwrap sdlWrap {};

    //Create window
    Window myWindow ("BA11-N", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        560, 149, SDL_WINDOW_SHOWN);

    //Create renderer for window
    Renderer myRenderer (myWindow);

    //Initialize renderer color
    myRenderer.setDrawColor (0xFF, 0xFF, 0xFF, 0xFF);

    Texture fooTexture {myRenderer, "../../assets/red-light-icon.png"};
    Texture backgroundTexture {myRenderer, "../../assets/pdp-11_03.png"};

    while (running_)
	{
		handleEvents();
		// update();
		// render();
        // std::this_thread::sleep_for (std::chrono::milliseconds (33));
        myRenderer.setDrawColor (0xFF, 0xFF, 0xFF, 0xFF);
        myRenderer.clear ();

        // Render background texture to the window
        backgroundTexture.render (0, 0);

        // Render scaled fooTexture to the window
        fooTexture.render (240, 100, 20, 20);

        //Update screen
        myRenderer.update ();
	}
}
