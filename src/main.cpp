#define _POSIX_C_SOURCE 200809L

#include "trace/trace.h"
#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "lsi11/lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "configurator/configprocessor/configprocessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include <iostream>
#include <thread>

#ifdef _WIN32
#include <fcntl.h>
#include <termio.h>
#include <clock_gettime.h>
#endif

/* #define DEBUG */

volatile int running;

/* RX02 double density bootstrap */
const char* odt_input =
	"2000/012701\r"
	"2002/177170\r"
	"2004/012700\r"
	"2006/100240\r"
	"2010/005002\r"
	"2012/012705\r"
	"2014/000200\r"
	"2016/012704\r"
	"2020/000401\r"
	"2022/012703\r"
	"2024/177172\r"
	"2026/030011\r"
	"2030/001776\r"
	"2032/100440\r"
	"2034/012711\r"
	"2036/000407\r"
	"2040/030011\r"
	"2042/001776\r"
	"2044/100433\r"
	"2046/110413\r"
	"2050/000304\r"
	"2052/030011\r"
	"2054/001776\r"
	"2056/110413\r"
	"2060/000304\r"
	"2062/030011\r"
	"2064/001776\r"
	"2066/100422\r"
	"2070/012711\r"
	"2072/000403\r"
	"2074/030011\r"
	"2076/001776\r"
	"2100/100415\r"
	"2102/010513\r"
	"2104/030011\r"
	"2106/001776\r"
	"2110/100411\r"
	"2112/010213\r"
	"2114/060502\r"
	"2116/060502\r"
	"2120/122424\r"
	"2122/120427\r"
	"2124/000007\r"
	"2126/003737\r"
	"2130/005000\r"
	"2132/005007\r"
	"2134/000000\r"
	"2000G";

extern void readStdin(DLV11J &dlv11);

void LSI11ConsoleSend (DLV11J* dlv, const char c)
{
	dlv->send (3, c);
}

void LSI11ConsoleSendString (DLV11J* dlv, const char* s)
{
	for(; *s; s++)
	{
		LSI11ConsoleSend(dlv, *s);
	}
}

#define	READ(addr)			lsi.bus.read(lsi.bus.user, (addr))
#define	WRITE(addr, val)	lsi.bus.write(lsi.bus.user, (addr), (val))

