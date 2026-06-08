#ifndef _BA11_N_H_
#define _BA11_N_H_

#include "bus/include/bus.h"
#include "panel.h"
#include "configdata/ba11/ba11n/ba11nconfig/ba11nconfig.h"

#include <thread>
#include <string>

class BA11_N
{
public:
    BA11_N (Bus *bus, Window *window, const BA11_NConfig& ba11_nConfig);
    ~BA11_N ();
    void restartSwitchClicked (Button::State state);
    void haltSwitchToggled (Button::State state);
    void auxOnOffSwitchToggled (Button::State state);
    void SRUNReceiver (bool signalValue);

private:
    // Definition of position and dimensions of the BA11_N panel
    Frame<float> ba11_nFrontFrame    {0, 0, 1.0, 1.0};

    Bus *bus_;
    Window *frontWindow_;
    Indicator *pwrOkLed_;
    Indicator *runLed_;
    Button *restartSwitch_;
    Button *haltSwitch_;
    Button *auxOnOffSwitch_;
    BA11_NConfig::Logo logo_;

    void createBezel (Cabinet::Position cabinetPosition);
    void createLabel (unique_ptr<PanelBuilder>& panelBuilder,
        BA11_NConfig::Logo logo);
};

#endif // _BA11_N_H_
