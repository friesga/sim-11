#include "absoluteloader.h"
#include "trace/trace.h"

#include <string>
#include <sstream>

using std::string;
using std::invalid_argument;

// Load the given file in absolute loader format, returning the start address
// of the loaded binary.
// 
// Example absolute loader binary files are VKAAC0.BIC and VKADC0.BIC.
//
u16 AbsoluteLoader::loadFile (const char* fileName, u8* memory)
{
    size_t size;
    u16 length;
    u16 loadAddress;
    u8 checksum;
    size_t bytes = 0;
    u16 startAddress {0};

    FILE* f = fopen (fileName, "rb");
    if (!f)
        throw invalid_argument ("Error opening load file " + string (fileName));

    fseek (f, 0, SEEK_END);
    size = ftell (f);
    fseek (f, 0, SEEK_SET);
    fflush (stdout);
    while (bytes < size)
    {
        int c;
        while ((c = fgetc (f)) != EOF)
        {
            if (c == 1)
                break;
        }
        if (c == 1)
            c = fgetc (f);

        if (c == EOF)
        {
            fclose (f);
            throw invalid_argument ("Error: unexpected EOF in load file");
        }
        else if (c != 0)
        {
            fclose (f);
            std::stringstream errorText;
            errorText << "Error: invalid signature [0x" <<
                std::hex << c << "] in load file";
            throw invalid_argument (errorText.str ());
        }

        // Read byte count and load address from the file
        (void)!fread (&length, 2, 1, f);
        (void)!fread (&loadAddress, 2, 1, f);

        bytes += length;
        (void)!fread (&memory[loadAddress], length - 6, 1, f);
        trace.memoryDump (&memory[loadAddress], loadAddress, length - 6);
        (void)!fread (&checksum, 1, 1, f);
        if (length == 6)
        {
            if ((loadAddress & 1) == 0)
                startAddress = loadAddress;
            else
                startAddress = 0200;
            break;
        }
    }
    fclose (f);
    return startAddress;
}