int main (int argc, char const **argv)
try
{
	// ToDo: Replace module configuration by table
	LSI11 lsi;
	MSV11D msv11;
	RXV21 rxv21;
	DLV11J dlv11;
	BDV11 bdv11;
	// BA11_N ba11_n;

	struct timespec now;
	struct timespec last;

	FILE* floppy_file;
	u8* floppy;

	// Get command line options
	CmdLineOptions::processOptions (argc, argv);

	// Allocate memory for the floppy (RXV21) data, open the file with
	// the floppy drive data (if given) and pass the data buffer address
	// to the RXV21 controller.
	// Note that the complete contents of the file is read into the data
	// buffer.
	floppy = (u8*) malloc(77 * 26 * 256);
	if (!floppy) 
	{
		printf("Error: cannot allocate memory\n");
		return 1;
	}

	if (CmdLineOptions::get().floppy_filename) 
	{
		floppy_file = fopen (CmdLineOptions::get().floppy_filename, "rb");
		if (!floppy_file) 
		{
			printf("Error: cannot open file %s\n", 
				CmdLineOptions::get().floppy_filename);
			free(floppy);
			return 1;
		}
		(void) !fread (floppy, 77 * 26 * 256, 1, floppy_file);
		fclose (floppy_file);
	} 
	else 
	{
		memset (floppy, 0, 77 * 26 * 256);
	}

	rxv21.setData (floppy);

	if (CmdLineOptions::get().trace_file) 
	{
		TRCINIT(CmdLineOptions::get().trace_file);
		if (CmdLineOptions::get().compress) 
		{
			trc.flags |= TRACE_COMPRESS;
		}
	}

	// To enable debug print outs set TRACE_PRINT flag in trc.flags
	// trc.flags |= TRACE_PRINT;

	// Load device configuration
	DeviceConfig *deviceConfig {nullptr};
	ConfigProcessor configProcessor;

	if (CmdLineOptions::get().config_file)
	{
		iniparser::File ft;
		if (!ft.load (CmdLineOptions::get().config_file))
		{
			std::cout << "Error: cannot open file " << 
				CmdLineOptions::get().config_file << '\n';
			return 1;
		}

		try
		{
			configProcessor.process (ft);
		}
		catch (std::invalid_argument const &except)
		{
			std::cout << "Error in configuration file: " << 
				except.what() << '\n';
			return 1;
		}
	}

	// Retrieve the configuration as specified in the configuration file
	deviceConfig = configProcessor.getConfig ();

	RLV12 rlv12 (deviceConfig->rlConfig->address, deviceConfig->rlConfig->vector,
		deviceConfig->rlConfig->RLV11, deviceConfig->rlConfig->numUnits);

	// Attach files to the RL units
	for (size_t unitNumber = 0; 
		unitNumber < deviceConfig->rlConfig->numUnits; ++unitNumber)
	{
		RlUnitConfig rlUnitConfig = 
			deviceConfig->rlConfig->rlUnitConfig[unitNumber];
		if (!rlUnitConfig.fileName.empty())
		{
			Bitmask<AttachFlags> attachFlags {AttachFlags::Default};

			if (rlUnitConfig.readOnly)
				attachFlags |= AttachFlags::ReadOnly;
			if (rlUnitConfig.newFile) 
				attachFlags |= AttachFlags::NewFile;
			if (rlUnitConfig.overwrite)
				attachFlags |= AttachFlags::Overwrite;

			rlv12.unit(unitNumber)->attach (rlUnitConfig.fileName, 
				attachFlags);
		}
	}

	lsi.bus.installModule (1, &msv11);
	lsi.bus.installModule (2, &rlv12);
	lsi.bus.installModule (3, &rxv21);
	lsi.bus.installModule (4, &dlv11);
	lsi.bus.installModule (5, &bdv11);
	lsi.reset ();

	if (CmdLineOptions::get().bootstrap) 
	{
		LSI11ConsoleSendString (&dlv11, odt_input);
	}
	if (CmdLineOptions::get().load_file) 
	{
		/* execute absolute loader binary */
		/* const char* filename = "VKAAC0.BIC"; */
		/* const char* filename = "VKADC0.BIC"; */
		const char* filename = CmdLineOptions::get().load_file;
		size_t size;
		u16 len;
		u16 addr;
		u8 cksum;
		size_t bytes = 0;

		FILE* f = fopen (filename, "rb");
		if (!f) 
		{
			printf("error opening file!\n");
			return 1;
		}
		fseek (f, 0, SEEK_END);
		size = ftell (f);
		fseek (f, 0, SEEK_SET);
		printf ("loading %s [%zu bytes]...\n", filename, size);
		fflush (stdout);
		while (bytes < size) 
		{
			int c;
			while ((c = fgetc (f)) != EOF)
			{
				if (c == 1) 
				{
					break;
				}
			}
			if (c == 1) 
			{
				c = fgetc (f);
			}
			if (c == EOF) 
			{
				printf ("error: unexpected EOF\n");
				fclose (f);
				free (floppy);
				return 1;
			} 
			else if (c != 0) 
			{
				printf ("error: invalid signature! [%02x]\n", c);
				fclose (f);
				free (floppy);
				return 1;
			}
			(void) !fread (&len, 2, 1, f);
			(void) !fread (&addr, 2, 1, f);
			bytes += len;
			printf ("%06o: %d bytes\n", addr, len - 6);
			(void) !fread (&msv11.data[addr], len - 6, 1, f);
			TRCMemoryDump (&msv11.data[addr], addr, len - 6);
			(void) !fread (&cksum, 1, 1, f);
			if(len == 6)
			{
				if ((addr & 1) == 0) 
				{
					lsi.kd11.cpu().r[7] = addr;
					/* LSI11ConsoleSendString(&dlv11, "P"); */
					// ToDo: Use symbolic constants for runState
					lsi.kd11.cpu().runState = 1;
				} 
				else 
				{
					/* LSI11ConsoleSendString(&dlv11, "200G"); */
					lsi.kd11.cpu().r[7] = 0200;
					lsi.kd11.cpu().runState = 1;
				}
				break;
			}
		}
		fclose (f);
	}


	running = 1;

	if (CmdLineOptions::get().halt) 
	{
		lsi.kd11.cpu().runState = 0;
	} 
	else if (!CmdLineOptions::get().bootstrap && 
		!CmdLineOptions::get().halt && !CmdLineOptions::get().load_file) 
	{
		lsi.kd11.cpu().runState = 1;
	}

	clock_gettime (CLOCK_MONOTONIC, &last);

	// The consoleReader reads characters and sends them to the dlv11
	std::thread consoleReader(readStdin, std::ref(dlv11));
	
	while (running) 
	{
		unsigned int i;
		double dt;
		
		for(i = 0; i < 1000; i++)
			lsi.step ();

		if (CmdLineOptions::get().exit_on_halt && lsi.kd11.cpu().runState == 0)
		{
			/* make sure ODT finishes its prompt */
			for(i = 0; i < 32; i++)
				lsi.step();

			running = 0;
		}

		dlv11.step();
		rxv21.step();
	}

	free (floppy);

	printf ("\n");

	if(CmdLineOptions::get().trace_file)
	{
		TRCFINISH();
	}

	// Wait for the consoleReader to finish
	consoleReader.join();

	return 0;
}
catch (std::string msg)
{
	std::cerr << msg << '\n';
	return 1;
}
