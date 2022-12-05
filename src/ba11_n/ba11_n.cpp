//
// Support for the BA11-N Mounting Box
//
#include <iostream>
#include <thread>

#include "ba11_n.h"

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
    SDL_DestroyRenderer (renderer_);
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
    init ("BA11-N", 100, 100, 300, 500, false);

    while (running_)
	{
		handleEvents();
		// update();
		render();
        std::this_thread::sleep_for (std::chrono::milliseconds (33));
	}
}
