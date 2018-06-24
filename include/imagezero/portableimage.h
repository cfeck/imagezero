#ifndef IZ_PORTABLE_IMAGE_H
#define IZ_PORTABLE_IMAGE_H 1

#include "image.h"

namespace IZ
{
  class PortableImage : public IZ::Image<>
  {
  public:
    PortableImage();

    ~PortableImage();

  public:
    int components() const
    { return m_components; }

    void setComponents(int components)
    { m_components = components; }

    bool readHeader(const unsigned char* data);

    unsigned char* writeHeader(unsigned char* data);

  private:
    int m_components;
    int m_maxVal;
  };
}

#endif
