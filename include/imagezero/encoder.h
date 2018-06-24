#ifndef IZ_ENCODER_H
#define IZ_ENCODER_H 1

#include <cstring>

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
    typename Predictor = Predictor3avgplane<>,
    typename Code = U32
>
class ImageEncoder : public BitEncoder<Code>
{
public:
    ImageEncoder() {
        memcpy(dBits, staticdBits, sizeof(dBits));
        memcpy(dCount, staticdCount, sizeof(dCount));
    }

    void encodeImagePixels(const Image<> &im) __attribute__((always_inline)) {
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
                encodePixel(Predictor);
            }
        }
    }

    void encodeImageSize(const Image<> &im) {
        int w = im.width() - 1;
        int h = im.height() - 1;
        int b = IZ::numBits(w | h);
        this->writeBits(b, 4);
        this->writeBits(w, b);
        this->writeBits(h, b);
        this->flushCache();
    }

private:
    unsigned int dBits[1 << (2 * CONTEXT_BITS)];
    unsigned int dCount[1 << (2 * CONTEXT_BITS)];
};

} // namespace IZ

#endif
