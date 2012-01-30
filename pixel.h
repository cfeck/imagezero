#ifndef IZ_PIXEL_H
#define IZ_PIXEL_H 1

#include "intmacros.h"

namespace IZ {

#define UNUSED(x) { x = x; }

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
        UNUSED(p); UNUSED(bpp); UNUSED(bpr);
//        const int v = 1 << (sizeof(U) * CHAR_BIT - 1);
        const int v = 0;
        c[0] = v;
        c[1] = v;
        c[2] = v;
    }

    void predict1x(const U *p, int bpp, int bpr) {
        UNUSED(bpr);
        readFrom(p - bpp);
    }

    void predict1y(const U *p, int bpp, int bpr) {
        UNUSED(bpp);
        readFrom(p - bpr);
    }

    void predict2(const U *p, int bpp, int bpr) {
        // "(x + y) / 2" (Average) predictor
        int x, y;

        x = p[C0 - bpp];
        y = p[C0 - bpr];
        c[0] = (x + y) >> 1;

        x = p[C1 - bpp];
        y = p[C1 - bpr];
        c[1] = (x + y) >> 1;

        x = p[C2 - bpp];
        y = p[C2 - bpr];
        c[2] = (x + y) >> 1;
    }

    void predict3(const U *p, int bpp, int bpr) {
#if 0
        // "x + y - xy" (Plane) predictor
        int x, y, xy;

        x = p[C0 - bpp];
        xy = p[C0 - bpp - bpr];
        y = p[C0 - bpr];
        c[0] = x + y - xy;

        x = p[C1 - bpp];
        xy = p[C1 - bpp - bpr];
        y = p[C1 - bpr];
        c[1] = x + y - xy;

        x = p[C2 - bpp];
        xy = p[C2 - bpp - bpr];
        y = p[C2 - bpr];
        c[2] = x + y - xy;
#elif 0
        // TODO (Paeth/PNG) predictor
#elif 0
        // "Average of Average, Plane" predictor
        int x, y, xy;

        x = p[C0 - bpp];
        xy = p[C0 - bpp - bpr];
        y = p[C0 - bpr];
        c[0] = (3 * (x + y) - 2 * xy) >> 2;

        x = p[C1 - bpp];
        xy = p[C1 - bpp - bpr];
        y = p[C1 - bpr];
        c[1] = (3 * (x + y) - 2 * xy) >> 2;

        x = p[C2 - bpp];
        xy = p[C2 - bpp - bpr];
        y = p[C2 - bpr];
        c[2] = (3 * (x + y) - 2 * xy) >> 2;
#else
        // "Median of x, y, Plane" (MED) predictor
        int x, y, xy, dx, dy, dxy, s;

        x = p[C0 - bpp];
        xy = p[C0 - bpp - bpr];
        y = p[C0 - bpr];
        dy = x - xy;
        dx = xy - y;
        dxy = x - y;
        s = oppositeSign(dy, dx);
        dxy &= oppositeSign(dxy, dy);
        c[0] = selectVal(s, y + dy, x - dxy);

        x = p[C1 - bpp];
        xy = p[C1 - bpp - bpr];
        y = p[C1 - bpr];
        dy = x - xy;
        dx = xy - y;
        dxy = x - y;
        s = oppositeSign(dy, dx);
        dxy &= oppositeSign(dxy, dy);
        c[1] = selectVal(s, y + dy, x - dxy);

        x = p[C2 - bpp];
        xy = p[C2 - bpp - bpr];
        y = p[C2 - bpr];
        dy = x - xy;
        dx = xy - y;
        dxy = x - y;
        s = oppositeSign(dy, dx);
        dxy &= oppositeSign(dxy, dy);
        c[2] = selectVal(s, y + dy, x - dxy);
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

    void writeBits(BitEncoder &bc, int numBits) const {
        bc.writeBits(c[0], numBits);
        bc.writeBits(c[1], numBits);
        bc.writeBits(c[2], numBits);
        bc.flushCache();
    }

    void readBits(BitDecoder &bc, int numBits) {
        c[0] = bc.readBits(numBits);
        c[1] = bc.readBits(numBits);
        c[2] = bc.readBits(numBits);
    }

    int c[Count];
};

} // namespace IZ

#endif
