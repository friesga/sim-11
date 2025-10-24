#include "ky11_a.h"

KY11_A::StateMachine (KY11_A* context)
    :
    context_ {context}
{ }


KY11_A::State KY11_A::transition (AddressLoaded&&, LOAD_ADDR_Pressed)
{
    return AddressLoaded {};
}

KY11_A::State KY11_A::transition (AddressLoaded&&, EXAM_Pressed)
{
    return ExamineSequence {};
}

KY11_A::State KY11_A::transition (AddressLoaded&&, DEP_Pressed)
{
    return DepositSequence {};
}

KY11_A::State KY11_A::transition (ExamineSequence&&, EXAM_Pressed)
{
    return ExamineSequence {};
}

KY11_A::State KY11_A::transition (ExamineSequence&&, LOAD_ADDR_Pressed)
{
    return AddressLoaded {};
}

KY11_A::State KY11_A::transition (ExamineSequence&&, DEP_Pressed)
{
    return DepositSequence {};
}

KY11_A::State KY11_A::transition (DepositSequence&&, DEP_Pressed)
{
    return DepositSequence {};
}

KY11_A::State KY11_A::transition (DepositSequence&&, LOAD_ADDR_Pressed)
{
    return AddressLoaded {};
}
KY11_A::State KY11_A::transition (DepositSequence&&, EXAM_Pressed)
{
    return ExamineSequence {};
}