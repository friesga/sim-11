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

// 
void BA11_N::bezel ()
{
    // Create the BA11-N panel at the specified position, width and height
    // and then start a loop handling the events and rendering lamps and
    // switches.
    // 
    // The operator switch panel contains three switches: AUX ON/OFF, HALT,
    // and RESTART. The functions and positions of the switches are as follows:
    // 
    // AUX ON/OFF OFF   As configured at the factory, this switch, when turned
    //                  off, removes ac power to the system. 
    //            ON    As configured at the factory, it turns on ac power. If
    //                  the automatic bootstrap is selected and the HALT switch
    //                  is up, the system boots.
    // 
    // HALT       Up    (Enable) This enables the processor to run
    //            Down  (HALT) This halts the processor, which responds to
    //                  console OOT commands.
    // 
    // RESTART          (Momentary switch) When this switch is activated,
    //                  the processor carries out a power-up sequence.
    //                  The HALT switch must be up (enable).
    // 
    // The switch panel also contains two indicators that provide the following
    // information:
    // 
    // PWR OK     On    This LED lights when the proper dc output voltages are
    //                  being generated by the H786 power supply.
    // 
    // RUN        On    This LED lights when the processor is in the run state.
    //                  It goes out when the processor is not executing
    //                  instructions.
    // 
    // Source: LSI-11 Systems Service Manual
    // 
    // At least for Windows, event handling has to be performed in the same
    // thread as in which the window has been created.
    //
    frontWindow_ = sdlPanel_.createWindow ("PDP-11/03",
        100, 100, 750, 200);

    frontWindow_->createFront ("../../assets/11_03 front.png", 0, 0);
    pwrOkLed_ = frontWindow_->createIndicator ("../../assets/red led.png", 
        Indicator::State::On, 515, 114, 12, 12);
    runLed_ = frontWindow_->createIndicator ("../../assets/red led.png", 
        Indicator::State::On, 534, 114, 12, 12);

    restartSwitch_ = frontWindow_->createMomentaryButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::restartSwitchClicked, this, _1), 
        600, 111, 20, 20);
    haltSwitch_ = frontWindow_->createLatchingButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::haltSwitchToggled, this, _1),
        625, 111, 20, 20);
    auxOnOffSwitch_ = frontWindow_->createLatchingButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::auxOnOffSwitchToggled, this, _1),
        650, 111, 20, 20);

    // Now the RUN led is created when can subscribe to the signal indicating
    // the state to be shown.
    bus_->SRUN().subscribe (bind (&BA11_N::SRUNReceiver, this, _1));

    // Set DC power ok
    bus_->BDCOK().set (true);

    // Start rendering the panel
    render ();

    // The user clicked the close button. Set the bus EXIT signal to indicate
    // the simulator has to stop
    bus_->EXIT().set (true);
}

void BA11_N::render ()
{
    do
	{
		frontWindow_->render();
        sleep_for (std::chrono::milliseconds (10));
	}
    while (!frontWindow_->handleEvents ());
}

void BA11_N::restartSwitchClicked (Button::State state)
{
    if (state == Button::State::Up)
        bus_->RESTART ().cycle ();
}

void BA11_N::haltSwitchToggled (Button::State state)
{
    switch (state)
    {
        case Button::State::Down:
            bus_->BHALT().set (false);
            break;

        case Button::State::Up:
            bus_->BHALT().set (true);
            break;
    }
}

// ToDo: Implement AUX ON/OFF switch behaviour
void BA11_N::auxOnOffSwitchToggled (Button::State state)
{}

// The RUN led reflects the state of the SRUN signal.
//
// This function is called from another thread so possible run conditions
// might occur. As long as this is the only call ro runLed_->show() this
// should be fine.
void BA11_N::SRUNReceiver (bool signalValue)
{
    runLed_->show (signalValue ? Indicator::State::On : Indicator::State::Off);
}