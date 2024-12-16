#ifndef _SDLFOURPOSITIONSWITCH_H_
#define _SDLFOURPOSITIONSWITCH_H_

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
    State currentState () const override;
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