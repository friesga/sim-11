#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include <thread>

#ifdef _WIN32
#include <fcntl.h>
#include <termio.h>
#include <clock_gettime.h>
#endif

#include "trace.h"
#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "bdv11/bdv11.h"
#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "lsi11/lsi11.h"

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


int main (int argc, char** argv)
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

	const char* self = *argv;
	FILE* floppy_file;
	u8* floppy;

	const char* floppy_filename = NULL;
	const char* load_file = NULL;
	int halt = 0;
	int bootstrap = 0;
	const char* trace_file = NULL;
	int compress = 0;

	int exit_on_halt = 0;

	argc--;
	argv++;

	for (; argc; argc--, argv++)
	{
		if (!strcmp("-h", *argv))
		{
			halt = 1;
		} 
		else if(!strcmp("-b", *argv)) 
		{
			bootstrap = 1;
		}
		else if(!strcmp("-z", *argv)) 
		{
			compress = 1;
		}
		else if(!strcmp("-x", *argv))
		{
			exit_on_halt = 1;
		}
		else if(!strcmp("-l", *argv) && argc > 1) 
		{
			load_file = argv[1];
			argc--;
			argv++;
		} 
		else if(!strcmp("-f", *argv) && argc > 1) 
		{
			floppy_filename = argv[1];
			argc--;
			argv++;
		} 
		else if(!strcmp("-t", *argv) && argc > 1) 
		{
			trace_file = argv[1];
			argc--;
			argv++;
		} 
		else if(**argv != '-') 
		{
			load_file = *argv;
		} 
		else if(!strcmp("--help", *argv)) 
		{
			printf("Usage: %s [OPTIONS] [FILE]\n"
					"\n"
					"OPTIONS\n"
					"  -h              Halt CPU\n"
					"  -x              Exit on HALT\n"
					"  -b              Enter RX02 double density bootstrap program\n"
					"  -l file.bin     Load file.bin in absolute loader format\n"
					"  -f file.rx2     Load RX02 floppy image from file.rx2\n"
					"  -t file.trc     Record execution trace to file.trc\n"
					"  -z              Use delta compression fo rexecution trace\n"
					"\n"
					"The optional last argument FILE is equivalent to -f file\n", self);
			return 0;
		} 
		else 
		{
			printf("Unknown option: %s\n", *argv);
			return 1;
		}
	}

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

	if (floppy_filename) 
	{
		floppy_file = fopen (floppy_filename, "rb");
		if (!floppy_file) 
		{
			printf("Error: cannot open file %s\n", floppy_filename);
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

	if (trace_file) 
	{
		TRCINIT(trace_file);
		if (compress) 
		{
			trc.flags |= TRACE_COMPRESS;
		}
	}
	/* trc.flags |= TRACE_PRINT; */

	lsi.bus.installModule (1, &msv11);
	lsi.bus.installModule (2, &rxv21);
	lsi.bus.installModule (3, &dlv11);
	lsi.bus.installModule (4, &bdv11);
	lsi.reset ();

	if (bootstrap) 
	{
		LSI11ConsoleSendString (&dlv11, odt_input);
	}
	if (load_file) 
	{
		/* execute absolute loader binary */
		/* const char* filename = "VKAAC0.BIC"; */
		/* const char* filename = "VKADC0.BIC"; */
		const char* filename = load_file;
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

	if (halt) 
	{
		lsi.kd11.cpu().runState = 0;
	} 
	else if (!bootstrap && !halt && !load_file) 
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

		if(exit_on_halt && lsi.kd11.cpu().runState == 0) {
			/* make sure ODT finishes its prompt */
			for(i = 0; i < 32; i++)
				lsi.step();

			running = 0;
		}

		clock_gettime (CLOCK_MONOTONIC, &now);
		dt = (now.tv_sec - last.tv_sec) + 
			(now.tv_nsec - last.tv_nsec) / 1e9;
		last = now;
		bdv11.step ((float) dt);
		dlv11.step();
		rxv21.step();
	}

	free (floppy);

	printf ("\n");

	if(trace_file)
	{
		TRCFINISH();
	}

	// Wait for the consoleReader to finish
	consoleReader.join();

	return 0;
}
