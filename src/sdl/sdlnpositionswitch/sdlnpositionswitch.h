#ifndef _SDLNPOSITIONSWITCH_H_
#define _SDLNPOSITIONSWITCH_H_

#include "panel.h"

#include "../sdltile/sdltile.h"
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

// This class implements a switch with the positions indicated by the
// template parameter TPositions.
//
template <typename TPositions>
class SDLNPositionSwitch : public Button
{
public:
    using PositionTiles = vector<unique_ptr<SDLTile>>;

    SDLNPositionSwitch<TPositions> (PositionTiles positionTiles,
        TPositions initialState,
        EventCallback switchClicked);

    // Functions required for the Button interface
    void setState (State newState) override;
    State currentState () const override;
    void render () override;
    void handleEvent (InputEvent const* event) override;
    bool isWithinBounds (Position position, float margin) const override;

private:
    EventCallback switchClicked_;
    TPositions switchPosition_;
    PositionTiles positionTiles_;

    bool switchedUp (InputEvent const* event) const;
    bool switchedDown (InputEvent const* event) const;
    bool isRightOfCenter (Position position, float margin) const;
    bool isLeftOfCenter (Position position, float margin) const;
    bool isAboveCenter (Position position, float margin) const;
    bool isBelowCenter (Position position, float margin) const;
    TPositions nextPosition (TPositions position);
    TPositions previousPosition (TPositions position);
    TPositions togglePosition (TPositions position);

    const float centerMargin_ {0.5f};
};


#endif // _SDLNPOSITIONSWITCH_H_