#ifndef _KD11CONFIG_H_
#define _KD11CONFIG_H_

// This struct contains the configuration options common to the KD11-NA
// and KDF11 processors.
//
struct KD11Config
{
    // Definition of the power-up modes:
    // - Vector (mode 0)
    //   This option places the processor in a microcode sequence that
    //   fetches the contents of memory locations 24 and 26 and loads their
    //   contents into R7 and the PS, respectively.
    // - ODT (mode 1)
    //   This mode immediately places the processor in the console microcode
    //   regardless of the state of the BHALT L signal.
    // - Bootstrap (mode 2)
    //   This mode places the processor in a microcode sequence that loads a
    //   starting address of 173000 into R7 and begins program execution at
    //   this location if the BHALT L signal is not asserted.
    //   (EK-LSI-TM-003)
    //
    enum class PowerUpMode
    {
        Vector,
        ODT,
        Bootstrap
    };
};

#endif // !_KD11CONFIG_H_
