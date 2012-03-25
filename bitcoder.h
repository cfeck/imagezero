#ifndef IZ_BITCODER_H
#define IZ_BITCODER_H 1

#include "intmacros.h"

namespace IZ {

/* 64 bit unsigned datatype. On x86-32, the 64 bit datatypes are slow, so
 * use MMX if available.
 */
#if defined(__LP64__) || defined(__L64__)
typedef unsigned long U64;
typedef unsigned int U32;
#elif defined(__LLP64__) || defined(__LL64__) || defined(_WIN64)
typedef unsigned long long U64;
typedef unsigned int U32;
#elif defined(__MMX__)
#include <mmintrin.h>
typedef __m64 U64;
typedef unsigned int U32;
#define USE_MMX
#else
typedef unsigned long long U64;
typedef unsigned int U32;
#endif

template<typename Code = U32>
class BitCoderBase
{
protected:
    /**
     * Data types the bit coder uses
     *
     *  Code is used to store coding units
     *  Cache is used for the bit cache
     *
     * Usually, you are going to write single bytes for codes, so this
     * would be "unsigned char".
     * You may, however, use larger coding units to speed up the coding.
     *
     * NOTE size(CacheBits) >= 2 * sizeof(CodeBits) must be true
     */
    typedef U64 Cache;

    enum Constants {
        CodeBits = sizeof(Code) * CHAR_BIT,
        CacheBits = sizeof(Cache) * CHAR_BIT
    };

protected:
    /**
     * The number of valid bits in bit cache
     */
    unsigned int len;

    /**
     * The bit cache
     */
    Cache bitcache;
};

template<typename Code = U32>
class BitDecoder : public BitCoderBase<Code>
{
    using BitCoderBase<Code>::len;
    using BitCoderBase<Code>::bitcache;
    using BitCoderBase<Code>::CodeBits;

public:
    Code fetchCode() {
        return /*__builtin_bswap32*/(*p++);
    }

    void begin(const unsigned char *ptr) {
        p = (const Code *) ptr;
#if defined(USE_MMX)
        bitcache = _mm_cvtsi32_si64(fetchCode());
#else
        bitcache = fetchCode();
#endif
        len = CodeBits;
    }

    void fillCache() {
        if (len < CodeBits) {
#if defined(USE_MMX)
            bitcache = _mm_slli_si64(bitcache, CodeBits);
            bitcache = _mm_or_si64(bitcache, _mm_cvtsi32_si64(fetchCode()));
#else
            bitcache <<= CodeBits;
            bitcache += fetchCode();
#endif
            len += CodeBits;
        }
    }

    unsigned int peekBits(unsigned int count) const {
#if defined(USE_MMX)
        return _mm_cvtsi64_si32(_mm_srli_si64(bitcache, len - count)) & bitMask(count);
#else
        return (bitcache >> (len - count)) & bitMask(count);
#endif
    }

    void skipBits(unsigned int count) {
        len -= count;
    }

    unsigned int cachedLength() const {
        return len;
    }

    unsigned int readBits(unsigned int count) {
        len -= count;
#if defined(USE_MMX)
        return _mm_cvtsi64_si32(_mm_srli_si64(bitcache, len)) & bitMask(count);
#else
        return (bitcache >> len) & bitMask(count);
#endif
    }

    void align() {
        len = 0;
    }

    const unsigned char *end() {
#if defined(USE_MMX)
        _mm_empty();
#endif
        return (const unsigned char *) (p - (len >= CodeBits));
    }

private:
    const Code *p;
};

template<typename Code = U32>
class BitEncoder : public BitCoderBase<Code>
{
    using BitCoderBase<Code>::len;
    using BitCoderBase<Code>::bitcache;
    using BitCoderBase<Code>::CodeBits;

public:
    void storeCode(Code code) {
        *p++ = /*__builtin_bswap32*/(code);
    }

    void begin(unsigned char *ptr) {
        p = (Code *) ptr;
        len = 0;
#if defined(USE_MMX)
        bitcache = _mm_cvtsi32_si64(0);
#else
        bitcache = 0; // silence compiler
#endif
    }

    void flushCache() {
        if (len >= CodeBits) {
            len -= CodeBits;
#if defined(USE_MMX)
            storeCode(_mm_cvtsi64_si32(_mm_srli_si64(bitcache, len)));
#else
            storeCode(bitcache >> len);
#endif
        }
    }

    void writeBits(unsigned int bits, unsigned int count) {
        len += count;
#if defined(USE_MMX)
        bitcache = _mm_slli_si64(bitcache, count);
        bitcache = _mm_or_si64(bitcache, _mm_cvtsi32_si64(bits));
#else
        bitcache = (bitcache << count) + bits;
#endif
    }

    void align() {
        if (len > 0) {
#if defined(USE_MMX)
            storeCode(_mm_cvtsi64_si32(_mm_slli_si64(bitcache, CodeBits - len)));
            _mm_empty();
#else
            storeCode(bitcache << (CodeBits - len));
#endif
            len = 0;
        }
    }

    unsigned char *end() {
        align();
        return (unsigned char *) p;
    }

private:
    Code *p;
};

} // namespace IZ

#endif
