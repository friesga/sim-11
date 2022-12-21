#ifndef _SDL_H_
#define _SDL_H_

// The function of this class is solely to initalize SDL and make
// sure SDL's quit's functions are called on exit (or return).
class SDLwrap
{
public:
    SDLwrap ();
    ~SDLwrap ();
};

#endif // _SDL_H_