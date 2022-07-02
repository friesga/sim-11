#ifndef _CMDLINEOPTIONS_H_
#define _CMDLINEOPTIONS_H_

// Structure to encapsulate the storage and retrieval of command line
// options.
// The data members are public till processing of the options is merged
// into this class.
// ToDo: Merge option processing in class CmdLineOptions
// ToDo: CmdLineOptions should be a singleton
// ToDo: Change CmdLineOptions int's to bool's
// ToDo: readOnly, createNewFile and openExistingFile options should be per file
struct CmdLineOptions
{
    const char* floppy_filename = nullptr;
	const char* load_file = nullptr;
	int halt = 0;
	int bootstrap = 0;
	const char* trace_file = nullptr;
	int compress = 0;
	int exit_on_halt = 0;
	int readOnly = 0;
	int quiet = 0;
	int createNewFile = 0;
	int openExistingFile = 0;

	CmdLineOptions(int argc, char **argv);
};


#endif // _CMDLINEOPTIONS_H_