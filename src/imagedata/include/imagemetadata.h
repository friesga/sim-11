#ifndef _IMAGEMETADATA_H_
#define _IMAGEMETADATA_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

// This file contains the definitions of the metadata for a layered image.
namespace ImageMetaData
{
    struct Dimensions
    {
        int width, height;
    };

    struct Position
    {
        int x, y;
    };

    // The Layer description contains the source PNG file name (src), the
    // layer's name and its position (x,y) in the image.
    struct Layer
    {
        string src;
        string name;
        Position position;
        Dimensions dimensions;
    };

    struct Metadata
    {
        Dimensions imageDimensions;
        vector<Layer> layers;
    };
}

#endif // _IMAGEMETSDATA_H_
