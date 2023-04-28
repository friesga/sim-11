#include "ba11_n.h"

//
// Support for the BA11-N Mounting Box
//
#include <functional>
#include <thread>

using std::unique_ptr;
using std::this_thread::sleep_for;
using std::bind;
using std::placeholders::_1;

// Constructor
// Create a window showing the BA11-N and devices and then start a thread
// handling the events and render the lamps and switches.
BA11_N::BA11_N (Qbus *bus)
    :
    bus_ {bus}
{
    // Create a thread running the event handler and rendering
    ba11_nThread_ = std::thread (&BA11_N::bezel, this);
}

// Destructor
BA11_N::~BA11_N ()
{
    // Wait till the thread has finished
    ba11_nThread_.join ();
}

void BA11_N::bezel ()
{
    // Create the BA11-N panel at the specified position, width and height
    // and then start a loop handling the events and rendering lamps and
    // switches.
    // 
    // At least for Windows, event handling has to be performed in the same
    // thread as in which the window has been created.
    myWindow = sdlPanel_.createWindow ("PDP-11/03",
        100, 100, 750, 200);

    myWindow->createFront ("../../assets/11_03 front.png", 0, 0);
    indicator1_ = myWindow->createIndicator ("../../assets/red led.png", 
        Indicator::State::On, 515, 114, 12, 12);
    indicator2_ = myWindow->createIndicator ("../../assets/red led.png", 
        Indicator::State::On, 534, 114, 12, 12);

    button1_ = myWindow->createMomentaryButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::button1Clicked, this, _1), 
        600, 111, 20, 20);
    button2_ = myWindow->createLatchingButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::button1Clicked, this, _1),
        625, 111, 20, 20);
    button3_ = myWindow->createLatchingButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::button1Clicked, this, _1),
        650, 111, 20, 20);

    // Start rendering the panel
    render ();

    // The user clicked the close button. Set the bus EXIT signal to indicate
    // the simulator has to stop
    bus_->setSignal (Qbus::Signal::EXIT, Qbus::SignalValue::True);
}

void BA11_N::render ()
{
    do
	{
		myWindow->render();
        sleep_for (std::chrono::milliseconds (10));
	}
    while (!myWindow->handleEvents ());
}

void BA11_N::button1Clicked (Button::State state)
{
    switch (state)
    {
        case Button::State::Down:
            indicator2_->show (Indicator::State::Off);
            break;

        case Button::State::Up:
            indicator2_->show (Indicator::State::On);
            break;
    }
}