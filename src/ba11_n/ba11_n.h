#ifndef _BA11_N_H_
#define _BA11_N_H_

#include "qbus/qbus.h"
#include "../sdl/sdlpanel/sdlpanel.h"

#include <thread>
#include <memory>
#include <string>

using std::unique_ptr;

class BA11_N
{
public:
    BA11_N (Qbus *bus);
    ~BA11_N ();
    void render ();
    void restartSwitchClicked (Button::State state);
    void haltSwitchToggled (Button::State state);
    void auxOnOffSwitchToggled (Button::State state);
    void SRUNReceiver (Qbus::Signal signal, Qbus::SignalValue signalValue);

private:
    Qbus *bus_;

	std::thread ba11_nThread_;

    SDLPanel sdlPanel_;
    unique_ptr<Window> frontWindow_;
    Indicator *frontFigure_;
    Indicator *pwrOkLed_;
    Indicator *runLed_;
    Button *restartSwitch_;
    Button *haltSwitch_;
    Button *auxOnOffSwitch_;

    void bezel ();
};

#endif // _BA11_N_H_
