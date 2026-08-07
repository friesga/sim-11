#ifndef _FAKEWINDOW_H_
#define _FAKEWINDOW_H_

#include "panel.h"
#include "../fakedatapanelbuilder/fakedatapanelbuilder.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class FakeWindow : public Window
{
public:
    // Functions required by the Window interface
    void show () override;
    Panel* createPanel (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) override;
    unique_ptr<PanelBuilder> createFilePanelBuilder (Cabinet::Position cabinetPosition,
        RackUnit unitHeight) override;
    unique_ptr<PanelBuilder> createDataPanelBuilder (ImageContainer& imageContainer,
        Cabinet::Position cabinetPosition, RackUnit unitHeight) override;
    void addPanel (unique_ptr<Panel> panel) override;
    void render () override;
    bool handleEvents () override;

private:
    vector<unique_ptr<Panel>> panels_;
};

#endif // _FAKEWINDOW_H_