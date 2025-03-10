#include "tracefileoutstream.h"

#include <string>
#include <stdexcept>

using std::make_pair;
using std::string;
using std::to_string;
using std::invalid_argument;

TracefileOutStream::TracefileOutStream (char const *filename)
{
    open (filename);
}

TracefileOutStream::~TracefileOutStream ()
{
    close ();
}

void TracefileOutStream::open (char const *filename)
{
	// The baseFileName will be extended by a sequence number for
	// consecutive trace files.
	if (baseFileName.empty ())
        baseFileName = std::string (filename);

    ofstream::open (filename, 
        ofstream::out | ofstream::binary | ofstream::trunc);

	if (!ofstream::is_open())
		 throw invalid_argument ("Error: cannot open tracefile " +
             std::string(filename));

	// Write a header to the tracefile so it can be recognized as a valid
	// tracefile.
	ofstream::write (header_, sizeof (header_));
}

// Added a separate close function to be able to create a TracefileInstream
// in the same (unit test) function as the TracefileOutStream.
void TracefileOutStream::close ()
{
    ofstream::close ();
}

// Provide the header contents and size to TracefileInStream so it is defined
// in just one location.
pair<const char*, size_t> TracefileOutStream::header ()
{
	return make_pair (header_, sizeof (header_));
}

// Generate the trace file in chunks of 10^6 records to be able to inspect
// large traces (in the order of more than 100*10^6 records). The name of
// the new trace file is the original file name, appended with an underscore
// and a sequence number.
void TracefileOutStream::limitFileSize ()
{
    if (recordCounter_++ > 1'000'000)
    {
        // Change to next file
        ofstream::close ();
        string fileName = baseFileName + "_" + 
            to_string (fileSequenceNumber_++);

        // Use TracefileOutStream::open to open the new tracefile so it
        // will contain a trace file header and can be read separately.
        open (fileName.c_str ());
        recordCounter_ = 0;
    }
}
