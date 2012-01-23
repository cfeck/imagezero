#ifndef PORTABLE_IMAGE_H
#define PORTABLE_IMAGE_H 1

class PortableImage
{
public:
    PortableImage();
    ~PortableImage();

public:
    bool read(int fd);
    bool write(int fd);

    int width() const { return m_width; }
    int height() const { return m_height; }
    int components() const { return m_components; }
    const unsigned char *data() const { return m_data; }

    void setWidth(int width) { m_width = width; }
    void setHeight(int height) { m_height = height; }
    void setComponents(int components) { m_components = components; }
    void setData(unsigned char *data) { m_data = data; }

private:
    bool readHeader(const unsigned char *data);
    unsigned char *writeHeader(unsigned char *data);

private:
    int m_width;
    int m_height;
    int m_components;
    int m_maxVal;
    const unsigned char *m_data;
    void *m_buffer;
    int m_bufferSize;
    int m_flags;
};

#endif
