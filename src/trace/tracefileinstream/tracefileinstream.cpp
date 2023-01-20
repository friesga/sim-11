#include "tracefileinstream.h"
#include "../tracefileoutstream/tracefileoutstream.h"

#include <string>
#include <utility>

using std::string;
using std::pair;
using std::shared_ptr;
using std::make_shared;

TracefileInStream::TracefileInStream (char const *filename)
{
	ifstream::open (filename, ifstream::binary);
	if (!ifstream::is_open())
		throw "Cannot open " +  string (filename);

	// Retrieve contents and size of the trace file header from the
	// TracefileOutStream.
	pair<const char*, size_t> header = TracefileOutStream::header ();
	header_ = header.first;
	headerSize_ = header.second;

	// Allocate a buffer to read the trace file header in 
	shared_ptr<char[]> buffer (new char[headerSize_]);

	// Read file header and check the file is a valid trace file
	ifstream::read (reinterpret_cast<char *> (buffer.get()), 
		headerSize_);

	if (static_cast<size_t> (ifstream::gcount()) != headerSize_ || 
			!isHeader (buffer))
		throw string(filename) + string (" is not a valid trace file\n");
}

bool TracefileInStream::isHeader (shared_ptr<char[]> buffer)
{
	for (size_t index = 0; index < headerSize_; ++index)
		if (buffer[index] != header_[index])
			return false;

	return true;
}