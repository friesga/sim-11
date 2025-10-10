#ifndef _BA11C_H_
#define _BA11C_H_

#include "bus/include/bus.h"
#include "panel.h"
#include "configdata/ba11/ba11c/ba11cconfig/ba11cconfig.h"

#include <thread>
#include <string>

// The BA11-C mounting box was one of the two earliest earliest standard
// system unit backplane mounting boxes produced by DEC, along with the BA11-E.
// It was used to hold the KA11 CPU and front console (programmer's console or
// 'controller console') for the PDP-11/20 (hence the -C variant suffix).
// 
// The BA11's dimensions were 10-1/2" (i.e. 6 rack units) high, 19" wide,
// and 23" deep;
// 
// Source: https://gunkies.org/wiki/BA11-C_Mounting_Box
//
class BA11_C
{
public:
    BA11_C (Bus* bus, Window* window, const BA11_CConfig& ba11cConfig);

private:
    Bus* bus_;
    Window* frontWindow_;
    Button* powerSwitch_;

    Indicator* runLight_;
    
    void createBezel (Cabinet::Position cabinetPosition);
    void powerSwitchClicked (Button::State state);
};

#endif // _BA11C_H_
