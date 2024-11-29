#ifndef _SDLFOURPOSITIONSWITCH_H_
#define _SDLFOURPOSITIONSWITCH_H_

#include "panel.h"
#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"

#include <string>
#include <array>
#include <vector>
#include <memory>

using std::string;
using std::array;
using std::vector;
using std::unique_ptr;

// Definition of operator+ for scoped enums to convert it to the underlying
// type.
// Source: https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int
//
template <typename T>
constexpr auto operator+ (T e) noexcept
-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
{
    return static_cast<std::underlying_type_t<T>> (e);
}

class SDLFourPositionSwitch : public Button
{
public:
    SDLFourPositionSwitch (array<string, 4> positionImages,
        Button::FourPositionsState initialState,
        unique_ptr<SDLRenderer>& sdlRenderer,
        EventCallback switchClicked,
        SDL_Texture* targetTexture,
        Frame<int> frame);

    // Functions required for the Button interface
    void setState (State newState);
    void render ();
    void handleEvent (InputEvent const* event);
    bool isWithinBounds (Position position, float margin) const;

private:
    EventCallback switchClicked_;
    Frame<int> frame_;

    Button::FourPositionsState switchPosition_;
    vector<unique_ptr<SDLTexture>> positionTextures_;

    bool isRightOfCenter (Position position, float margin) const;
    bool isLeftOfCenter (Position position, float margin) const;
    Button::FourPositionsState nextPosition (Button::FourPositionsState position);
    Button::FourPositionsState previousPosition (Button::FourPositionsState position);
};

#endif // _SDLFOURPOSITIONSWITCH_H_