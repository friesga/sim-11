#ifndef _PANEL_H_
#define _PANEL_H_

#include "cabinet/cabinet.h"
#include "rackunit.h"
#include "imagedata/include/imagecontainer.h"

#include <string>
#include <memory>
#include <functional>
#include <variant>
#include <array>

using std::string;
using std::unique_ptr;
using std::function;
using std::shared_ptr;
using std::variant;
using std::array;

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

// The class Event is an implementation-independend version of input events.
class InputEvent
{
public:
    enum class Button
    {
        Left,
        Right
    };

    enum class Type
    {
        MouseButtonDown,
        MouseButtonUp,
        MouseMotion,
        Quit,
        Other
    };

    virtual Type type () const = 0;
    virtual Button button () const = 0;
    virtual Position mousePosition () const = 0;
};

// A Front is the image to be rendered on the panel
class Front
{
public:
    virtual void render () = 0;
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
    virtual void render () = 0;
    virtual bool isWithinBounds (Position position, float margin) const = 0;
};

class Button
{
public:
    enum class TwoPositionsState : size_t
    {
        Down = 0,
        Up = 1
    };

    enum class MomentaryTwoPositionsState : size_t
    {
        Down = 0,
        Up = 1
    };

    enum class CenteredTwoPositionsState : size_t
    {
        Down = 0,
        Up = 1
    };

    enum class ThreePositionsState : size_t
    {
        Left = 0,
        Center = 1,
        Right = 2
    };

    // Definition of a three position switch the last position of which
    // is  momentary.
    enum class MomentaryThreePositionsState : size_t
    {
        Left = 0,
        Center = 1,
        Right = 2
    };

    enum class FourPositionsState : size_t
    {
        P0 = 0,
        P1 = 1,
        P2 = 2,
        P3 = 3
    };

    struct ImageNames
    {
        string buttonUpIndicatorOff;
        string buttonUpIndicatorOn;
        string buttonDownIndicatorOff;
        string buttonDownIndicatorOn;
    };

    using State = variant<TwoPositionsState, MomentaryTwoPositionsState,
        CenteredTwoPositionsState, ThreePositionsState,
        MomentaryThreePositionsState, FourPositionsState>;

    using EventCallback = function<void(State)>;
    virtual void setState (State newState) = 0;
    virtual State currentState () const = 0;
    virtual void render () = 0;
    virtual void handleEvent (InputEvent const* event) = 0;
    virtual bool isWithinBounds (Position position, float margin) const = 0;
};

// Definition of type traits for the different types of switches. For every
// Button enum type to be handled by SDLMultiPositionSwitch, these type traits
// have to be defined. They configure the behaviour of the switch. 
// 
// The first and last enum values are used to switch the Button to the
// correct position.
// 
// The isLatching trait indicates whether the the last position of the switch
// is latching or momentary.
//
// The orientation trait indicates whether the switch is oriented horizontally,
// vertically or is centered. This is used to determine whether a click on the
// left or right side (horizontal), the upper or lower side (vertical) or the
// center of the switch updates the switch position.
//
enum class Orientation
{
    Horizontal,
    Vertical,
    Centered
};

template <typename T>
struct ButtonTrait;

template <>
struct ButtonTrait<Button::TwoPositionsState>
{
    static constexpr Button::TwoPositionsState first =
        Button::TwoPositionsState::Down;
    static constexpr Button::TwoPositionsState last =
        Button::TwoPositionsState::Up;
    static const bool isLatching = true;
    static const Orientation orientation = Orientation::Vertical;
};

template <>
struct ButtonTrait<Button::MomentaryTwoPositionsState>
{
    static constexpr Button::MomentaryTwoPositionsState first =
        Button::MomentaryTwoPositionsState::Down;
    static constexpr Button::MomentaryTwoPositionsState last =
        Button::MomentaryTwoPositionsState::Up;
    static const bool isLatching = false;
    static const Orientation orientation = Orientation::Vertical;
};

