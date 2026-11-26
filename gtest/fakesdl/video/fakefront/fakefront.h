#ifndef _FAKEFRONT_H_
#define _FAKEFRONT_H_

#include "panel.h"

class FakeFront : public Front
{
public:
    FakeFront ();
    ~FakeFront ();
    void show (State image) override;
    void render () override;
    void handleEvent (InputEvent const* event) override;
};


#endif // _FAKEFRONT_H_