# ImageZero

ImageZero is a high-performance lossless RGB color image
compressor/decompressor.

Compared to PNG, it compresses 24 bit PPM files about 20 times
faster, and decompresses to PPM about 2 times faster, while the
compression rate is nearly as good as PNG for natural photos.
ImageZero is not suited for “flat” (computer generated) graphics or
grayscale images.

The code is “early work in progress”, and neither the image format,
nor the library API are fully outlined yet.

To contact the author, see the AUTHORS file.
Any feedback is appreciated.

This code was originally found here:
https://github.com/cfeck/imagezero

It has been modified to build a shared library as part of a catkin package.