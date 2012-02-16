#include <cstring>

#include "libiz.h"
#include "iz_p.h"

namespace IZ {

#define decodePixel(predictor)                  \
{                                               \
    Pixel<> pix, pp;                            \
                                                \
    pp.predict(p, bpp, bpr, predictor::predict); \
    this->fillCache();                          \
    int nl = decodeTable[pl][this->peekBits(MAX_CODE_LENGTH)]; \
    this->skipBits(dCount[(pl << CONTEXT_BITS) + nl]); \
    pl = nl;                                    \
    pix.readBits(*this, nl);                    \
                                                \
    pix.toSigned();                             \
    pix.reverseTransform();                     \
    pix += pp;                                  \
    pix.writeTo(p);                             \
    p += bpp;                                   \
}

template <
    int bpp = 3,
    typename Predictor = Predictor3med<>,
    typename Code = U32
>
class ImageDecoder : public BitDecoder<Code>
{
public:
    ImageDecoder() {
        memcpy(dCount, staticdCount, sizeof(dCount));
    }

    void decodeImagePixels(Image<> &im) {
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
        int w = this->readValue(7) + 1;
        int h = this->readValue(7) + 1;
        im.setWidth(w);
        im.setHeight(h);
    }

private:
    unsigned int dCount[1 << (2 * CONTEXT_BITS)];
};

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
