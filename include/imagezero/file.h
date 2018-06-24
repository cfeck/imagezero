#ifndef IZ_FILE_H
#define IZ_FILE_H 1

#include <cstddef>

namespace IZ
{
  class InputFile
  {
  public:
    explicit InputFile(const char* filename);

    ~InputFile();

    bool isReadable() const;

    const unsigned char* data() const;

    size_t dataSize() const;

  private:
    class Private;

    Private* const d;
  };

  class OutputFile
  {
  public:
    explicit OutputFile(const char* filename);

    ~OutputFile();

    bool isWritable() const;

    unsigned char* prepareData(size_t maxSize);

    void commitData(unsigned char* data, size_t size);

  private:
    class Private;

    Private* const d;
  };
}

#endif
