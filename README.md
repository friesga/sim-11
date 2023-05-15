SIM-11
======

This is an LSI-11 system (PDP-11/03-L) simuator based on 
https://github.com/hackyourlife/lsi-11 with the following changes and
additions:

- Most parts rewriten in C++,
- Runs on both Windows and Linux,
- Added support for RL01/RL02,
- Added support for BA11-N mounting box, showing status LEDs and switches
  to control the CPU,
- Power-up and powerfail support,
- Configurable by means of an ini-file,
- Improved FIS instruction support (it is now accurate except for one-bit
  edge cases),
- Added support for DLV11-J BREAK key,
- Complete ODT support that implements the original ODT realistically,
- Added unit tests.

The simulator supports the followingh modules:
- KD11-NA (M7270) CPU
- MSV11 (M8044) 32KW RAM
- RXV21 (M8029) RX02 floppy disk controller
- RLV11/RLV12 (M8061) RL01/RL02 hard disk controller
- DLV11-J (M8043) 4x Serial line interface
- BDV11 bus terminator, bootstrap, and diagnostic ROM

This emulator passes the following XXDP diagnostics:
- VKAAC0 (Basic instruction test),
- VKABB0 (EIS instruction test),
- VKACC1 (FIS instruction test). This diagnostics produces four errors for the
  one-bit edge cases,
- VKADC1 (Trap test),
- VKAHA1 (4K system exerciser),
- VRLBC0 (RLV11 diskless controller test),
- ZRLGE0 (RLV11 controller test 1),
- ZRLHB1 (RLV11 controller test 2),
- ZRLJC0 (RL01/02 drive test 2 (Seek),
- ZRLKB3 (RL01/02 performance exerciser).

The simulator runs RT-11 V05.07 and XXDP V2.6.

Usage
-----

```
lsi-11 [-c <config-file>] [-t <tracefile>] [-l <loadfile>] [-h]
```

Options:
- `-c`: loads the configuration specified in the specified ini-file.
- `-t`: saves the complete execution trace of the LSI-11 system in the
  given file. This is useful for offline analysis.
- `-l`: loads the specified file as a program in standard absolute loader format. If it is specified, the emulator will
  load and execute it.
- `--help`: prints the program usage.

The simulator will start switched off. Put the AUX ON/OFF switch in the upper position to start
the system.

Implementation Details
----------------------

- The switch configuration on the BDV11 module can be changed in the file
  `src/bdv11.c`. The macro `BDV11_SWITCH` is the switch configuration.
- IRQs are delayed: if a device asserts an IRQ, some time passes before
  the CPU acknowledges it. This is necessary for some diagnostic tests to pass.
- The time between asserting the IRQ and the CPU acknowledging it has a
  random jitter. This is required to pass some tests which assume that
  there is always some jitter.
- The simulator is partly multi-threaded. This makes it possible that threads
  can go to sleep, waiting for events to occurr. The downside is that the threads have
  have to be synchronized. Some diagnostics also report inerrupt timing issues
  because of the lack thread scheduling.
 
