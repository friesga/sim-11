#ifndef _PANEL_H_
#define _PANEL_H_

#include "cabinet/cabinet.h"
#include "rackunit.h"

#include <string>
#include <memory>
#include <functional>

using std::string;
using std::unique_ptr;
using std::function;
using std::shared_ptr;

//
// This header file defines an interface for a panel for the sim-11 GUI.
// The interface hides the GUI library used from the sim-11 classes.
// 
// A Window comprises one or more Panels (presumably one for each device),
// which on their turn comprise a front and one or more Indicators and
// Buttons.
//

struct Position
{
    int x;
    int y;
};

// The Frame struct defines a rectangle dimensions and position
// relative to a canvas (i.e. texture or window). The origin of the
// box is at the upper left corner and the down right corner is at the
// coordinates (1.0, 1.0).
template <typename T>
struct Frame
{
    T x;
    T y;
    T width;
    T height;
};

// The State enum constants are given values to be able to use them as
// indices in an array.
class Indicator
{
public:
    enum class State : size_t
    {
        Off = 0,
        On = 1
    };

    virtual void show (State showFigure) = 0;
};

class Button
{
public:
    enum class State : size_t
    {
        Down = 0,
        Up = 1
    };

    struct ImageNames
    {
        string buttonUpIndicatorOff;
        string buttonUpIndicatorOn;
        string buttonDownIndicatorOff;
        string buttonDownIndicatorOn;
    };

    using EventCallback = function<void(State)>;
};

class Panel
{
public:
    // A default value (0) may be specified for the width and height of
    // images. This indicates that the width and height of the image
    // will be used.
    virtual void createFront (string imageFile, 
        Frame<float> frame) = 0;
    virtual Indicator *createIndicator (string imageFile, 
        Indicator::State showFigure,
        Frame<float> frame) = 0;
    virtual Button* createLatchingButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        Frame<float> frame) = 0;
    virtual Button* createMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        Frame<float> frame) = 0;
    virtual Button* createSDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::State initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) = 0;
};

class Window
{
public:
    enum class Flag
    {
        WindowShown,
        WindowHidden
    };
    
    virtual void show () = 0;
    virtual Panel *createPanel (shared_ptr<Cabinet::Position> cabinetPosition,
        RackUnit unitHeight) = 0;
    virtual void render () = 0;
    virtual bool handleEvents () = 0;
};

#endif // !_PANEL_H_
