#include "pdp11/pdp11.h"
#include "m9312/m9312.h"
#include "configdata/m9312/m9312config/m9312config.h"

#include <gtest/gtest.h>
#include <memory>

using std::shared_ptr;
using std::make_shared;

TEST (M9312Test, m93isResponsibleForCorrectAddresses)
{
    M9312Config config =
    {
        M9312Config::DiagROMType::_23_248F1,
        {M9312Config::BootROMType::_23_751A9, M9312Config::BootROMType::_23_752A9,
        M9312Config::BootROMType::_23_753A9, M9312Config::BootROMType::_23_755A9},
        0173000
    };
    
    shared_ptr<M9312Config> m9312ConfigPtr =
        make_shared<M9312Config> (config);

    Qbus bus;
    M9312 m9312 (&bus, m9312ConfigPtr);

    EXPECT_TRUE (m9312.responsible (0165000));
    EXPECT_TRUE (m9312.responsible (0165776));
    EXPECT_TRUE (m9312.responsible (0173000));
    EXPECT_TRUE (m9312.responsible (0173776));

    EXPECT_FALSE (m9312.responsible (0166000));
    EXPECT_FALSE (m9312.responsible (0174000));
}
