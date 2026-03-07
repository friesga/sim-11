#ifndef _SDLINDICATORLATCHINGBUTTON_H_
#define _SDLINDICATORLATCHINGBUTTON_H_

#include "panel.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdltile/sdltile.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>
#include <array>

using std::string;
using std::unique_ptr;
using std::array;

// This class combines a latching button with an indicator.
//
class SDLIndicatorLatchingButton : public IndicatorButton
{
public:
    // The TileGrid is indexed first by the button state and then by
    // the indicator state. So TileGrid[0][0] is the tile for the
    // button in the on state and the indicator in the off state.
    using TileGrid = array<array<unique_ptr<SDLTile>, 2>, 2>;

    SDLIndicatorLatchingButton (TileGrid tiles,
        Button::TwoPositionsState initialState, EventCallback buttonClicked,
        Indicator::State showIndicator);
    ~SDLIndicatorLatchingButton ();

    // Definition of functions required for the Button interface
    void handleEvent (InputEvent const* event) override;
    void setState (Button::State newState) override;
    Button::State currentState () const override;
    void render () override;
    bool isWithinBounds (Position position, float margin = 0.0) const;
    void setSwitchClickedCallback (EventCallback callback) override;

    // Definition of functions required for the Indicator interface
    Indicator::State indicatorState () const override;
    void show (Indicator::State indicatorState) override;


private:
    SDLTile* getTile (Button::TwoPositionsState buttonState,
        Indicator::State indicatorState) const;

    EventCallback buttonClicked_;
    TileGrid tiles_;
    Button::TwoPositionsState buttonState_;
    Indicator::State indicatorState_;

};

// This function transforms an enum class to the underlying type (such as
// an int) so it can be used to index an array.
// See: https://stackoverflow.com/questions/14589417/can-an-enum-class-be-converted-to-the-underlying-type
//
template<typename E>
constexpr auto to_integral (E e) -> typename std::underlying_type<E>::type
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

#endif // _SDLINDICATORLATCHINGBUTTON_H_
