#include "openrasterfile.h"

#include <sstream>
#include <string>

using std::istringstream;
using std::string;
using std::getline;
using std::stoi;

// This function is an extremely simple XML parser (to avoid dependency
// on a real XML parser). The function extracts the tags one by one and
// checks its contents for image and layer tags. From these tags it extracts
// the relevant attributes and fills the Metadata structure.
//
ImageMetaData::Metadata OpenRasterFile::parseStackXML ()
{
    vector<uint8_t> data = zipReader_->read ("stack.xml");
    string text {reinterpret_cast<char*> (data.data ())};

    ImageMetaData::Metadata metadata {};

    istringstream iss {text};
    string line;

    while (getline (iss, line, '>'))
    {
        if (line.find ("<image") != std::string::npos)
            metadata.imageDimensions = extractImageDimensions (line);

        if (line.find ("<layer") != std::string::npos)
            metadata.layers.push_back (extractLayer (line));
    }

    return metadata;
}

ImageMetaData::Dimensions OpenRasterFile::extractImageDimensions (const string& line)
{
    Dimensions imageDimensions {0, 0};

    imageDimensions.width = stoi (extractAttribute ("w=", line));
    imageDimensions.height = stoi (extractAttribute ("h=", line));

    return imageDimensions;
}

ImageMetaData::Layer OpenRasterFile::extractLayer (const string& line)
{
    Layer layer {};
    
    layer.src = extractAttribute ("src=", line);
    layer.name = extractAttribute ("name=", line);
    layer.position.x = stoi (extractAttribute ("x=", line));
    layer.position.y = stoi (extractAttribute ("y=", line));
    
    return layer;
}

string OpenRasterFile::extractAttribute (const string& attributeName, const string& line)
{
    auto pos = line.find (attributeName);
    if (pos != std::string::npos)
    {
        auto start = line.find ('"', pos);
        auto end   = line.find ('"', start + 1);
        return line.substr (start + 1, end - start - 1);
    }
    else
        return "";
}