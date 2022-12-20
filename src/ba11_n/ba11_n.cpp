#include "ba11_n.h"

//
// Support for the BA11-N Mounting Box
//
#include <iostream>
#include <thread>

using std::make_unique;

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
    // At least for Windows, event handling has to be performed in the same
    // thread as in which the window has been created.

    // Start up SDL
    SDLwrap sdlWrap {};

    //Create window
    window_ = std::make_unique<Window> ("BA11-N", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, 560, 149, SDL_WINDOW_SHOWN);

    //Create renderer for window
    renderer_ = std::make_unique<Renderer> (*window_);

    //Initialize renderer color
    renderer_->setDrawColor (0xFF, 0xFF, 0xFF, 0xFF);

    pdp11_03_frontTexture = make_unique<Texture> (*renderer_, "../../assets/pdp-11_03.png");
    pwrOkTexture_ = make_unique<Texture> (*renderer_, "../../assets/red-light-icon.png");


    while (running_)
	{
		handleEvents();
		// update();
		render();
        std::this_thread::sleep_for (std::chrono::milliseconds (33));
	}
}
