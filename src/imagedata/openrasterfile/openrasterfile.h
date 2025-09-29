#ifndef _OPENRASTER_H_
#define _OPENRASTER_H_

#include "imagedata/include/image.h"
#include "imagedata/include/imagecontainer.h"
#include "imagedata/zipfilereader/zipfile_reader.h"

#include <string>
#include <memory>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;

// This class opens an OpenRaster file and provides access to its layers
// and metadata.
//
// OpenRaster is a file format proposed for the common exchange of layered
// images between raster graphics editors (https://en.wikipedia.org/wiki/OpenRaster)
//
// An OpenRaster file is a ZIP-compressed archive containing a number of PNG
// images plus an XML file (stack.xml) describing the layers and other metadata.
// See https://www.openraster.org/baseline/file-layout-spec.html for details.
//
class OpenRasterFile : public ImageContainer
{
public:
    OpenRasterFile (vector<unsigned char> buffer);
    OpenRasterFile (const string& fileName);

    // Functions required by the ImageContainer interface.
    Image::Dimensions imageDimensions () override;
    string getFileName (string layerName) override;
    unique_ptr<Image> getImage (string fileName) override;
    LayerDescription getLayerMetadata (string layerName) override;

private:
    vector<unsigned char> buffer_;
    unique_ptr<ZipFileReader> zipReader_;
    Metadata metadata_;

    vector<unsigned char> readFileData (const string& name);
    Metadata parseStackXML ();
    Image::Dimensions extractImageDimensions (const string& line);
    LayerDescription extractLayer (const string& line);
    string extractAttribute (const string& attributeName, const string& line);
    Image::Dimensions getLayerDimensions (string fileName);
};

#endif // _OPENRASTER_H_