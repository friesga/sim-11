#ifndef _MOCKWINDOW_H_
#define _MOCKWINDOW_H_

#include "panel.h"
#include "mockdatapanelbuilder.h"

#include <memory>

using std::make_unique;

class MockWindow : public Window
{
public:
    // Functions required by the Window interface
    void show () override
    {}

    Panel* createPanel (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) override
    {
        return nullptr; 
    }

    unique_ptr<PanelBuilder> createFilePanelBuilder (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) override
    {
        return nullptr;
    }

    unique_ptr<PanelBuilder> createDataPanelBuilder (ImageContainer& imageContainer,
        Cabinet::Position cabinetPosition, RackUnit unitHeight) override
    {
        return make_unique<MockDataPanelBuilder> ();
    }

    void addPanel (unique_ptr<Panel> panel) override
    {
    }
    void render () override
    { }

    bool handleEvents () override
    {
        return false;
    }
};

#endif // _MOCKWINDOW_H_