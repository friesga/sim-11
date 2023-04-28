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
    void button1Clicked (Button::State state);

private:
    Qbus *bus_;

	std::thread ba11_nThread_;

    SDLPanel sdlPanel_;
    unique_ptr<Window> myWindow;
    Indicator *frontFigure_;
    Indicator *indicator1_;
    Indicator *indicator2_;
    Button *button1_;
    Button *button2_;
    Button *button3_;

    void bezel ();
};

#endif // _BA11_N_H_
