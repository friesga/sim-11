#ifndef _BA11C_H_
#define _BA11C_H_

#include "bus/include/bus.h"
#include "panel.h"
#include "configdata/ba11/ba11c/ba11cconfig/ba11cconfig.h"

#include <thread>
#include <string>

class BA11_C
{
public:
    BA11_C (Bus* bus, Window* window, const BA11_CConfig& ba11cConfig);

private:
    Bus* bus_;
    Window* frontWindow_;
    
    void createBezel (Cabinet::Position cabinetPosition);
};

#endif // _BA11C_H_