template <>
struct ButtonTrait<Button::CenteredTwoPositionsState>
{
    static constexpr Button::CenteredTwoPositionsState first =
        Button::CenteredTwoPositionsState::Down;
    static constexpr Button::CenteredTwoPositionsState last =
        Button::CenteredTwoPositionsState::Up;
    static const bool isLatching = true;
    static const Orientation orientation = Orientation::Centered;
};

template <>
struct ButtonTrait<Button::ThreePositionsState>
{
    static constexpr Button::ThreePositionsState first =
        Button::ThreePositionsState::Left;
    static constexpr Button::ThreePositionsState last =
        Button::ThreePositionsState::Right;
    static const bool isLatching = true;
    static const Orientation orientation = Orientation::Horizontal;
};

template <>
struct ButtonTrait<Button::MomentaryThreePositionsState>
{
    static constexpr Button::MomentaryThreePositionsState first =
        Button::MomentaryThreePositionsState::Left;
    static constexpr Button::MomentaryThreePositionsState last =
        Button::MomentaryThreePositionsState::Right;
    static const bool isLatching = false;
    static const Orientation orientation = Orientation::Horizontal;
};

template <>
struct ButtonTrait<Button::FourPositionsState>
{
    static constexpr Button::FourPositionsState first =
        Button::FourPositionsState::P0;
    static constexpr Button::FourPositionsState last =
        Button::FourPositionsState::P3;
    static const bool isLatching = true;
    static const Orientation orientation = Orientation::Horizontal;

};

// An IndicatorButton is the combination of a button and an indicator. i.e.
// it can be switched up and down and has an indicator that can be switched
// on and off.
class IndicatorButton : public Button, public Indicator
{
public:
    // Functions defined in the Button interface
    virtual void render () = 0;
    virtual void setState (Button::State newState) = 0;
    virtual void handleEvent (InputEvent const* event) = 0;
    virtual bool isWithinBounds (Position position, float margin) const = 0;

    // Additional functions defined in the Indicator interface
    virtual void show (Indicator::State showFigure) = 0;
};

struct PanelComposition
{
    vector<unique_ptr<Front>> fronts_;
    vector<unique_ptr<Indicator>> indicators_;
    vector<unique_ptr<Button>> buttons_;
    vector<unique_ptr<IndicatorButton>> indicatorButtons_;
};

class Panel
{
public:
    virtual void render () = 0;
    virtual void handleEvent (InputEvent const* event) = 0;
    virtual bool isOverButton (Position position) = 0;
};

class PanelBuilder
{
public:
    // A default value (0) may be specified for the width and height of
    // images. This indicates that the width and height of the image
    // will be used.
    virtual void createFront (string imageFile,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) = 0;
    virtual Indicator* createIndicator (string indicatorOffImage,
        string indicatorOnImage, Indicator::State showFigure,
        Frame<float> frame) = 0;
    virtual IndicatorButton* createIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::TwoPositionsState initialState,
        Button::EventCallback buttonClicked, Indicator::State showIndicator,
        Frame<float> frame) = 0;
    virtual Button* createMultiPositionSwitch (vector<string> positionImages,
        Button::State initialState,
        Button::EventCallback switchClicked,
        Frame<float> frame = Frame<float> (0, 0, 0, 0)) = 0;
    virtual unique_ptr<Panel> getPanel () = 0;
    virtual ~PanelBuilder () = default;
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
    virtual Panel *createPanel (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) = 0;
    virtual unique_ptr<PanelBuilder> createFilePanelBuilder (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) = 0;
    virtual unique_ptr<PanelBuilder> createDataPanelBuilder (ImageContainer& imageContainer,
        Cabinet::Position cabinetPosition, RackUnit unitHeight) = 0;
    virtual void addPanel (unique_ptr<Panel> panel) = 0;
    virtual void render () = 0;
    virtual bool handleEvents () = 0;
};

#endif // !_PANEL_H_
