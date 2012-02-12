#ifndef IZ_PIXEL_H
#define IZ_PIXEL_H 1

#include "intmacros.h"
#include "predict.h"

namespace IZ {

template<typename U = unsigned char>
class Pixel
{
    enum Components {
        R = 0,
        G = 1,
        B = 2,
        Count = 3,
        C0 = G,
        C1 = R,
        C2 = B
    };

public:
    void readFrom(const U *p) {
        c[0] = p[C0];
        c[1] = p[C1];
        c[2] = p[C2];
    }

    void writeTo(U *p) const {
        p[C0] = c[0];
        p[C1] = c[1];
        p[C2] = c[2];
    }

    void predict0(const U *p, int bpp, int bpr) {
        c[0] = Predictor0<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor0<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor0<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
    }

    void predict1x(const U *p, int bpp, int bpr) {
        c[0] = Predictor1x<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor1x<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor1x<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
    }

    void predict1y(const U *p, int bpp, int bpr) {
        c[0] = Predictor1y<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor1y<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor1y<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
    }

    void predict2(const U *p, int bpp, int bpr) {
        // "(x + y) / 2" (Average) predictor
        c[0] = Predictor2avg<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor2avg<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor2avg<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
    }

    void predict3(const U *p, int bpp, int bpr) {
#if 0
        // "x + y - xy" (Plane) predictor
        c[0] = Predictor3plane<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor3plane<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor3plane<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
#elif 0
        // TODO (Paeth/PNG) predictor
#elif 0
        // "Average of Average, Plane" predictor
        c[0] = Predictor3avgplane<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor3avgplane<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor3avgplane<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
#else
        // "Median of x, y, Plane" (MED) predictor
        c[0] = Predictor3med<U>::predict(p[C0 - bpp], p[C0 - bpr], p[C0 - bpp - bpr]);
        c[1] = Predictor3med<U>::predict(p[C1 - bpp], p[C1 - bpr], p[C1 - bpp - bpr]);
        c[2] = Predictor3med<U>::predict(p[C2 - bpp], p[C2 - bpr], p[C2 - bpp - bpr]);
#endif
    }

    void operator-=(const Pixel<U> &other) {
        c[0] -= other.c[0];
        c[1] -= other.c[1];
        c[2] -= other.c[2];
    }

    void operator+=(const Pixel<U> &other) {
        c[0] += other.c[0];
        c[1] += other.c[1];
        c[2] += other.c[2];
    }

    bool operator==(const Pixel<U> &other) const {
        return c[0] == other.c[0] && c[1] == other.c[2] && c[3] == other.c[3];
    }

    void forwardTransform()
    {
        c[1] -= c[0];
        c[2] -= c[0];
    }

    void reverseTransform()
    {
        c[1] += c[0];
        c[2] += c[0];
    }

    void toUnsigned() {
        if (sizeof(U) == sizeof(signed char)) {
            c[0] = s2u((signed char) c[0]);
            c[1] = s2u((signed char) c[1]);
            c[2] = s2u((signed char) c[2]);
        } else if (sizeof(U) == sizeof(signed short)) {
            c[0] = s2u((signed short) c[0]);
            c[1] = s2u((signed short) c[1]);
            c[2] = s2u((signed short) c[2]);
        } else {
            c[0] = s2u(c[0]);
            c[1] = s2u(c[1]);
            c[2] = s2u(c[2]);
        }
    }

    void toSigned() {
        c[0] = u2s(c[0]);
        c[1] = u2s(c[1]);
        c[2] = u2s(c[2]);
    }

    unsigned int numBits() const {
        return ::numBits(c[0] | c[1] | c[2]);
    }

    void writeBits(BitEncoder<> &bc, int numBits) const {
        bc.writeBits(c[0], numBits);
        bc.writeBits(c[1], numBits);
        bc.writeBits(c[2], numBits);
        bc.flushCache();
    }

    void readBits(BitDecoder<> &bc, int numBits) {
        c[0] = bc.readBits(numBits);
        c[1] = bc.readBits(numBits);
        c[2] = bc.readBits(numBits);
    }

    int c[Count];
};

} // namespace IZ

#endif
