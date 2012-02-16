#include <cstring>

#include "libiz.h"
#include "iz_p.h"

namespace IZ {

#define encodePixel(predictor)                   \
{                                                \
    Pixel<> pix, pp;                             \
                                                 \
    pix.readFrom(p);                             \
    pp.predict(p, bpp, bpr, predictor::predict); \
    pix -= pp;                                   \
    pix.forwardTransform();                      \
    p += bpp;                                    \
    pix.toUnsigned();                            \
                                                 \
    int nl = pix.numBits();                      \
    cx = (cx << CONTEXT_BITS) + nl;              \
    this->writeBits(dBits[cx & bitMask(2 * CONTEXT_BITS)], dCount[cx & bitMask(2 * CONTEXT_BITS)]); \
    pix.writeBits(*this, nl);                    \
}

template <
    int bpp = 3,
    typename Code = U32
>
class ImageEncoder : public BitEncoder<Code>
{
public:
    ImageEncoder() {
        memcpy(dBits, staticdBits, sizeof(dBits));
        memcpy(dCount, staticdCount, sizeof(dCount));
    }

    void encodeImagePixels(const Image<> &im) {
        const int bpr = im.samplesPerLine();
        const unsigned char *p = im.data();
        int size = im.width() * im.height();
        const unsigned char *pend = p + bpp * size;
        unsigned int cx = (7 << CONTEXT_BITS) + 7;

        /* first pixel in first line */
        encodePixel(Predictor0<>);
        /* remaining pixels in first line */
        const unsigned char *endline = p + bpr - bpp;
        while (p != endline) {
            encodePixel(Predictor1x<>);
        }
        while (p != pend) {
            /* first pixel in remaining lines */
            encodePixel(Predictor1y<>);
            /* remaining pixels in remaining lines */
            const unsigned char *endline = p + bpr - bpp;
            while (p != endline) {
                encodePixel(Predictor3med<>);
            }
        }
    }

    void encodeImageSize(const Image<> &im) {
        this->writeValue(im.width() - 1, 7);
        this->writeValue(im.height() - 1, 7);
    }

private:
    unsigned int dBits[1 << (2 * CONTEXT_BITS)];
    unsigned int dCount[1 << (2 * CONTEXT_BITS)];
};

unsigned char *encodeImage(const Image<> &im, unsigned char *dest)
{
    ImageEncoder<> ic;
    ic.begin(dest);
    ic.encodeImageSize(im);
    ic.encodeImagePixels(im);
    return ic.end();
}

} // namespace IZ
