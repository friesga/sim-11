#include "ba11_n.h"

void BA11_N::render()
{
    renderer_->setDrawColor (0xFF, 0xFF, 0xFF, 0xFF);
    renderer_->clear ();

    // Render background texture to the window
    pdp11_03_frontTexture->render (0, 0);

    // Render scaled PWR OK light on the front
    pwrOkTexture_->render (385, 86, 9, 9);

    // Render RUN light on the front
    if (bus_->signalIsSet (Qbus::Signal::SRUN))
        runLightTexture_->render (400, 86, 9, 9);

    //Update screen
    renderer_->update (); 
}