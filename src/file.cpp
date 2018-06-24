#include <imagezero/file.h>

#ifdef HAVE_CONFIG_H
#include "iz_config.h"
#endif

#if defined(HAVE_MMAP)

/// POSIX implementation

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace IZ
{
  class InputFile::Private
  {
  public:
    Private() : addr(MAP_FAILED)
    {}

    void* addr;
    size_t size;
  };

  InputFile::InputFile(const char* filename)
      : d(new Private)
  {
      int fd = open(filename, O_RDONLY);
      if (fd != -1)
      {
          struct stat sb;
          fstat(fd, &sb);
          d->size = sb.st_size;
          d->addr = mmap(0, d->size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
          if (d->addr == MAP_FAILED)
          {
              d->addr = mmap(0, d->size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
              if (d->addr != MAP_FAILED)
              {
                  read(fd, d->addr, d->size);
              }
          }
          close(fd);
      }
  }

  InputFile::~InputFile()
  {
      if (d->addr != MAP_FAILED)
      {
          munmap(d->addr, d->size);
      }
      delete d;
  }

  bool InputFile::isReadable() const
  {
      return d->addr != MAP_FAILED;
  }

  const unsigned char* InputFile::data() const
  {
      return (const unsigned char*) d->addr;
  }

  size_t InputFile::dataSize() const
  {
      return d->size;
  }


  class OutputFile::Private
  {
  public:
    Private()
    {}

    int fd;
    size_t mapSize;
  };

  OutputFile::OutputFile(const char* filename)
      : d(new Private)
  {
      d->fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
  }

  OutputFile::~OutputFile()
  {
      if (d->fd != -1)
      {
          close(d->fd);
      }
      delete d;
  }

  bool OutputFile::isWritable() const
  {
      if (d->fd != -1)
      {
          return true;
      }
      return false;
  }

  unsigned char* OutputFile::prepareData(size_t maxSize)
  {
      if (d->fd != -1)
      {
          ftruncate(d->fd, maxSize);
          d->mapSize = maxSize;
          void* addr = mmap(0, d->mapSize, PROT_WRITE | PROT_READ, MAP_SHARED, d->fd, 0);
          if (addr != MAP_FAILED)
          {
              return (unsigned char*) addr;
          }
          ftruncate(d->fd, 0);
          d->mapSize = 0;
          return (unsigned char*) malloc(maxSize);
      }
      return 0;
  }

  void OutputFile::commitData(unsigned char* data, size_t size)
  {
      if (data)
      {
          if (d->mapSize)
          {
              munmap(data, d->mapSize);
              ftruncate(d->fd, size);
          }
          else
          {
              write(d->fd, data, size);
              free(data);
          }
      }
  }

#else // defined(HAVE_MMAP)

  /// STDC implementation

#include <cstdio>
#include <cstdlib>

  class InputFile::Private
  {
  public:
      Private() : buffer(0), size(0) { }

      unsigned char *buffer;
      size_t size;
  };

  InputFile::InputFile(const char *filename)
      : d(new Private)
  {
      FILE *fp = fopen(filename, "rb");
      if (fp) {
          setbuf(fp, 0);
          fseek(fp, 0, SEEK_END);
          d->size = ftell(fp);
          fseek(fp, 0, SEEK_SET);
          if (d->size > 0) {
              d->buffer = (unsigned char *) malloc(d->size);
              if (d->buffer) {
                  d->size = fread(d->buffer, 1, d->size, fp);
              }
          }
          fclose(fp);
      }
  }

  InputFile::~InputFile()
  {
      if (d->buffer) {
          free(d->buffer);
      }
      delete d;
  }

  bool InputFile::isReadable() const
  {
      return d->buffer != 0;
  }

  const unsigned char *InputFile::data() const
  {
      return d->buffer;
  }

  size_t InputFile::dataSize() const
  {
      return d->size;
  }


  class OutputFile::Private
  {
  public:
      Private() { }

      FILE *fp;
  };

  OutputFile::OutputFile(const char *filename)
      : d(new Private)
  {
      d->fp = fopen(filename, "wb");
      if (d->fp) {
          setbuf(d->fp, 0);
      }
  }

  OutputFile::~OutputFile()
  {
      if (d->fp) {
          fclose(d->fp);
      }
      delete d;
  }

  bool OutputFile::isWritable() const
  {
      if (d->fp) {
          return true;
      }
      return false;
  }

  unsigned char *OutputFile::prepareData(size_t maxSize)
  {
      return (unsigned char *) malloc(maxSize);
  }

  void OutputFile::commitData(unsigned char *data, size_t size)
  {
      if (data) {
          if (d->fp) {
              fwrite(data, 1, size, d->fp);
          }
          free(data);
      }
  }

#endif // defined(HAVE_MMAP)

}