#include "ba11_n.h"

//
// Support for the BA11-N Mounting Box
//
#include <functional>
#include <map>

using std::unique_ptr;
using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::map;

// Constructor
// Create a window showing the BA11-N and devices and then start a thread
// handling the events and render the lamps and switches.
BA11_N::BA11_N (Qbus *bus, Window *window, shared_ptr<BA11_NConfig> ba11_nConfig)
    :
    bus_ {bus},
    frontWindow_ {window},
    logo_ {ba11_nConfig->logo}
{
    createBezel ();
}

// Destructor
BA11_N::~BA11_N ()
{}

// 
void BA11_N::createBezel ()
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
    Panel *panel = frontWindow_->createPanel ();

    panel->createFront (frontImage (logo_), 0, 0);
    pwrOkLed_ = panel->createIndicator ("../../assets/red led.png", 
        Indicator::State::Off, 515, 114, 12, 12);
    runLed_ = panel->createIndicator ("../../assets/red led.png", 
        Indicator::State::Off, 534, 114, 12, 12);

    restartSwitch_ = panel->createMomentaryButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::restartSwitchClicked, this, _1), 
        600, 111, 20, 20);
    haltSwitch_ = panel->createLatchingButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::haltSwitchToggled, this, _1),
        625, 111, 20, 20);
    auxOnOffSwitch_ = panel->createLatchingButton ("../../assets/switch down.png",
        "../../assets/switch up.png", 
        Button::State::Down, bind (&BA11_N::auxOnOffSwitchToggled, this, _1),
        650, 111, 20, 20);

    // Now the RUN led is created when can subscribe to the signal indicating
    // the state to be shown.
    bus_->SRUN().subscribe (bind (&BA11_N::SRUNReceiver, this, _1));
}

// Get the fromt image for the given logo
string BA11_N::frontImage (BA11_NConfig::Logo logo)
{
    map <BA11_NConfig::Logo, string> frontImages =
    {
        {BA11_NConfig::Logo::PDP_1103L, "../../assets/11_03 front.png"},
        {BA11_NConfig::Logo::PDP_1123,  "../../assets/11_23 front.png"}
    };

    return frontImages[logo];
}

void BA11_N::restartSwitchClicked (Button::State state)
{
    if (state == Button::State::Up)
        bus_->RESET ().cycle ();
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

void BA11_N::auxOnOffSwitchToggled (Button::State state)
{
    switch (state)
    {
        case Button::State::Down:
            pwrOkLed_->show (Indicator::State::Off);
            bus_->BPOK ().set (false);
            break;

        case Button::State::Up:
            // Set Power OK
            pwrOkLed_->show (Indicator::State::On);
            bus_->BPOK ().set (true);
            break;
    }
}

// The RUN led reflects the state of the SRUN signal.
void BA11_N::SRUNReceiver (bool signalValue)
{
    runLed_->show (signalValue ? Indicator::State::On : Indicator::State::Off);
}