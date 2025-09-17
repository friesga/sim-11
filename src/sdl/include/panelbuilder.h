#ifndef _PANELBUILDER_H_
#define _PANELBUILDER_H_

#include "panel.h"

struct PanelComposition
{
    vector<unique_ptr<Front>> fronts_;
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
    vector<unique_ptr<IndicatorButton>> indicatorButtons_;
};

// This class is an interface definition for SDL panel builders
class PanelBuilder
{
public:
    virtual PanelComposition getPanelComposition () = 0;
};

#endif // _PANELBUILDER_H_