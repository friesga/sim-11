#ifndef _KDF11_A_CPU_H_
#define _KDF11_A_CPU_H_

#include "kd/common//decoder/decoder.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "qbus/qbus.h"
#include "float/float.h"
#include "types.h"
#include "kd/kdf11/cpudata/kdf11registers/kdf11registers.h"
#include "kd/kdf11/ktf11_a/ktf11_a.h"
#include "kd/kdf11/cpucontrol/addresstimes.h"

#include <functional>
#include <chrono>
// 
// This class contains the control functions for the KDF11-A and KDF11-B.
//
class KDF11_CpuControl : public CpuControl
{
public:
	friend class PDP_11;
	
	KDF11_CpuControl (Qbus *bus, CpuData*, MMU* mmu);

	// Definition of functions required by the CpuControl interface.
	void cpuReset () override;
	void busReset () override;
	void halt () override;
	void wait () override;
    void start (u16 address) override;
	void proceed () override;
	constexpr HaltReason haltReason ();
	bool step () override;

private:
	AddressTimes const sourceB4Group1Times
    {
        {0.0,   0.0},
        {1.425, 1.200},
        {1.425, 1.200},
        {2.850, 2.400},
        {1.725, 1.500},
        {3.150, 2.700},
        {3.150, 2.700},
        {4.575, 3.900}
    };

    AddressTimes const sourceB4Group2Times
    {
        {1.275, 1.275},
        {1.725, 1.450},
        {1.725, 1.450},
        {2.850, 2.400},
        {1.725, 1.500},
        {3.150, 2.700},
        {3.150, 2.700},
        {4.575, 3.900}
    };

    AddressTimes const destinationB5Group1Times
    {   
        {0.0,    0.0},
        {2.025,  1.800},
        {2.025,  1.800},
        {3.150,  2.700},
        {2.025,  1.950},
        {3.450,  3.000},
        {3.450,  3.000},
        {4.875,  4.500}
    };

    AddressTimes const destinationB5Group2Times
    {
        {0.0,   0.0},
        {1.725, 1.500},
        {1.725, 1.500},
        {2.850, 2.400},
        {1.725, 1.500},
        {3.150, 2.700},
        {3.150, 2.700},
        {4.575, 3.900}
    };

    AddressTimes const destinationB5Group3Times
    {
        {0.0, 0.0},
        {2.850, 2.625},
        {2.850, 2.625},
        {4.275, 3.825},
        {2.850, 2.625},
        {4.575, 4.125},
        {4.575, 4.125},
        {6.000, 5.325}
    };

    // Basic (Fetch and Execute) Times as defined in Table B-6.
    // For the SOB instruction only the No Branch time is recorded in this
    // table as a SOB instruction in most of the cases will not branch.
    AddressTimes const basicB6Group1Times  {{2.025,  1.800}};     // MOV, etc.
    AddressTimes const basicB6Group2Times  {{3.600,  3.375}};     // MTPS
    AddressTimes const basicB6Group3Times  {{4.050,  3.600}};     // MFPI, MFPD
    AddressTimes const basicB6Group4Times  {{4.725,  2.275}};     // MTPI, MTPD
    AddressTimes const basicB6Group5Times  {{2.625,  2.400}};     // SOB (NO BRANCH)
    AddressTimes const basicB6Group6Times  {{2.025,  1.800}};     // ALL BRANCHES
    AddressTimes const basicB6Group7Times  {{2.925,  2.700}};     // CLN, etc.
    AddressTimes const basicB6Group8Times  {{3.750,  3.300}};     // RTS
    AddressTimes const basicB6Group9Times  {{5.325,  4.875}};     // MARK
    AddressTimes const basicB6Group10Times {{6.225,  5.550}};     // RTI
    AddressTimes const basicB6Group11Times {{7.500,  6.825}};     // RTT
    AddressTimes const basicB6Group12Times {{10.500, 9.375}};     // IOT, BPT
    AddressTimes const basicB6Group13Times {{9.525,  8.850}};     // EMT, TRAP
    AddressTimes const basicB6Group14Times {{3.375,  3.150}};     // WAIT
    AddressTimes const basicB6Group15Times {{33.300, 33.075}};    // MUL
    AddressTimes const basicB6Group16Times {{49.650, 49.425}};    // DIV
    AddressTimes const basicB6Group17Times {{24.825, 24.600}};    // ASH
    AddressTimes const basicB6Group18Times {{46.050, 45.825}};    // ASHC

    AddressTimes const basicB7JMPTimes
    {
        {0.0,   0.0},
        {2.325, 2.100},
        {2.625, 2.400},
        {3.450, 3.000},
        {2.625, 2.400},
        {3.750, 3.300},
        {3.750, 3.300},
        {5.175, 4.500}
    };

    AddressTimes const basicB7JSRTimes
    {
        {0.0, 0.0},
        {4.350, 3.900},
        {4.650, 4.200},
        {5.475, 4.800},
        {4.650, 4.200},
        {5.775, 5.100},
        {5.775, 5.100},
        {7.200, 6.300}
    };

    // The KDF11-B manual doesn't state the instruction timing for the HALT
    // and RESET instructions but that timing isn't very relevant for us anyway.
    // For the HALT instruction we don't take any instruction time into account
    // and for the RESET instruction we'll use an average instruction time.
    AddressTimes const basicHALTTimes  {{0.0, 0.0}};
    AddressTimes const basicRESETTimes {{5.0, 4.750}};

	// Definition of CPU run states
	enum class CpuRunState
	{
		HALT,
		RUN,
		WAIT
	};

	Qbus* bus_;
	MMU* mmu_;
	CpuData* cpuData_;
	CpuRunState runState;
	Decoder decoder;
	HaltReason haltReason_;
	bool traceFlag_;

	constexpr bool inKernelMode ();
	void execInstr ();
	void serviceTrap ();
	void serviceInterrupt ();
	u8 cpuPriority ();
	void execute ();
	void swapPcPSW (u16 vectorAddress);
	bool fetchFromVector (u16 address, u16* dest);
	void traceStep ();
    double calcInstructionTime (u16 instruction);
};

 constexpr CpuControl::HaltReason KDF11_CpuControl::haltReason ()
 {
	 return haltReason_;
 }


#endif // _KDF11_A_CPU_H_