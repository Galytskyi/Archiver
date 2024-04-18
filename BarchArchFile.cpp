#include "../BarchArchFile.h"

BarchFile::BarchFile()
{
}

BarchFile::BarchFile(const QString& fileName)
    : File(fileName)
{
}

BarchFile::BarchFile(const QString& fileName, uint32_t width, uint32_t height)
    : File(fileName)
    , m_header(width, height)
{
}

BarchFile::~BarchFile()
{
}

bool BarchFile::read()
{
    if (m_fileName.isEmpty())
        return false;

    // read banch data
    //
    QFile inputFile(m_fileName);
    if (!inputFile.open(QIODevice::ReadOnly))
        return false;

    m_data = inputFile.readAll();
    inputFile.close();

    // set header
    //
    memcpy(&m_header, m_data.data(), sizeof(BarchHeader));
    if (!signatureIsOk())
        return false;

    calcAddionalParams(m_header.width);

    return true;
}

bool BarchFile::write(const std::vector<Arch::RowData>& rowData)
{
    if (m_fileName.isEmpty())
        return false;


    QFile outputFile(m_fileName);
    if (!outputFile.open(QIODevice:: QIODevice::WriteOnly))
        return false;

    // write header
    //
    outputFile.write(reinterpret_cast<char*>(&m_header), sizeof(BarchHeader));

    // write indexes
    //
    for(auto row : rowData)
        outputFile.write(reinterpret_cast<char*>(&row.index.whole), sizeof(Arch::RowIndex));

    // write data
    //
    for(auto row : rowData)
        outputFile.write(reinterpret_cast<char*>(row.data.data()), row.data.size());


    outputFile.close();

    return true;
}
