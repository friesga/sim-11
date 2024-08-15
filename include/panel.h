#ifndef _PANEL_H_
#define _PANEL_H_

#include <string>
#include <memory>
#include <functional>

using std::string;
using std::unique_ptr;
using std::function;

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

// Definition of the position of a device in a cabinet. The cabinet number
// is ranking number, the left-most cabinet is cabinet 0. The height of the
// device in the cabinet is expressed in "rack units" (U), measured from the top
// of the device. See https://en.wikipedia.org/wiki/19-inch_rack and
// https://en.wikipedia.org/wiki/Rack_unit.
//
struct CabinetPosition
{
    size_t cabinetNr;
    size_t height;
};

class Indicator
{
public:
    enum class State
    {
        Off,
        On
    };

    virtual void show (State showFigure) = 0;
};

class Button
{
public:
    enum class State
    {
        Down,
        Up
    };

    using EventCallback = function<void(State)>;
};

class Panel
{
public:
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

    // A default value (0) may be specified for the width and height of
    // images. This indicates that the width and height of the image
    // will be used.
    virtual void createFront (string imageFile, 
        Frame<float> frame) = 0;
    virtual Indicator *createIndicator (string imageFile, 
        Indicator::State showFigure,
        Frame<float> boundingBox) = 0;
    virtual Button *createLatchingButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        Frame<float> boundingBox) = 0;
    virtual Button *createMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        Frame<float> boundingBox) = 0;
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
    virtual Panel *createPanel (CabinetPosition cabinetPosition) = 0;
    virtual void render () = 0;
    virtual bool handleEvents () = 0;
};

#endif // !_PANEL_H_
