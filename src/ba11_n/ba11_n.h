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
    Panel::BoundingBox pwrOkLedBoundingBox       {515, 114, 12, 12};
    Panel::BoundingBox runLedBoundingBox         {534, 114, 12, 12};
    Panel::BoundingBox restartSwitchBoundingBox  {600, 111, 20, 20};
    Panel::BoundingBox haltSwitchBoundingBox     {625, 111, 20, 20};
    Panel::BoundingBox auxOnOffSwitchBoundingBox {650, 111, 20, 20};

    Qbus *bus_;
    Window *frontWindow_;
    Indicator *pwrOkLed_;
    Indicator *runLed_;
    Button *restartSwitch_;
    Button *haltSwitch_;
    Button *auxOnOffSwitch_;
    BA11_NConfig::Logo logo_;

    void createBezel ();
    string frontImage (BA11_NConfig::Logo logo);
};

#endif // _BA11_N_H_
