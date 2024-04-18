#include "../BmpArchFile.h"

#include <QFile>

BmpFile::BmpFile()
{
}

BmpFile::BmpFile(const QString& fileName)
    :  File(fileName)
{
}

BmpFile::BmpFile(const QString& fileName, uint32_t width, uint32_t height)
    : File(fileName)
    , m_header(width, height)
{
}

BmpFile::~BmpFile()
{
}

bool BmpFile::read()
{
    if (m_fileName.isEmpty())
        return false;

    // read bmp data
    //
    QFile inputFile(m_fileName);
    if (!inputFile.open(QIODevice::ReadOnly))
        return false;

    m_data = inputFile.readAll();
    inputFile.close();

    // set header
    //
    memcpy(&m_header, m_data.data(), sizeof(BmpHeader));
    if (!signatureIsOk())
        return false;

    calcAddionalParams(m_header.width);

    return true;
}

bool BmpFile::write(std::vector<uint8_t>& bmpData)
{
    // read bin header for all bmp
    //
    QFile bmpHeaderFile(":/bin/bmpHeader.bin");
    if (!bmpHeaderFile.open(QIODevice::ReadOnly))
        return false;

    QByteArray bmpHeaderData = bmpHeaderFile.readAll();
    bmpHeaderFile.close();

    // correct size of bmp
    //
    BmpHeader* bmpHeader = reinterpret_cast<BmpHeader*>(bmpHeaderData.data());
    bmpHeader->width = m_header.width;
    bmpHeader->height = m_header.height;

    //
    //
    if (m_fileName.isEmpty())
        return false;

    QFile outputFile(m_fileName);
    if (!outputFile.open(QIODevice:: QIODevice::WriteOnly))
        return false;

    outputFile.write(reinterpret_cast<char*>(bmpHeaderData.data()), bmpHeaderData.size());
    outputFile.write(reinterpret_cast<char*>(bmpData.data()), bmpData.size());

    outputFile.close();

    return true;
}

