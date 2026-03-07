#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstdint>
#include <vector>

using std::vector;

// Definition of an interface for image data.
class Image
{
public:
    struct Dimensions
    {
        int width, height;
    };

    virtual ~Image () {};
    virtual bool isValid () = 0;
    virtual bool containsValidIHDRChunk () = 0;
    virtual Dimensions dimensions () = 0;
    virtual const vector<uint8_t>& data () = 0;
};

#endif // _IMAGE_H_