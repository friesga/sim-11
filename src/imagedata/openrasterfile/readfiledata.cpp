#include "openrasterfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

using std::filesystem::file_size;
using std::filesystem::path;
using std::vector;
using std::ifstream;

// The function file_size will thrown an exception if the file with the
// given name does not exist.
//
vector<unsigned char> OpenRasterFile::readFileData (const string& name)
{
    path inputFilePath {name};
    auto length = file_size (inputFilePath);
  
    vector<unsigned char> buffer (length);
    ifstream inputFile (name, std::ios_base::binary);
    inputFile.read (reinterpret_cast<char*> (buffer.data ()), length);
    inputFile.close ();
    return buffer;
}