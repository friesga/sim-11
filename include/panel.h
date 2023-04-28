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
// A Panel comprises a Window, which on its turn comprises a front,
// and one or more Indicators and Buttons.
//
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

class Window
{
public:
    // A default value (0) may be specified for the width and height of
    // images. This indicates that the width and height of the image
    // will be used.
    virtual void createFront (string imageFile, 
        int x, int y, int width = 0, int height = 0) = 0;
    virtual Indicator *createIndicator (string imageFile, 
        Indicator::State showFigure,
        int x, int y, int width = 0, int height = 0) = 0;
    virtual Button *createLatchingButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        int x, int y, int width = 0, int height = 0) = 0;
    virtual Button *createMomentaryButton (string buttonDownImage, string buttonUpImage,
        Button::State initialState, Button::EventCallback buttonClicked,
        int x, int y, int width = 0, int height = 0) = 0;
    virtual void render () = 0;
    virtual bool handleEvents () = 0;
};

class Panel
{
public:
    virtual unique_ptr<Window> createWindow (char const *title, 
        int x, int y, int width, int height) = 0;
};

#endif // !_PANEL_H_
