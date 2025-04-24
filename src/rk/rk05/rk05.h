#ifndef _RK05_H_
#define _RK05_H_

#include "unit/unit.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "panel.h"

#include <memory>

using std::shared_ptr;

class RK05 : public Unit
{
    Bus* bus_ {nullptr};
    PDP11Peripheral* controller_ {nullptr};

    // Definitions of buttons and indicatores
    Indicator* pwrIndicator_ {};
    Indicator* rdyIndicator_ {};
    Indicator* oncylIndicator_ {};
    Indicator* faultIndicator_ {};
    Indicator* wtprotIndicator_ {};
    Indicator* loadIndicator_ {};
    Indicator* wtIndicator_ {};
    Indicator* rdIndicator_ {};

    // Button and indicators positions and dimensions
    Frame<float> pwrIndicatorFrame    {0.674, 0.605, 0.024, 0.048};
    Frame<float> readyIndicatorFrame  {0.703, 0.605, 0.024, 0.048};
    Frame<float> oncylIndicatorFrame  {0.732, 0.605, 0.024, 0.048};
    Frame<float> faultIndicatorFrame  {0.761, 0.605, 0.024, 0.048};
    Frame<float> wtprotIndicatorFrame {0.674, 0.690, 0.024, 0.048};
    Frame<float> loadIndicatorFrame   {0.703, 0.690, 0.024, 0.048};
    Frame<float> wtIndicatorFrame     {0.732, 0.690, 0.024, 0.048};
    Frame<float> rdIndicatorFrame     {0.761, 0.690, 0.024, 0.048};

public:
    RK05 (Bus* bus, PDP11Peripheral* controller, Window* window,
        shared_ptr<RK05Config> configPtr);

private:
    void createBezel (Window* window, shared_ptr<RK05Config> rk05Config);
};


#endif // _RK05_H_