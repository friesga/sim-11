#ifndef _KY11_A_H_
#define _KY11_A_H_

#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "panel.h"

class KY11_A
{
public:
    KY11_A (Window* window, const KY11_AConfig& ky11_aConfig);

private:
    Button* powerSwitch_;

    Indicator* runLight_;

    void createBezel (Window* window, const KY11_AConfig& ky11_aConfig);
    void powerSwitchClicked (Button::State state);
};

#endif // _KY11_A_H_