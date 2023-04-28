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
	static const char* load_file_;
	static const char* config_file_;
	static const char* trace_file_;

public:
	CmdLineOptions(int argc, char const **argv);

	const char*  const &load_file;
	const char*  const &config_file;
	const char* const &trace_file;
};

#endif // _CMDLINEOPTIONS_H_
