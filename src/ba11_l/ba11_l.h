#ifndef _BA11L_H_
#define _BA11L_H_

#include "bus.h"
#include "panel.h"
#include "configdata/ba11lconfig/ba11lconfig.h"

#include <thread>
#include <memory>
#include <string>

using std::unique_ptr;
using std::shared_ptr;

class BA11_L
{
public:
    BA11_L (Bus* bus, Window* window, shared_ptr<BA11_LConfig> ba11lConfig);
    void powerSwitchClicked (Button::State state);
    void hcbSwitchClicked (Button::State state);
    void SRUNReceiver (bool signalValue);

private:
    // Definition of the front, indicators and buttons on the BA11_N panel
    Frame<float> ba11_nFrontFrame {0, 0, 1.0, 1.0};
    Frame<float> powerSwitchFrame {0.853, 0.599, 0.032, 0.136};
    Frame<float> runLedFrame      {0.674, 0.502, 0.016, 0.060};
    Frame<float> dcOnLedFrame     {0.705, 0.502, 0.016, 0.060};
    Frame<float> batteryLedFrame  {0.736, 0.502, 0.016, 0.060};
    Frame<float> hcbSwitchFrame   {0.710, 0.673, 0.029, 0.060};

    Bus* bus_;
    Window* frontWindow_;
    Button* powerSwitch_;
    Button* hcbSwitch_;
    Indicator* runLed_;
    Indicator* dcOnLed_;
    Indicator* batteryLed_;

    void createBezel (shared_ptr<Cabinet::Position> cabinetPosition);
    void disableHCBSwitch ();
    void enableHCBSwitch ();
    void blockSignals ();
    void unblockSignals ();
};

#endif // _BA11L_H_
