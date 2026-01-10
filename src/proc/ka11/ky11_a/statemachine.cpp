#include "ky11_a.h"

KY11_A::StateMachine::StateMachine (KY11_A* context)
    :
    context_ {context}
{
    initialTransition ();
}

// The transition from the initial state depends on the position of the
// ENABLE/HALT switch.
//
void KY11_A::StateMachine::initialTransition ()
{
    if (context_->haltEnablePosition () == KY11Console::HaltEnablePosition::Halt)
        dispatch (HALT_Pressed {});
    else
        dispatch (START_Pressed {});
}

KY11_A::State KY11_A::StateMachine::transition (Initial&&, HALT_Pressed)
{
    return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (Initial&&, START_Pressed)
{
    return ProgramOperation {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = *context_->switchRegister_;
    return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, EXAM_Pressed)
{
    *context_->dataRegister_ = 
        context_->bus_->read (BusAddress (*context_->addressRegister_,
            BusAddress::Width::_16Bit));
    return ExamineSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, DEP_Pressed)
{
    context_->bus_->writeWord (BusAddress (*context_->addressRegister_,
        BusAddress::Width::_16Bit), *context_->switchRegister_);
    *context_->dataRegister_ = *context_->switchRegister_;
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, START_Pressed)
{
    return ProgramOperation {};
} 

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, EXAM_Pressed)
{
    *context_->addressRegister_+= 2;
    *context_->dataRegister_ =
        context_->bus_->read (BusAddress (*context_->addressRegister_,
            BusAddress::Width::_16Bit));
    return ExamineSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = *context_->switchRegister_;
    return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, DEP_Pressed)
{
    context_->bus_->writeWord (BusAddress (*context_->addressRegister_,
        BusAddress::Width::_16Bit), *context_->switchRegister_);
    *context_->dataRegister_ = *context_->switchRegister_;
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, START_Pressed)
{
    return ProgramOperation {};
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, DEP_Pressed)
{
    *context_->addressRegister_+= 2;
    context_->bus_->writeWord (BusAddress (*context_->addressRegister_,
        BusAddress::Width::_16Bit), *context_->switchRegister_);
    *context_->dataRegister_ = *context_->switchRegister_;
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = *context_->switchRegister_;
    return AddressLoaded {};
}
KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, EXAM_Pressed)
{
    *context_->dataRegister_ =
        context_->bus_->read (BusAddress (*context_->addressRegister_,
            BusAddress::Width::_16Bit));
    return ExamineSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, START_Pressed)
{
    return ProgramOperation {};
}
