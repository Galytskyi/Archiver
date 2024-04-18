#ifndef BMPARCHFILE_H
#define BMPARCHFILE_H

#include "../ArchFile.h"

// -------------------------------------------------------------------------------------

#define BMP_FILE_EXT QString(".bmp")

// -------------------------------------------------------------------------------------

#define BMP_SIGNATURE 0x4D42

#pragma pack(push, 1)

struct BmpHeader
{
    BmpHeader() {};
    BmpHeader(uint32_t width, uint32_t height)
        : width(width), height(height) {};

    uint16_t signature = BMP_SIGNATURE;
    uint32_t fileSize = 0;
    uint16_t reserve0 = 0;
    uint16_t reserve1 = 0;
    uint32_t offsetData = 0;
    uint32_t headerSize = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

#pragma pack(pop)

// -------------------------------------------------------------------------------------

class BmpFile : public Arch::File
{
public:
    BmpFile();
    explicit BmpFile(const QString& fileName);
    BmpFile(const QString& fileName, uint32_t width, uint32_t height);
    virtual ~BmpFile();

    BmpHeader& header() { return m_header; }

    bool signatureIsOk() { return m_header.signature == BMP_SIGNATURE; }

    bool read();
    bool write(std::vector<uint8_t>& bmpData);

private:

    BmpHeader m_header;
};

// -------------------------------------------------------------------------------------

struct RawImageData
{
    RawImageData(BmpFile& bmpFile)
    {
        width = bmpFile.header().width;
        height = bmpFile.header().height;
        data = reinterpret_cast<unsigned char*>(bmpFile.data().data() + bmpFile.header().offsetData);
    }

    int width = 0;                  // image width in pixels
    int height = 0;                 // image height in pixels
    unsigned char* data = nullptr;  // Pointer to image data. data[j * width + i] is color of pixel in row j and column i.
};

// -------------------------------------------------------------------------------------

#endif // BMPARCHFILE_H
