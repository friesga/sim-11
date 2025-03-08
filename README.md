SIM-11
======

This is an PDP-11 system simulator. It currently supports the following models:
- PDP-11/03,
- PDP-11/23,
- PDP-11/23 PLUS,
- PDP-11/24.
The options for these installations are defined by means of configuration files,
see the config directory. The options are described in these files.

This simulator is work in progress and additional CPU's and devices are to be added.

The simulator supports the following Qbus modules:
- KD11-NA (M7270) CPU (containing EIS and FIS support)
- KDF11-A (M8186) CPU with KTF11-A (MMU) support
- KDF11-B (M8191),
- MSV11 (M8044) 32kW RAM
- RXV21 (M8029) RX02 floppy disk controller
- RLV11/RLV12 (M8061) RL01/RL02 hard disk controller
- DLV11-J (M8043) 4x Serial line interface
- BDV11 (M8012) bus terminator, bootstrap, and diagnostic ROM

The simulator supports the following Unibus modules:
- KDF11-U (M7133) CPU,
- RL11 (M7762) RL01/RL02 hard disk controller,
- MS11-P (M8743) 512 kW memory,
- KT24 (M7134) Unibus Map Option,
- M9312 Bootstrap/Terminator Module.

This emulator passes the following XXDP diagnostics:
- VKAAC0 (11/03 CPU basic instruction test),
- VKABB0 (11/03 CPU EIS instruction test),
- VKACC1 (11/03 CPU FIS instruction test).
  This diagnostic produces four errors for the one-bit edge cases,
- VKADC1 (11/03 trap test),
- VKAHA1 (4K system exerciser),
- JKDBD0 (11/23 Basic CPU test),
- JKDAD1 (11/23 Mamory Management test),
- VMSAC0 (0-4 Megabute Memory Exerciser),
- VRLBC0 (RLV11 diskless controller test),
- ZRLGE0 (RLV11 controller test 1),
- ZRLHB1 (RLV11 controller test 2),
- ZRLJC0 (RL01/02 drive test 2 (Seek),
- ZRLKB3 (RL01/02 performance exerciser),
- VDLAB0 (DLV11-J test),
- VM8AF0 (BDV11/KDF11-B Bootstrap diagnostic),
- JKDKB0 (11/23+ CPU Cluster diagnostic),
- JDLAA0 (11/23+ SLU/DLV11-J diagnostic),
- JKDIB0 (KDF11-B 11/23+ SLU-LTC Repair),
- KKUAE0 (11/24/44 UBI MAP),
- ZMSPC0 (MS11-L/M/P MEM DIAG).

The simulator runs:
- RT-11 V05.07,
- XXDP V2.6,
- RSX-11M when configured as a PDP-11/23(+).

The simuator has the following features:
- Completely written in C++,
- Runs on both Windows and Linux,
- Support for BA11-N and BA11-L mounting boxes, showing status LEDs and switches
  to control the CPU,
- Power-up and powerfail support,
- Configurable by means of an ini-files,
- FIS instruction support (accurate except for one-bit edge cases),
- Complete Micro-ODT support that implements the original ODT realistically
  for both the KD11-NA and the reduced command set in the KDF11-A,
- Realistic timing of device actions,
- Functionality verified by unit tests.

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

The simulator will start switched off. To start the system the LOAD button of
RL drive 0 must be pushed. After a configurable time (default 10 seconds) the unit
is spun up and the CPU can be started, for the PDP-11/03, -23 and -23 PLUS by putting
the AUX ON/OFF switch in the upper position, for the 11/24 to set the switch to the
LOCAL position (click at the right of the switch). The system should boot the
XXDP monitor.

Implementation Details
----------------------

- The simulator is configured by means of a .ini file. Every section defines the
  configuration of one device, i.e. a board, a device unit or the mounting box.
  Every section contains a number of key-value pairs, defining a specific
  device setting.

 