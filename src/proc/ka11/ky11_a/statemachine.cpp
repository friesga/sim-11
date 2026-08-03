#include "ky11_a.h"

KY11_A::StateMachine::StateMachine (KY11_A* context)
    :
    context_ {context}
{}


KY11_A::State KY11_A::StateMachine::transition (Off&&, BPOK_High)
{
    context_->audioPlayer_->play ({
        {"start.wav", PlaybackMode::OneShot},
        {"fans.wav", PlaybackMode::Continuous}});

    if (context_->currentHaltEnablePosition_ ==
            KY11Console::HaltEnablePosition::Enable)
        return ProgramOperation {};
    else 
        return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = context_->tempRegister_ =
        *context_->switchRegister_;
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

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&& currentState,
    START_Pressed)
{
    return startPressed (currentState);
} 

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&& currentState,
    CONT_Pressed)
{
    return contPressed (currentState);
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&& currentState,
    BPOK_Low)
{
    context_->audioPlayer_->play ({{"switch off.wav", PlaybackMode::OneShot}});
    return Off {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, EXAM_Pressed)
{
    context_->tempRegister_+= 2;
    *context_->addressRegister_ = context_->tempRegister_;
    *context_->dataRegister_ =
        context_->bus_->read (BusAddress (*context_->addressRegister_,
            BusAddress::Width::_16Bit));
    return ExamineSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = context_->tempRegister_ = 
        *context_->switchRegister_;
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
    return startPressed (AddressLoaded {});
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&& currentState,
    CONT_Pressed)
{
    return contPressed (currentState);
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&& currentState,
    BPOK_Low)
{
    context_->audioPlayer_->play ({{"switch off.wav", PlaybackMode::OneShot}});
    return Off {};
}


KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, DEP_Pressed)
{
    context_->tempRegister_+= 2;
    *context_->addressRegister_ = context_->tempRegister_;
    context_->bus_->writeWord (BusAddress (*context_->addressRegister_,
        BusAddress::Width::_16Bit), *context_->switchRegister_);
    *context_->dataRegister_ = *context_->switchRegister_;
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = context_->tempRegister_ =
        *context_->switchRegister_;
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
    return startPressed (AddressLoaded {});
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&& currentState,
    CONT_Pressed)
{
    return contPressed (currentState);
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&& currentState,
    BPOK_Low)
{
    context_->audioPlayer_->play ({{"switch off.wav", PlaybackMode::OneShot}});
    return Off {};
}

KY11_A::State KY11_A::StateMachine::transition (ProgramOperation&&, HALT_Pressed)
{
    context_->cpuController_->halt (Interfaces::CpuController::HaltReason::HaltInstruction);
    return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (ProgramOperation&&, BPOK_Low)
{
    context_->cpuController_->halt (Interfaces::CpuController::HaltReason::HaltInstruction);
    context_->audioPlayer_->play ({{"switch off.wav", PlaybackMode::OneShot}});
    return Off {};
}

// When ENABLE/HALT is set to HALT, depressing START provides a system clear
// (initialize) only. The processor does not start; the Bus Address Register is
// loaded from a temporary processor register (TEMP) which is usually pre-loaded
// by LOAD ADDR.
KY11_A::State KY11_A::StateMachine::startPressed (KY11_A::State newState)
{
    context_->bus_->BINIT ().cycle ();

    if (context_->currentHaltEnablePosition_ ==
        KY11Console::HaltEnablePosition::Enable)
    {
        context_->cpuController_->start (*context_->addressRegister_);
        context_->bus_->START ().cycle ();
        return ProgramOperation {};
    }
    else
    {
        *context_->addressRegister_ = context_->tempRegister_;
        return move (newState);
    }
}

KY11_A::State KY11_A::StateMachine::contPressed (KY11_A::State newState)
{
    context_->cpuController_->proceed ();
    context_->bus_->START ().cycle ();

    if (context_->currentHaltEnablePosition_ == 
            KY11Console::HaltEnablePosition::Enable)
        return ProgramOperation {};
    else
        return AddressLoaded {};
}