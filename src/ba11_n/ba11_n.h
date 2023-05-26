#ifndef _BA11_N_H_
#define _BA11_N_H_

#include "qbus/qbus.h"
#include "panel.h"

#include <thread>
#include <memory>
#include <string>

using std::unique_ptr;

class BA11_N
{
public:
    BA11_N (Qbus *bus, Window *window);
    ~BA11_N ();
    void restartSwitchClicked (Button::State state);
    void haltSwitchToggled (Button::State state);
    void auxOnOffSwitchToggled (Button::State state);
    void SRUNReceiver (bool signalValue);

private:
    Qbus *bus_;
    Window *frontWindow_;
    Indicator *frontFigure_;
    Indicator *pwrOkLed_;
    Indicator *runLed_;
    Button *restartSwitch_;
    Button *haltSwitch_;
    Button *auxOnOffSwitch_;

    void createBezel ();
};

#endif // _BA11_N_H_
