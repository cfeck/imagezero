#ifndef IZ_IZ_P_H
#define IZ_IZ_P_H 1

#include "intmacros.h"
#include "bitcoder.h"
#include "pixel.h"
#include "image.h"

namespace IZ {

const int CONTEXT_COUNT = 9;
const int CONTEXT_BITS = 4;
const int CONTEXT_SIZE_REGULAR = 1;
const int CONTEXT_SIZE_RLE = 3;

const int MAX_CODE_LENGTH = 6;
const int MAX_CODE_VALUE = (1 << MAX_CODE_LENGTH);

extern unsigned int staticdBits[1 << (2 * CONTEXT_BITS)];
extern const unsigned int staticdCount[1 << (2 * CONTEXT_BITS)];
extern char decodeTable[1 << CONTEXT_BITS][MAX_CODE_VALUE];

} // namespace IZ

#endif
