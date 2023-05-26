#ifndef _SDLINIT_H_
#define _SDLINIT_H_

// SDL initialization is put in its own class as initialization and quitting
// the SDL subsystems perfectly fits the constructor/destructor condept.
class SDLInit
{
public:
    SDLInit ();
    ~SDLInit ();
};


#endif // _SDLINIT_H_