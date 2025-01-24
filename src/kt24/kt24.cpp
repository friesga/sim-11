#include "kt24.h"

using std::make_shared;

KT24::KT24 (Bus* bus)
    :
    KT24 (bus, make_shared<KT24Config> ())
{}

KT24::KT24 (Bus* bus, shared_ptr<KT24Config> kt24Config)
{
    m9312_ = make_unique<M9312> (bus, kt24Config->m9312ConfigPtr);
    unibusMapLogic_ = make_unique<UnibusMapLogic> (bus, kt24Config);
    registerHandler_ = 
        make_unique<RegisterHandler> (vector<BusDevice*> {m9312_.get(), unibusMapLogic_.get()});
}

StatusCode KT24::read (BusAddress address, u16* destination)
{
    return registerHandler_->read (address, destination);
}

StatusCode KT24::writeWord (BusAddress address, u16 value)
{
    return registerHandler_->writeWord (address, value);
}

StatusCode KT24::writeByte (BusAddress address, u8 value)
{
    return registerHandler_->writeByte (address, value);
}

// For this function RegisterHandler::responsible() isn't used as it tests
// if the given address in in the I/O page and the M9312 needs responsibility
// for the powerfail vector.
bool KT24::responsible (BusAddress address)
{
    return m9312_->responsible (address) || 
        unibusMapLogic_->responsible (address);
}

void KT24::reset ()
{
    return registerHandler_->reset ();
}

// Functions required for the UnibusMap interface
BusAddress KT24::physicalAddressFrom18bitAddress (BusAddress address)
{
    return unibusMapLogic_->physicalAddressFrom18bitAddress (address);
}

void KT24::setControlLines (u16 controlLines)
{
    return unibusMapLogic_->setControlLines (controlLines);
}

// The enable() and disable() functions are added to be able to control
// the KT24 state from the unit tests.
void KT24::enable ()
{
    return unibusMapLogic_->enable ();
}

void KT24::disable ()
{
    return unibusMapLogic_->disable ();
}
