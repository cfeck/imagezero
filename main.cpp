#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstring>

#include "libiz.h"

using namespace IZ;

static void decodeIZ(const char *infilename, const char *outfilename)
{
    PortableImage pi;
    int infd = ::open(infilename, O_RDONLY);
    if (infd < 0) {
        fprintf(stderr, "Cannot open input file.\n");
        exit(EXIT_FAILURE);
    }
    int outfd = ::open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0) {
        fprintf(stderr, "Cannot open output file.\n");
        exit(EXIT_FAILURE);
    }
    unsigned char *src = new unsigned char[1024*1024*300];
    ::read(infd, src, 1024*1024*300);
    initDecodeTable();
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
    if (infd < 0 || pi.read(infd)) {
        if (pi.components() != 3) {
            fprintf(stderr, "Can only read 24 bit PPM files.\n");
            exit(EXIT_FAILURE);
        }
        if (pi.width() > 16384 || pi.height() > 16384) {
            fprintf(stderr, "Maximum dimension is 16384.\n");
            exit(EXIT_FAILURE);
        }
        ::close(infd);
        int outfd = ::open(outfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfd < 0) {
            fprintf(stderr, "Cannot open output file.\n");
            exit(EXIT_FAILURE);
        }
        unsigned char *dest = new unsigned char[pi.height() * pi.width() * 4];
        initEncodeTable();
        ::write(outfd, dest, encodeImage(pi, dest) - dest);
        ::close(outfd);
        delete dest;
    } else {
        fprintf(stderr, "Can only read 24 bit PPM files.\n");
        exit(EXIT_FAILURE);
    }
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
