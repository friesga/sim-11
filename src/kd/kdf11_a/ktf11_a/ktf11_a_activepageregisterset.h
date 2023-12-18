#ifndef _KTF11_A_ACTIVEPAGEREGISTERSET_H_
#define _KTF11_A_ACTIVEPAGEREGISTERSET_H_

#include "ktf11_a_activepageregister.h"

#include <array>

using std::array;

// This struct defines a set of eight Active Page Registers.
struct ActivePageRegisterSet
{
    array<ActivePageRegister, 8> activePageRegister_;

    ActivePageRegisterSet ();
    ActivePageRegisterSet (ActivePageRegisterSet const &aprSet);
};

// Definition of a default constructor as the default default constructor
// is deleted because of the definition of the copy constructor.
inline ActivePageRegisterSet::ActivePageRegisterSet ()
    :
    activePageRegister_ {}
{}

// Definition of a copy constructor to be used for copying the Active Page
// Register Set in a trace record.
inline ActivePageRegisterSet::ActivePageRegisterSet (ActivePageRegisterSet const &other)
{
    activePageRegister_ = other.activePageRegister_;
}

#endif // _KTF11_A_ACTIVEPAGEREGISTERSET_H_