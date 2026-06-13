#ifndef _BA11L_H_
#define _BA11L_H_

#include "bus/include/bus.h"
#include "panel.h"
#include "configdata/ba11/ba11l/ba11lconfig/ba11lconfig.h"

#include <thread>
#include <string>

class BA11_L
{
public:
    BA11_L (Bus* bus, Window* window, const BA11_LConfig& ba11lConfig);
    void powerSwitchClicked (Button::State state);
    void hcbSwitchClicked (Button::State state);
    void SRUNReceiver (bool signalValue);

private:
    Bus* bus_;
    Window* frontWindow_;
    Button* powerSwitch_;
    Button* hcbSwitch_;
    Indicator* runLed_;
    Indicator* dcOnLed_;
    Indicator* batteryLed_;

    void createBezel (Cabinet::Position cabinetPosition);
    void disableHCBSwitch ();
    void enableHCBSwitch ();
    void blockSignals ();
    void unblockSignals ();
};

#endif // _BA11L_H_
