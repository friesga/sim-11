#ifndef _DATAREGISTER_H_
#define _DATAREGISTER_H_

#include "panel.h"
#include "types.h"

#include <utility>
#include <string>

using std::pair;
using std::string;

// The Data Register displays the information within the data paths of the
// processor.
//
// Data is normally loaded into the DATA register (data paths) by setting the
// data value into the Switch Register and lifting the DEP switch.
//
// When the console switches are used, information shown on the DATA display
// is as follows:
// 
// LOAD ADDRS - no indication
// 
// DEP - the Switch Register information just deposited. Note that the data
// and address [are] correlated. The address is where this data was stored.
// 
// EXAM - the information from the address examined. - note address and data
// correlation.
// 
// S-INST - when stepping through a program a single instruction at a time,
// there is no indication on the DATA display.
// 
// S-CYCLE - the information last in the data paths (refer to flow diagram).
// Usually is a derivative of last bus data.
// 
// During HALT and WAIT instructions, information shown on the DATA display is as follows :
// 
// WAIT - the RUN light is on, no indication on the DATA display.
// 
// HALT - when bus control is transferred to to the console on a HALT
// instruction, processor register R0 is displayed. This allows program
// identification of halts.
// 
// During DMA operations, the processor is not involved in data transfer
// functions. Therefore, the data displayed in the DATA display is not that
// of the last bus operation.
// 
// Source: DEC-11-HR1B-D, Table 3-1.
//
class DataRegister
{
public:
    DataRegister(unique_ptr<PanelBuilder>& panelBuilder);
    void operator= (u16 const value);

private:
    static const size_t numberOfIndicators = 16;

    array<Indicator*, numberOfIndicators> dataIndicators_ {};

    using ImageNames = pair<string, string>;

    array<ImageNames, numberOfIndicators> indicatorNames_ =
    {{
        {"data_00_off", "data_00_on"},
        {"data_01_off", "data_01_on"},
        {"data_02_off", "data_02_on"},
        {"data_03_off", "data_03_on"},
        {"data_04_off", "data_04_on"},
        {"data_05_off", "data_05_on"},
        {"data_06_off", "data_06_on"},
        {"data_07_off", "data_07_on"},
        {"data_08_off", "data_08_on"},
        {"data_09_off", "data_09_on"},
        {"data_10_off", "data_10_on"},
        {"data_11_off", "data_11_on"},
        {"data_12_off", "data_12_on"},
        {"data_13_off", "data_13_on"},
        {"data_14_off", "data_14_on"},
        {"data_15_off", "data_15_on"},
    }};

    void createDataRegisterIndicators (unique_ptr<PanelBuilder>& panelBuilder);
    void setIndicators (u16 value);

    template <size_t indicatorIndex>
    void createDataRegisterIndicator (unique_ptr<PanelBuilder>& panelBuilder);
};

template <size_t indicatorIndex>
void DataRegister::createDataRegisterIndicator (unique_ptr<PanelBuilder>& panelBuilder)
{
    dataIndicators_[indicatorIndex] = panelBuilder->createIndicator (
        indicatorNames_[indicatorIndex].first,
        indicatorNames_[indicatorIndex].second,
        Indicator::State::Off);
}

#endif // _DATAREGISTER_H_