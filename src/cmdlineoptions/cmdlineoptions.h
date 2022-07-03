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
    static const char *floppy_filename_;
	static const char* load_file_;
	static int halt_;
	static int bootstrap_;
	static const char* trace_file_;
	static int compress_;
	static int exit_on_halt_;
	static int readOnly_;
	static int quiet_;
	static int createNewFile_;
	static int openExistingFile_;

protected:
	// CmdLineOptions(int argc, char **argv);
	CmdLineOptions();

public:
	static CmdLineOptions &get();
	static void processOptions (int argc, char **argv);

	const char* const &floppy_filename;
	const char*  const &load_file;
	int const &halt;
	int const &bootstrap;
	const char* const &trace_file;
	int const &compress;
	int const &exit_on_halt;
	int const &readOnly;
	int const &quiet;
	int const &createNewFile;
	int const &openExistingFile;
	
	CmdLineOptions (CmdLineOptions const&) = delete;
	CmdLineOptions (CmdLineOptions &&) = delete;
	CmdLineOptions &operator= (CmdLineOptions const&) = delete;
	CmdLineOptions &operator= (CmdLineOptions &&) = delete;
};

#endif // _CMDLINEOPTIONS_H_
