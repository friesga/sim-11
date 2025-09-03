#ifndef _PNG_H_
#define _PNG_H_

#include "imagedata/include/imagemetadata.h"

#include <vector>

using std::vector;

class PNG
{
public:
    PNG (vector<uint8_t> data_);

    bool isValid ();
    bool containsValidIHDRChunk ();
    ImageMetaData::Dimensions dimensions ();
    const vector<uint8_t>& data ();

private:
    const vector<uint8_t> data_;
};


#endif // _PNG_H