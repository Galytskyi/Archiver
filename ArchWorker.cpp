#include "ArchWorker.h"

#include "BmpArchFile.h"
#include "BarchArchFile.h"


namespace Arch
{
    Worker::Worker(QObject *parent) : QObject{parent}
    {
    }

    Worker::Worker(const QString& fileName, QObject *parent) : QObject{parent} , m_fileName(fileName)
    {
    }

    Worker::~Worker()
    {
    }

    // BMP to BARCH
    //
    void Worker::encode()
    {
        if (m_fileName.isEmpty())
        {
            m_error = WorkerErrors::ErrorFileName;
            emit finished();
            return;
        }

        BmpFile bmpFile(m_fileName);
        if (!bmpFile.read())
        {
            m_error = WorkerErrors::ErrorInputFile;
            emit finished();
            return;
        }

        RawImageData rid(bmpFile);

        // in bytes in the output file
        //
        uint32_t offsetIndex = 0;

        std::vector<RowData> rowsData;
        rowsData.reserve(rid.height);

        for(int row = 0 ; row < rid.height; row++)
        {
            if (bmpFile.paddingSize())
                memset(rid.data + rid.width, 0xFF, bmpFile.paddingSize());

            std::vector<uint8_t> bits;
            uint32_t whiteBlockCount = 0;

            for(uint i = 0 ; i < bmpFile.blockCount(); i++)
            {
                uint32_t* blockData = reinterpret_cast<uint32_t*>(rid.data);

                switch (*blockData)
                {
                    case WHITE_BLOCK:
                        bits.push_back(0);
                        rid.data += BLOCK_SIZE;
                        whiteBlockCount++;
                        break;

                    case BLACK_BLOCK:
                        bits.push_back(1);
                        bits.push_back(0);
                        rid.data += BLOCK_SIZE;
                        break;

                    default:

                        bits.push_back(1);
                        bits.push_back(1);

                        for(uint byte = 0; byte < BLOCK_SIZE; byte++)
                        {
                            for(uint bit = 0; bit < BITS_OF_BYTE; bit++)
                            {
                                if (*rid.data & (0x1 << bit))
                                {
                                    bits.push_back(1);
                                }
                                else
                                {
                                    bits.push_back(0);
                                }
                            }

                            rid.data++;
                        }

                        break;
                }
            }


            RowData rd;

            if (whiteBlockCount == bmpFile.blockCount())
            {
                // is empty line - whtie line
                //
                rd.index.offset = offsetIndex++;
                rd.index.bitsCount = 1;
                rd.data.push_back({1});
            }
            else
            {
                //
                //
                rd.index.offset = offsetIndex;
                rd.index.bitsCount = bits.size();

                uint8_t byte = 0;
                uint8_t bitIndex = 0;

                for(const auto& bit: bits)
                {
                    if (bit)
                        byte |= (0x1 << bitIndex);

                    bitIndex++;

                    if (bitIndex == BITS_OF_BYTE)
                    {
                        rd.data.push_back(byte);
                        offsetIndex++;

                        byte = 0;
                        bitIndex = 0;
                    }
                }

                if (bitIndex)
                {
                    rd.data.push_back(byte);
                    offsetIndex++;
                }
            }

            rowsData.push_back(rd);
        }

        QString outputFileName = removeExtension(m_fileName.toStdString()).c_str() + BARCH_FILE_EXT;

        BarchFile barchFile(outputFileName, rid.width, rid.height);
        if (!barchFile.write(rowsData))
        {
            m_error = WorkerErrors::ErrorOutputFile;
            emit finished();
            return;
        }

        m_error = WorkerErrors::NoErrors;
        emit finished();
    }

