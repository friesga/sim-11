#ifndef _SDLPANEL_H_
#define _SDLPANEL_H_

#include "panel.h"

#include <memory>

using std::unique_ptr;

class SDLPanel : public Panel
{
public:
    SDLPanel ();
    ~SDLPanel ();
    unique_ptr<Window> createWindow (char const *title, 
        int x, int y, int width, int height) override;
};

#endif // _SDLPANEL_H_