#ifndef _BA11L_H_
#define _BA11L_H_

#include "qbus/qbus.h"
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
    BA11_L (Qbus* bus, Window* window, shared_ptr<BA11_LConfig> ba11lConfig);
    void powerSwitchClicked (Button::State state);

private:
    // Definition of the front, indicators and buttons on the BA11_N panel
    Frame<float> ba11_nFrontFrame {0, 0, 1.0, 1.0};
    Frame<float> powerSwitchFrame {0.86, 0.6, 0.016, 0.060};

    Qbus* bus_;
    Window* frontWindow_;
    Button* powerSwitch_;

    void createBezel (shared_ptr<Cabinet::Position> cabinetPosition);
};

#endif // _BA11L_H_
