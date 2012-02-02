#include <cstring>

#include "libiz.h"
#include "iz_p.h"

namespace IZ {

#define decodePixel(predict)                    \
{                                               \
    Pixel<> pix, pp;                            \
                                                \
    bc.fillCache();                             \
    int nl = decodeTable[pl][bc.peekBits(MAX_CODE_LENGTH)]; \
    bc.skipBits(dCount[(pl << CONTEXT_BITS) + nl]); \
    pl = nl;                                    \
    pix.readBits(bc, nl);                       \
                                                \
    pix.toSigned();                             \
    pix.reverseTransform();                     \
    pp.predict(p, bpp, bpr);                    \
    pix += pp;                                  \
    pix.writeTo(p);                             \
    p += bpp;                                   \
}

const unsigned char *decodeImage(Image<> &im, const unsigned char *src)
{
    BitDecoder<> bc;
    bc.begin(src);

    const int bpp = 3;
    int w = bc.readValue(7) + 1;
    int h = bc.readValue(7) + 1;
    im.setWidth(w);
    im.setHeight(h);
    int size = w * h;
    unsigned char *p = new unsigned char[size * bpp];
    im.setData(p);
    const int bpr = bpp * im.width();
    im.setSamplesPerLine(bpr);
    unsigned char *pend = p + bpp * size;
    int pl = 7;

    unsigned int dCount[1 << (2 * CONTEXT_BITS)];
    memcpy(dCount, staticdCount, sizeof(dCount));

    /* first pixel in first line */
    decodePixel(predict0);
    /* remaining pixels in first line */
    const unsigned char *endline = p + bpr - bpp;
    while (p != endline) {
        decodePixel(predict1x);
    }
    while (p != pend) {
        /* first pixel in remaining lines */
        decodePixel(predict1y);
        /* remaining pixels in remaining lines */
        const unsigned char *endline = p + bpr - bpp;
        while (p != endline) {
            decodePixel(predict3);
        }
    }
    return bc.end();
}

} // namespace IZ
