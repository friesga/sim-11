#ifndef _SDLINDICATORLATCHINGBUTTON_H_
#define _SDLINDICATORLATCHINGBUTTON_H_

#include "panel.h"
#include "../sdlcommonbutton/sdlcommonbutton.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlevent/sdlevent.h"

#include <SDL.h>
#include <string>
#include <array>

using std::string;
using std::unique_ptr;
using std::array;

// This class combines a latching button with an indicator.
//
class SDLIndicatorLatchingButton : public Button, public Indicator
{
public:
    SDLIndicatorLatchingButton (Button::ImageNames const& imageNames,
        Button::State initialState, unique_ptr<SDLRenderer>& sdlRenderer,
        EventCallback buttonClicked, Indicator::State showIndicator,
        SDL_Texture* targetTexture, Frame<int> frame);
    ~SDLIndicatorLatchingButton ();

    // Definition of functions required for the Button interface
    void handleEvent (InputEvent const* event) override;
    void render () override;
    bool isWithinBounds (Position position, float margin = 0.0) const;

    // Definition of functions required for the Indicator interface
    void show (Indicator::State indicatorState) override;


private:
    SDLTexture* getTexture (Button::State buttonState,
        Indicator::State indicatorState) const;

    EventCallback buttonClicked_;
    array<array<unique_ptr<SDLTexture>, 2>, 2> textures_;
    Button::State buttonState_;
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
