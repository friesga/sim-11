#ifndef _DUMMYCONTROLS_H_
#define _DUMMYCONTROLS_H_

#include "panel.h"

// This file contains dummy panel elements to be used to avoid
// null pointer references.
class DummyIndicator : public Indicator
{
public:
    void show (State showFigure) {};
    void render () {};
};

#endif // _DUMMYCONTROLS_H_