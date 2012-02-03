#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstring>

#include <sys/stat.h>

#include "libiz.h"
#include "portableimage.h"

using namespace IZ;

static void decodeIZ(const char *infilename, const char *outfilename)
{
    PortableImage pi;
    int infd = ::open(infilename, O_RDONLY);
    if (infd < 0) {
        perror("Cannot open input file");
        exit(EXIT_FAILURE);
    }
    int outfd = ::open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0) {
        perror("Cannot open output file");
        exit(EXIT_FAILURE);
    }
    struct stat sb;
    fstat(infd, &sb);
    int insize = sb.st_size;
    unsigned char *src = new unsigned char[insize + 17];
    int pos = 0;
    int remaining = insize;
    while (remaining > 0) {
        int bytesRead = ::read(infd, src + pos, remaining);
        if (bytesRead < 0)
            break;
        remaining -= bytesRead;
        pos += bytesRead;
    }
    if (remaining > 0) {
        perror("Cannot read input file");
        exit(EXIT_FAILURE);
    }
    initDecodeTable();
    pi.setComponents(3);
    decodeImage(pi, src);
    pi.write(outfd);
    ::close(infd);
    ::close(outfd);
    delete src;
}

static void encodeIZ(const char *infilename, const char *outfilename)
{
    PortableImage pi;
    int infd = ::open(infilename, O_RDONLY);
    if (infd < 0) {
        perror("Cannot open input file");
        exit(EXIT_FAILURE);
    }
    int outfd = ::open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0) {
        perror("Cannot open output file");
        exit(EXIT_FAILURE);
    }
    if (!pi.read(infd)) {
        fprintf(stderr, "Cannot handle input file, only 24 bit PPM files supported.\n");
        exit(EXIT_FAILURE);
    }
    if (pi.components() != 3) {
        fprintf(stderr, "Cannot handle 8-bit (grayscale) PGM files, only 24 bit PPM files supported.\n");
        exit(EXIT_FAILURE);
    }
    if (pi.width() > 16384 || pi.height() > 16384) {
        fprintf(stderr, "Cannot handle image size %d x %d, limit is 16384 x 16384.\n", pi.width(), pi.height());
        exit(EXIT_FAILURE);
    }
    unsigned char *dest = new unsigned char[pi.height() * pi.width() * 4 + 33];
    initEncodeTable();
    ::write(outfd, dest, encodeImage(pi, dest) - dest);
    ::close(infd);
    ::close(outfd);
    delete dest;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && (!strncmp(argv[1], "-h", 2) || !strncmp(argv[1], "--h", 3))) {
        printf("Usage: %s [c|d] INPUTFILE OUTPUTFILE\n"
        "\n"
        "  --help         show this help\n"
        "  --version      show version and author information\n"
        "\n"
        "%s is a high-performance lossless RGB color image codec\n"
        "\n"
        "To compress a 24 bit binary PPM image to an IZ file, use\n"
        "   %s c input.ppm output.iz\n"
        "To decompress an IZ file to a 24 bit binary PPM image, use\n"
        "   %s d input.iz output.ppm\n", argv[0], argv[0], argv[0], argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc == 2 && (!strncmp(argv[1], "-v", 2) || !strncmp(argv[1], "--v", 3))) {
        printf("%s 0.1\n"
        "High-performance lossless RGB color image codec\n"
        "Copyright (c) 2012, Christoph Feck <christoph@maxiom.de>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [c|d] INPUTFILE OUTPUTFILE\n"
        "Use \"%s --help\" for more information\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    if (!strncmp(argv[1], "c", 1)) {
        encodeIZ(argv[2], argv[3]);
    } else if (!strncmp(argv[1], "d", 1)) {
        decodeIZ(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Usage: %s [c|d] INPUTFILE OUTPUTFILE\n"
        "Use \"%s --help\" for more information\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    return(0);
}
