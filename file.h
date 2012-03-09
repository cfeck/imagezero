#ifndef FILE_H
#define FILE_H 1

class InputFile
{
public:
    InputFile(const char *filename);
    ~InputFile();

    bool isReadable() const;
    const unsigned char *data() const;
    int dataSize() const;

private:
    class Private;
    Private * const d;
};

class OutputFile
{
public:
    OutputFile(const char *filename);
    ~OutputFile();

    bool isWritable() const;
    unsigned char *prepareData(int maxSize);
    void commitData(unsigned char *data, int size);

private:
    class Private;
    Private * const d;
};

#endif
