#include "imagedata/include/image.h"
#include "openrasterfile.h"
#include "imagedata/zipfilereader/zipfile_reader.h"
#include "imagedata/pngimage/pngimage.h"

#include <memory>
#include <stdexcept>

using std::make_unique;

// This constructor is used by the unit tests to open an OpenRaster
// file stored in a byte array.
//
OpenRasterFile::OpenRasterFile (vector<unsigned char> buffer)
    :
    buffer_ {buffer}
{
    zipReader_ = make_unique<ZipFileReader> (buffer_.data (),
        buffer_.data () + buffer_.size ());

    // Parse the stack.xml file to get metadata about the layers in the file
    metadata_ = parseStackXML ();

    // Set the dimensions of each layer by reading the PNG images
    for (auto& layer : metadata_.layers)
        layer.dimensions = getLayerDimensions (layer.src);
}

// Open the given Open Raster file (in zip-format). ReadFileData ()
// will throw a std::filesystem::filesystem_error if the file doesn't
// exist or cannot be read.
//
OpenRasterFile::OpenRasterFile (const string& fileName)
    :
    OpenRasterFile (readFileData (fileName))
{}

Image::Dimensions OpenRasterFile::imageDimensions ()
{
    return metadata_.imageDimensions;
}

// This function gets the the file name from the given layer name.
string OpenRasterFile::getFileName (string layerName)
{
    // Disclaimer: we could use std::find_if here, but that doesn't make
    // the code any clearer, so we use a simple for-loop instead.
    for (auto layer : metadata_.layers)
        if (layer.name == layerName)
            return layer.src;

    return "";
}

unique_ptr<Image> OpenRasterFile::getImage (string fileName)
{
    return make_unique<PNGImage> (zipReader_->read (fileName));
}

// This functions returns the dimensions of a PNG image in the ORA data,
// given its filename. The functions reads the PNG image from the zip-file
// and extracts the width and height from the PNG header.
Image::Dimensions OpenRasterFile::getLayerDimensions (string fileName)
{
    PNGImage png {zipReader_->read (fileName)};
    
    if (!png.isValid ())
        throw std::runtime_error ("Invalid PNG image");

    if (!png.containsValidIHDRChunk ())
        throw std::runtime_error ("IHDR chunk not found where expected");

    return png.dimensions ();
}

ImageContainer::LayerDescription OpenRasterFile::getLayerMetadata (string layerName)
{
    for (auto layer : metadata_.layers)
        if (layer.name == layerName)
            return layer;

    throw std::runtime_error ("Layer not found");
}