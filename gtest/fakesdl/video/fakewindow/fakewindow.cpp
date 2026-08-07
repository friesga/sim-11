#include "fakewindow.h"

using std::make_unique;

void FakeWindow::show () 
{
}

Panel* FakeWindow::createPanel (Cabinet::Position cabinetPosition,
    RackUnit unitHeight) 
{
    return nullptr;
}

unique_ptr<PanelBuilder> FakeWindow::createFilePanelBuilder (Cabinet::Position cabinetPosition,
    RackUnit unitHeight) 
{
    return nullptr;
}

unique_ptr<PanelBuilder> FakeWindow::createDataPanelBuilder (ImageContainer& imageContainer,
    Cabinet::Position cabinetPosition, RackUnit unitHeight) 
{
    return make_unique<FakeDataPanelBuilder> ();
}

void FakeWindow::addPanel (unique_ptr<Panel> panel) 
{
    panels_.push_back (move (panel));
}

void FakeWindow::render () 
{
}

bool FakeWindow::handleEvents () 
{
    return false;
}