#ifndef IZ_DECODER_H
#define IZ_DECODER_H 1

#include <cstring>

#include "iz_p.h"

namespace IZ {

#define decodePixel(predictor)                  \
{                                               \
    Pixel<> pix, pp;                            \
                                                \
    this->fillCache();                          \
    int nl = decodeTable[pl][this->peekBits(MAX_CODE_LENGTH)]; \
    this->skipBits(dCount[(pl << CONTEXT_BITS) + nl]); \
    pl = nl;                                    \
    pix.readBits(*this, nl);                    \
                                                \
    pp.predict(p, bpp, bpr, predictor::predict); \
    pix.toSigned();                             \
    pix.reverseTransform();                     \
    pix += pp;                                  \
    pix.writeTo(p);                             \
    p += bpp;                                   \
}

template <
    int bpp = 3,
    typename Predictor = Predictor3avgplane<>,
    typename Code = U32
>
class ImageDecoder : public BitDecoder<Code>
{
public:
    ImageDecoder() {
        memcpy(dCount, staticdCount, sizeof(dCount));
    }

    void decodeImagePixels(Image<> &im) __attribute__((always_inline)) {
        unsigned char *p = (unsigned char *) im.data();
        const int bpr = bpp * im.width();
        im.setSamplesPerLine(bpr);
        const int size = im.width() * im.height();
        unsigned char *pend = p + bpp * size;
        int pl = 7;

        /* first pixel in first line */
        decodePixel(Predictor0<>);
        /* remaining pixels in first line */
        const unsigned char *endline = p + bpr - bpp;
        while (p != endline) {
            decodePixel(Predictor1x<>);
        }
        while (p != pend) {
            /* first pixel in remaining lines */
            decodePixel(Predictor1y<>);
            /* remaining pixels in remaining lines */
            const unsigned char *endline = p + bpr - bpp;
            while (p != endline) {
                decodePixel(Predictor);
            }
        }
    }

    void decodeImageSize(Image<> &im) {
        this->fillCache();
        int b = this->readBits(4);
        int w = this->readBits(b) + 1;
        int h = this->readBits(b) + 1;
        im.setWidth(w);
        im.setHeight(h);
    }

    void skipImageSize() {
        this->fillCache();
        int b = this->readBits(4);
        this->skipBits(2 * b);
    }

private:
    unsigned int dCount[1 << (2 * CONTEXT_BITS)];
};

} // namespace IZ

#endif
