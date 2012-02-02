#ifndef PORTABLE_IMAGE_H
#define PORTABLE_IMAGE_H 1

#include "image.h"

class PortableImage : public IZ::Image<>
{
public:
    PortableImage();
    ~PortableImage();

public:
    bool read(int fd);
    bool write(int fd);

    int components() const { return m_components; }
    void setComponents(int components) { m_components = components; }

private:
    bool readHeader(const unsigned char *data);
    unsigned char *writeHeader(unsigned char *data);

private:
    int m_components;
    int m_maxVal;
    void *m_buffer;
    int m_bufferSize;
    int m_flags;
};

#endif
