#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <string>
#include <sstream>

using std::string;

// Load the given file in absolute loader format
void LSI11::loadFile ()
{
    /* execute absolute loader binary */
    /* const char* filename = "VKAAC0.BIC"; */
    /* const char* filename = "VKADC0.BIC"; */
    size_t size;
    u16 len;
    u16 addr;
    u8 cksum;
    size_t bytes = 0;

    FILE* f = fopen (cmdLineOptions_.load_file, "rb");
    if (!f)
        throw "Error opening load file " + string (cmdLineOptions_.load_file);

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
            throw "Error: unexpected EOF in load file";
        }
        else if (c != 0)
        {
            fclose (f);
            std::stringstream errorText;
            errorText << "Error: invalid signature [0x" <<
                std::hex << c << "] in load file";
            throw errorText;
        }
        (void)!fread (&len, 2, 1, f);
        (void)!fread (&addr, 2, 1, f);
        bytes += len;
        (void)!fread (&msv11_->data[addr], len - 6, 1, f);
        trace.memoryDump (&msv11_->data[addr], addr, len - 6);
        (void)!fread (&cksum, 1, 1, f);
        if (len == 6)
        {
            if ((addr & 1) == 0)
            {
                kdv11_->cpu ().r[7] = addr;
                /* console->sendString("P"); */
                // ToDo: Use symbolic constants for runState
                kdv11_->cpu ().runState = 1;
                bus_.setProcessorRunning (true);
            }
            else
            {
                /* console->sendString("200G"); */
                kdv11_->cpu ().r[7] = 0200;
                kdv11_->cpu ().runState = 1;
                bus_.setProcessorRunning (true);
            }
            break;
        }
    }
    fclose (f);
}