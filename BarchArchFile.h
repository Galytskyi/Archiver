#ifndef BARCHARCHFILE_H
#define BARCHARCHFILE_H

#include "ArchFile.h"

// -------------------------------------------------------------------------------------

#define BARCH_FILE_EXT QString(".barch")

// -------------------------------------------------------------------------------------

#define BARCH_SIGNATURE 0x4142

#pragma pack(push, 1)

struct BarchHeader
{
    BarchHeader() {};
    BarchHeader(uint32_t width, uint32_t height)
        : width(width), height(height) {};

    uint16_t signature = BARCH_SIGNATURE;
    uint32_t width = 0;
    uint32_t height = 0;
};

#pragma pack(pop)

// -------------------------------------------------------------------------------------

class BarchFile : public Arch::File
{
public:
    BarchFile();
    explicit BarchFile(const QString& fileName);
    BarchFile(const QString& fileName, uint32_t width, uint32_t height);
    virtual ~BarchFile();

    BarchHeader& header() { return m_header; }

    bool signatureIsOk() { return m_header.signature == BARCH_SIGNATURE; }

    bool read();
    bool write(const std::vector<Arch::RowData>& rowData);

private:

    BarchHeader m_header;
};

// -------------------------------------------------------------------------------------

#endif // BARCHARCHFILE_H
