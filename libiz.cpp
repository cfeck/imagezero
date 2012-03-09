#include "libiz.h"
#include "encoder.h"
#include "decoder.h"

namespace IZ {

unsigned char *encodeImage(const Image<> &im, unsigned char *dest)
{
    ImageEncoder<> ic;
    ic.begin(dest);
    ic.encodeImageSize(im);
    ic.encodeImagePixels(im);
    return ic.end();
}

const unsigned char *decodeImage(Image<> &im, const unsigned char *src)
{
    const int bpp = 3;
    ImageDecoder<> ic;
    ic.begin(src);
    ic.decodeImageSize(im);
    int size = im.width() * im.height();
    unsigned char *p = new unsigned char[size * bpp];
    im.setData(p);
    ic.decodeImagePixels(im);
    return ic.end();
}

} // namespace IZ
