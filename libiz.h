#ifndef IZ_LIBIZ_H
#define IZ_LIBIZ_H 1

#include "portableimage.h"

namespace IZ {

void initDecodeTable(void);
void initEncodeTable(void);

const unsigned char *decodeImage(PortableImage &pi, const unsigned char *src);
unsigned char *encodeImage(const PortableImage &pi, unsigned char *dest);

} // namespace IZ

#endif
