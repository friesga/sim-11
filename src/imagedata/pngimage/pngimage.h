#ifndef _PNG_H_
#define _PNG_H_

#include "imagedata/include/image.h"

#include <cstdint>
#include <vector>

using std::vector;

class PNGImage : public Image
{
public:
    PNGImage (vector<uint8_t> data_);

    // Functions required by the Image interface.
    bool isValid () override;
    bool containsValidIHDRChunk () override;
    Dimensions dimensions () override;
    const vector<uint8_t>& data () override;

private:
    const vector<uint8_t> data_;
};


#endif // _PNG_H