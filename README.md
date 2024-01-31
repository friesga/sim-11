SIM-11
======

This is an PDP-11 system (currently PDP-11/03 and PDP-11/23) simulator.
It is work in progress and additional CPU's and devices are to be added.

The simulator supports the following modules:
- KD11-NA (M7270) CPU (containing EIS and FIS support)
- KDF11-A (M8186) CPU with KTF11-A (MMU) support
- MSV11 (M8044) 32kW RAM
- RXV21 (M8029) RX02 floppy disk controller
- RLV11/RLV12 (M8061) RL01/RL02 hard disk controller
- DLV11-J (M8043) 4x Serial line interface
- BDV11 (M8012) bus terminator, bootstrap, and diagnostic ROM

This emulator passes the following XXDP diagnostics:
- VKAAC0 (11/03 CPU basic instruction test),
- VKABB0 (11/03 CPU EIS instruction test),
- VKACC1 (11/03 CPU FIS instruction test).
  This diagnostics produces four errors for the one-bit edge cases,
- VKADC1 (11/03 trap test),
- VKAHA1 (4K system exerciser),
- JKDBD0 (11/23 Basic CPU test),
- JKDAD1 (11/23 Mamory Management test),
- VMSAC0 (0-4 Megabute Memory Exerciser),
- VRLBC0 (RLV11 diskless controller test),
- ZRLGE0 (RLV11 controller test 1),
- ZRLHB1 (RLV11 controller test 2),
- ZRLJC0 (RL01/02 drive test 2 (Seek),
- ZRLKB3 (RL01/02 performance exerciser).

The simulator runs:
- RT-11 V05.07,
- XXDP V2.6,
- RSX-11M when configured as a PDP-11/23.

The simuator is based on https://github.com/hackyourlife/lsi-11
with the following changes and additions:

- Almost completely rewritten in C++,
- Runs on both Windows and Linux,
- Added support for RL01/RL02,
- Added support for KDF11-A,
- Added support for BA11-N mounting box, showing status LEDs and switches
  to control the CPU,
- Power-up and powerfail support,
- Configurable by means of an ini-file,
- Improved FIS instruction support (it is now accurate except for one-bit
  edge cases),
- Added support for DLV11-J BREAK key,
- Complete Micro-ODT support that implements the original ODT realistically
  for both the KD11-NA and the reduced command set in the KDF11-A,
- Added unit tests.

Usage
-----

```
sim-11 [-c <config-file>] [-t <tracefile>] [-l <loadfile>] [-h]
```

Options:
- `-c`: loads the configuration specified in the specified ini-file.
- `-t`: saves the complete execution trace of the LSI-11 system in the
        given file. This is useful for offline analysis.
- `-l`: loads the specified file as a program in standard absolute loader
        format. If it is specified, the emulator will load and execute it.
- `-h`: prints the program usage.

The simulator will start switched off. Put the AUX ON/OFF switch in the upper position
to start the system.

Implementation Details
----------------------

- The simulator is configured by means of a .ini file. Every section defines the
  configuration of one device, i.e. a board, a device unit or the mounting box.
  Every section contains a number of key-value pairs, defining a specific
  device setting.

- The simulator is partly multi-threaded. This makes it possible that threads
  can go to sleep, waiting for events to occur. Some diagnostics report
  interrupt timing issues because of the lack of thread scheduling.
 