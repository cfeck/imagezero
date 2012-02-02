#include <cstring>

#include "libiz.h"
#include "iz_p.h"

namespace IZ {

#define encodePixel(predict)                     \
{                                                \
    Pixel<> pix, pp;                             \
                                                 \
    pix.readFrom(p);                             \
    pp.predict(p, bpp, bpr);                     \
    pix -= pp;                                   \
    pix.forwardTransform();                      \
    p += bpp;                                    \
    pix.toUnsigned();                            \
                                                 \
    int nl = pix.numBits();                      \
    cx = (cx << CONTEXT_BITS) + nl;              \
    bc.writeBits(dBits[cx & bitMask(2 * CONTEXT_BITS)], dCount[cx & bitMask(2 * CONTEXT_BITS)]); \
    pix.writeBits(bc, nl);                       \
}

unsigned char *encodeImage(const Image<> &im, unsigned char *dest)
{
    const int bpp = 3;
    const int bpr = im.samplesPerLine();
    const unsigned char *p = im.data();
    int size = im.width() * im.height();
    const unsigned char *pend = p + bpp * size;
    unsigned int cx = (7 << CONTEXT_BITS) + 7;

    unsigned int dBits[1 << (2 * CONTEXT_BITS)];
    unsigned int dCount[1 << (2 * CONTEXT_BITS)];
    memcpy(dBits, staticdBits, sizeof(dBits));
    memcpy(dCount, staticdCount, sizeof(dCount));

    BitEncoder<> bc;
    bc.begin(dest);
    bc.writeValue(im.width() - 1, 7);
    bc.writeValue(im.height() - 1, 7);

    /* first pixel in first line */
    encodePixel(predict0);
    /* remaining pixels in first line */
    const unsigned char *endline = p + bpr - bpp;
    while (p != endline) {
        encodePixel(predict1x);
    }
    while (p != pend) {
        /* first pixel in remaining lines */
        encodePixel(predict1y);
        /* remaining pixels in remaining lines */
        const unsigned char *endline = p + bpr - bpp;
        while (p != endline) {
            encodePixel(predict3);
        }
    }
    return bc.end();
}

} // namespace IZ
