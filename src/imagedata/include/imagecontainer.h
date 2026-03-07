#ifndef _IMAGECONTAINER_H_
#define _IMAGECONTAINER_H_

#include"image.h"

#include <vector>
#include <string>
#include <memory>

using std::vector;
using std::string;
using std::unique_ptr;

// Definition of an interface for a container of images with support for
// layers and metadata.
class ImageContainer
{
public:
    struct Position
    {
        int x, y;
    };

    // The Layer description contains the source PNG file name (src), the
    // layer's name and its position (x,y) in the image.
    struct LayerDescription
    {
        string src;
        string name;
        Position position;
        Image::Dimensions dimensions;
    };

    struct Metadata
    {
        Image::Dimensions imageDimensions;
        vector<LayerDescription> layers;
    };

    virtual ~ImageContainer () {};
    virtual Image::Dimensions imageDimensions () = 0;
    virtual string getFileName (string layerName) = 0;
    virtual unique_ptr<Image> getImage (string fileName) = 0;
    virtual LayerDescription getLayerMetadata (string layerName) = 0;
};

#endif // _IMAGECONTAINER_H_
