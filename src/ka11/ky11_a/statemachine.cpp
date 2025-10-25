#include "ky11_a.h"

KY11_A::StateMachine::StateMachine (KY11_A* context)
    :
    context_ {context}
{ }


KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, LOAD_ADDR_Pressed)
{
    *context_->addressRegister_ = *context_->switchRegister_;
    return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, EXAM_Pressed)
{
    return ExamineSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (AddressLoaded&&, DEP_Pressed)
{
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, EXAM_Pressed)
{
    return ExamineSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, LOAD_ADDR_Pressed)
{
    return AddressLoaded {};
}

KY11_A::State KY11_A::StateMachine::transition (ExamineSequence&&, DEP_Pressed)
{
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, DEP_Pressed)
{
    return DepositSequence {};
}

KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, LOAD_ADDR_Pressed)
{
    return AddressLoaded {};
}
KY11_A::State KY11_A::StateMachine::transition (DepositSequence&&, EXAM_Pressed)
{
    return ExamineSequence {};
}