    // BARCH to BMP
    //
    void Worker::decode()
    {
        if (m_fileName.isEmpty())
        {
            m_error = WorkerErrors::ErrorFileName;
            emit finished();
            return;
        }

        // read barch File
        //
        BarchFile barchFile(m_fileName);
        if (!barchFile.read())
        {
            m_error = WorkerErrors::ErrorInputFile;
            emit finished();
            return;
        }

        //
        //
        const int offsetIndexs = sizeof(BarchHeader);
        RowIndex* rowIndex = reinterpret_cast<RowIndex*>(barchFile.data().data() + offsetIndexs);

        const int offsetData = offsetIndexs + (barchFile.header().height * sizeof(RowIndex));
        uint8_t* rowData = reinterpret_cast<uint8_t*>(barchFile.data().data() + offsetData);

        //
        //
        std::vector<RowData> rowsData;

        for (uint r = 0; r < barchFile.header().height; r++)
        {
            RowData rd;

            rd.index.whole = rowIndex->whole;

            int bytesCount = 0;

            if (rowIndex->bitsCount % BITS_OF_BYTE == 0)
                bytesCount = rowIndex->bitsCount;
            else
                bytesCount = BITS_OF_BYTE - (rowIndex->bitsCount % BITS_OF_BYTE) + rowIndex->bitsCount;

            bytesCount /= BITS_OF_BYTE;

            if (!bytesCount)
                continue;

            rd.data.reserve(bytesCount);

            for(int b = 0; b < bytesCount; b++)
                rd.data.push_back(rowData[rowIndex->offset + b]);

            rowsData.push_back(rd);

            rowIndex++;
        }

        //
        //
        std::vector<uint8_t> bmpData;

        const int rowCount = rowsData.size();
        for (int r = 0; r < rowCount; r++)
        {
            RowData& rd = rowsData[r];

            if (rd.index.bitsCount == 1 &&rd.data.size() == 1 && rd.data.front() == 1)
            {
                // is empty line - whtie line
                //
                for (uint b = 0; b < barchFile.realRowWidth(); b++)
                    bmpData.push_back(0xFF);
            }
            else
            {
                uint bitsCount = 0;
                std::vector<uint8_t> bits;
                bits.reserve(rd.index.bitsCount);

                const int byteCount = rd.data.size();
                for (int byte = 0; byte < byteCount; byte++)
                {
                    for (int bit = 0; bit < BITS_OF_BYTE; bit++)
                    {
                        if (bitsCount == rd.index.bitsCount)
                            break;

                        if (rd.data[byte] & (0x1 << bit))
                            bits.push_back({1});
                        else
                            bits.push_back({0});

                        bitsCount++;
                    }
                }

                uint bit = 0;
                bitsCount = bits.size();
                while(bit < bitsCount)
                {
                    if (bits[bit] == 0)
                    {
                        // 4 bytes - white
                        //
                        bit++; // 0

                        for(uint b = 0; b < BLOCK_SIZE; b++)
                            bmpData.push_back(0xFF);
                    }
                    else
                    {
                        if (bits[bit + 1] == 0)
                        {
                            // 4 bytes - black
                            //
                            bit++; // 1
                            bit++; // 0

                            for(uint b = 0; b < BLOCK_SIZE; b++)
                                bmpData.push_back(0x00);

                        }
                        else
                        {
                            // 4 bytes - different
                            //
                            bit++; // 1
                            bit++; // 1

                            for(uint b = 0; b < BLOCK_SIZE; b++)
                            {
                                uint8_t byte = 0;

                                for(uint ib = 0; ib < BITS_OF_BYTE; ib++)
                                {
                                    if (bits[bit])
                                        byte |= (0x1 << ib);

                                    bit++;
                                }

                                bmpData.push_back(byte);
                            }
                        }
                    }
                }
            }
        }


        QString outputFileName = removeExtension(m_fileName.toStdString()).c_str() + BMP_FILE_EXT;

        BmpFile bmpfile(outputFileName, barchFile.header().width, barchFile.header().height);
        if (!bmpfile.write(bmpData))
        {
            m_error = WorkerErrors::ErrorOutputFile;
            emit finished();
            return;
        }

        m_error = WorkerErrors::NoErrors;
        emit finished();
    }

}
