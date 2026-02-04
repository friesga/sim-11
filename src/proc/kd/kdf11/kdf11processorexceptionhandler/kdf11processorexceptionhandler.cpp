#include "kdf11processorexceptionhandler.h"

KDF11ProcessorExceptionHandler::KDF11ProcessorExceptionHandler (Bus* bus, CpuData* cpuData,
    Interfaces::CpuController* cpuController, DataPaths* dataPaths)
    :
    bus_ {bus},
    cpuData_ {cpuData},
    dataPaths_ {dataPaths}
{ }

void KDF11ProcessorExceptionHandler::serviceTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (cpuData_->trapVector ());

    // Check if a stack overflow occurred as a result of the trap. In that
    // case a stack overflow trap has to be processed first unless the
    // original trap was caused by a stack overflow in the executed
    // instruction.
    if (cpuData_->stackOverflow () &&
        !cpuData_->trapPending (CpuData::TrapType::StackOverflow))
        swapPcPSW (cpuData_->trapVector (CpuData::TrapType::StackOverflow));

    cpuData_->clearTrap ();
}

void KDF11ProcessorExceptionHandler::serviceInterrupt ()
{
    InterruptRequest intrptReq;

    if (bus_->getIntrptReq (intrptReq))
    {
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());

        // Check if a stack overflow occurred as a result of the interrupt.
        // In that case a stack overflow trap has to be processed first.
        if (cpuData_->stackOverflow ())
            swapPcPSW (cpuData_->trapVector (CpuData::TrapType::StackOverflow));
    }
}

// Swap the PC and PSW with new values from the given vector
void KDF11ProcessorExceptionHandler::swapPcPSW (u16 vectorAddress)
{
    // Save PC and PSW on the stack. 
    // Unlike the KD11-F and KD11-HA, the KDF11-AA does not enter console
    // ODT upon occurrence of a double bus error (for example, when R6 points
    // to nonexistent memory during a bus timeout trap). The KDF11-AA creates
    // a new stack at location 2 and continues to trap to 4. (EK-KDF11-UG-PR2)
    // 
    // The order of execution of the trap sequence seems to be that the new
    // PSW is loaded before the old PSW is saved onto the stack. This means
    // that if a bus error occurs on the push of the psw onto the stack the
    // new psw already is loaded. This behaviour is implied by JKDBD0
    // test 407. The same behaviour might exist for saving and retrieval of
    // the PC.
    // 
    // If a bus timeout occurs while getting an interrupt vector, the KDF11-AA
    // ignores it and continues execution of the program, whereas in such case
    // the KDll-F and KDll-HA enter console ODT. (EK-KDF11-UG-PR2)
    //
    u16 newPC, newPSW;

    // Ignore bus errors on fetching the interrupt vector.
    if (!fetchFromVector (vectorAddress, &newPC) ||
        !fetchFromVector (vectorAddress + 2, &newPSW))
        return;

    // Set new PSW before saving the old PSW
    u16 oldPSW = cpuData_->psw ();
    cpuData_->psw ().set (PSW::ProtectionMode::Trap, newPSW);

    if (!dataPaths_->pushWord (oldPSW) || !dataPaths_->pushWord (cpuData_->registers ()[7]))
    {
        // Set up new stack at location 2 and trap to address 4. The stack
        // pointer is set at location 4 as it will be decremented before the
        // PSW is pushed.
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, cpuData_->registers ()[6]);
        fetchFromVector (4, &newPC);
        cpuData_->registers ()[6] = 4;
        dataPaths_->pushWord (cpuData_->psw ());
        dataPaths_->pushWord (cpuData_->registers ()[7]);
    }

    cpuData_->registers ()[7] = newPC;
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
bool KDF11ProcessorExceptionHandler::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = dataPaths_->fetchWord (address, PSW::Mode::Kernel);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}