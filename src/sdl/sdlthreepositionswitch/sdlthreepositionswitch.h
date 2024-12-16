#ifndef _THREEPOSITIONSWITCH_H_
#define _THREEPOSITIONSWITCH_H_

#include "panel.h"

#include "../sdltexture/sdltexture.h"
#include "../sdlrenderer/sdlrenderer.h"
#include "unary_operator_plus.h"

#include <string>
#include <array>
#include <vector>
#include <memory>

using std::string;
using std::array;
using std::vector;
using std::unique_ptr;

// This class implements a switch with three positions with the pecularity
// that two positions of the switch are latching and the third position is
// momentary.
//
class SDLThreePositionSwitch : public Button
{
public:
    SDLThreePositionSwitch (array<string, 3> positionImageNames,
        Button::ThreePositionsState initialState,
        unique_ptr<SDLRenderer>& sdlRenderer,
        EventCallback switchClicked,
        SDL_Texture* targetTexture,
        Frame<int> frame);

    // Functions required for the Button interface
    void setState (State newState);
    State currentState () const override;
    void render ();
    void handleEvent (InputEvent const* event);
    bool isWithinBounds (Position position, float margin) const;

private:
    EventCallback switchClicked_;
    Frame<int> frame_;

    Button::ThreePositionsState switchPosition_;
    vector<unique_ptr<SDLTexture>> positionTextures_;

    bool isRightOfCenter (Position position, float margin) const;
    bool isLeftOfCenter (Position position, float margin) const;
    Button::ThreePositionsState nextPosition (Button::ThreePositionsState position);
    Button::ThreePositionsState previousPosition (Button::ThreePositionsState position);
};


#endif // _THREEPOSITIONSWITCH_H_