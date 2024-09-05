#ifndef _BA11_N_H_
#define _BA11_N_H_

#include "qbus/qbus.h"
#include "panel.h"
#include "configdata/ba11nconfig/ba11nconfig.h"

#include <thread>
#include <memory>
#include <string>

using std::unique_ptr;
using std::shared_ptr;

class BA11_N
{
public:
    BA11_N (Qbus *bus, Window *window, shared_ptr<BA11_NConfig> ba11_nConfig);
    ~BA11_N ();
    void restartSwitchClicked (Button::State state);
    void haltSwitchToggled (Button::State state);
    void auxOnOffSwitchToggled (Button::State state);
    void SRUNReceiver (bool signalValue);

private:
    // Definition of the front, indicators and buttons on the BA11_N panel
    Frame<float> ba11_nFrontFrame    {0, 0, 1.0, 1.0};
    Frame<float> pwrOkLedFrame       {0.687, 0.573, 0.016, 0.060};
    Frame<float> runLedFrame         {0.712, 0.573, 0.016, 0.060};
    Frame<float> restartSwitchFrame  {0.800, 0.558, 0.027, 0.101};
    Frame<float> haltSwitchFrame     {0.833, 0.558, 0.027, 0.101};
    Frame<float> auxOnOffSwitchFrame {0.867, 0.558, 0.027, 0.101};

    Qbus *bus_;
    Window *frontWindow_;
    Indicator *pwrOkLed_;
    Indicator *runLed_;
    Button *restartSwitch_;
    Button *haltSwitch_;
    Button *auxOnOffSwitch_;
    BA11_NConfig::Logo logo_;

    void createBezel (shared_ptr<Cabinet::Position> cabinetPosition);
    string frontImage (BA11_NConfig::Logo logo);
};

#endif // _BA11_N_H_
