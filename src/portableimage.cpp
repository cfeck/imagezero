#include <imagezero/portableimage.h>

namespace IZ
{
  static unsigned char* writeValue(unsigned char* p, unsigned char whiteSpace, int value)
  {
    unsigned int v = value;
    *p++ = whiteSpace;
    bool first = true;
    unsigned int k = 10000;
    while (k > 0)
    {
      if (v >= k || !first)
      {
        *p++ = '0' + v / k;
        v %= k;
        first = false;
      }
      k /= 10;
    }
    return p;
  }

  static const unsigned char* readValue(const unsigned char* p, int& value)
  {
    // skip white space
    unsigned char c;
    while (c = *p, true)
    {
      if (c == ' ' || (c >= '\011' && c <= '\015'))
      {
        ++p;
      }
      else if (c == '#')
      {
        do
        {
          ++p;
        } while (*p != '\n');
      }
      else
      {
        break;
      }
    }
    int v = 0;
    unsigned int d;
    while (d = *p - '0', d < 10U)
    {
      ++p;
      v = 10 * v + d;
    }
    value = v;
    return p;
  }

  unsigned char* PortableImage::writeHeader(unsigned char* p)
  {
    *p++ = 'P';

    switch (m_components)
    {
      case 3:
        *p++ = '6';
        break;
      case 1:
        *p++ = '5';
        break;
      default:
        return p;
    }

    p = writeValue(p, '\n', m_width);
    p = writeValue(p, ' ', m_height);
    p = writeValue(p, '\n', 255);

    setSamplesPerLine(m_components * m_width);

    *p++ = '\n';
    m_data = p;
    return p;
  }

  bool PortableImage::readHeader(const unsigned char* p)
  {
    if (*p++ != 'P')
    {
      return false;
    }

    switch (*p++)
    {
      case '6':
        m_components = 3;
        break;
      case '5':
        m_components = 1;
        break;
      default:
        return false;
    }

    p = readValue(p, m_width);
    p = readValue(p, m_height);
    p = readValue(p, m_maxVal);

    if (m_maxVal != 255 || m_width < 1 || m_height < 1)
    {
      return false;
    }

    setSamplesPerLine(m_components * m_width);

    ++p; // skip single space
    m_data = (unsigned char*) p;
    return true;
  }

  PortableImage::PortableImage()
      : m_components(0)
  {
  }

  PortableImage::~PortableImage()
  {
  }
}