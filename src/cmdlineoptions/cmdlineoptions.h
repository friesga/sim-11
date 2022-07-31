#ifndef _CMDLINEOPTIONS_H_
#define _CMDLINEOPTIONS_H_

// Structure to encapsulate the storage and retrieval of command line
// options.
//
// The class contains public const data members referencing the actual 
// private data members. This way the private data members can be accessed as
// public data members.
//
// ToDo: readOnly, createNewFile and openExistingFile options should be
// per file
class CmdLineOptions
{
    static const char *floppy_filename_;
	static const char* load_file_;
	static bool halt_;
	static bool bootstrap_;
	static const char* trace_file_;
	static bool compress_;
	static bool exit_on_halt_;
	static bool readOnly_;
	static bool quiet_;
	static bool createNewFile_;

protected:
	// CmdLineOptions(int argc, char **argv);
	CmdLineOptions();

public:
	static CmdLineOptions &get();
	static void processOptions (int argc, char const **argv);
	static void reset ();

	const char* const &floppy_filename;
	const char*  const &load_file;
	bool const &halt;
	bool const &bootstrap;
	const char* const &trace_file;
	bool const &compress;
	bool const &exit_on_halt;
	bool const &readOnly;
	bool const &quiet;
	bool const &createNewFile;
	
	CmdLineOptions (CmdLineOptions const&) = delete;
	CmdLineOptions (CmdLineOptions &&) = delete;
	CmdLineOptions &operator= (CmdLineOptions const&) = delete;
	CmdLineOptions &operator= (CmdLineOptions &&) = delete;
};

#endif // _CMDLINEOPTIONS_H_
