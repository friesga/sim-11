#ifndef _SDLBUTTON_H_
#define _SDLBUTTON_H_

#include "panel.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>

using std::string;
using std::unique_ptr;

class SDLButton : public Button
{
public:
    virtual void handleEvent (SDLEvent const* event) = 0;
    virtual bool isWithinBounds (Position position, float margin = 0.0) const = 0;
    virtual void render () = 0;
};

#endif // _SDLBUTTON_H_