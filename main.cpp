#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "libiz.h"
#include "portableimage.h"
#include "file.h"

static void decodeIZ(const char *infilename, const char *outfilename)
{
    PortableImage pi;
    InputFile infile(infilename);
    if (!infile.isReadable()) {
        perror("Cannot open input file");
        exit(EXIT_FAILURE);
    }
    OutputFile outfile(outfilename);
    if (!outfile.isWritable()) {
        perror("Cannot open output file");
        exit(EXIT_FAILURE);
    }
    const unsigned char *src = infile.data();
    IZ::initDecodeTable();
    IZ::decodeImageSize(pi, src);
    pi.setComponents(3);
    const unsigned int dataSize = pi.width() * pi.height() * pi.components();
    unsigned char *dest = outfile.prepareData(dataSize + 33);
    if (!dest) {
        perror("Cannot write output file");
        exit(EXIT_FAILURE);
    }
    pi.writeHeader(dest);
    IZ::decodeImage(pi, src);
    outfile.commitData(dest, pi.data() - dest + dataSize);
}

static void encodeIZ(const char *infilename, const char *outfilename)
{
    PortableImage pi;
    InputFile infile(infilename);
    if (!infile.isReadable()) {
        perror("Cannot open input file");
        exit(EXIT_FAILURE);
    }
    OutputFile outfile(outfilename);
    if (!outfile.isWritable()) {
        perror("Cannot open output file");
        exit(EXIT_FAILURE);
    }
    if (!pi.readHeader(infile.data())) {
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
    unsigned char *dest = outfile.prepareData(pi.height() * pi.width() * 4 + 33);
    if (!dest) {
        perror("Cannot write output file");
        exit(EXIT_FAILURE);
    }
    IZ::initEncodeTable();
    unsigned char *destEnd = IZ::encodeImage(pi, dest);
    outfile.commitData(dest, destEnd - dest);
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
    return 0;
}
