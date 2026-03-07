#include "pngimage.h"

PNGImage::PNGImage (vector<uint8_t> data)
    :
    data_ {data}
{ }

bool PNGImage::isValid ()
{
    // Minimum number of bytes is 8 (signature) + 4 (length) + 4 (chunk type)
    // + 8 (width+height)
    if (data_.size () < 24)
        return false;

    // Check if the header is a valid PNG header
    const vector<uint8_t> expectedSignature = {0x89,'P','N','G',0x0D,0x0A,0x1A,0x0A};

    for (size_t i = 0; i < expectedSignature.size (); ++i)
    {
        if (data_[i] != expectedSignature[i])
            return false;
    };

    return true;
}

// This functions checks if the PNG image contains a IHDR chunk immediately 
// following the signature.
// Chunk layout: [4-byte length][4-byte type][data]
//
bool PNGImage::containsValidIHDRChunk ()
{
    return data_[12] == 'I' && data_[13] == 'H' &&
           data_[14] == 'D' && data_[15] == 'R';
}

Image::Dimensions PNGImage::dimensions ()
{
    auto readBigEndian32 = [&] (int offset) -> int
        {
            return (data_[offset] << 24) |
                (data_[offset + 1] << 16) |
                (data_[offset + 2] << 8) |
                (data_[offset + 3]);
        };

    // Width en Height are in the first 8 bytes of IHDR data
    int width = readBigEndian32 (16);
    int height = readBigEndian32 (20);

    return {width, height};
}

const vector<uint8_t>& PNGImage::data ()
{
    return data_;
}
