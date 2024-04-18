#ifndef ARCHFILE_H
#define ARCHFILE_H

// -------------------------------------------------------------------------------------

const char* const FilesFilters[] =
{
    "*.*",
    "*.bmp",
    "*.barch",
};

const int FilesFiltersCount = sizeof(FilesFilters)/sizeof(FilesFilters[0]);

const int   FileFilters_All		= 0,
            FileFilters_Bmp		= 1,
            FileFilters_Barch	= 2;


namespace Arch
{
    // -------------------------------------------------------------------------------------

    union RowIndex
    {
        struct
        {
            uint32_t offset = 0;
            uint32_t bitsCount = 0;
        };

        uint64_t whole;
    };

    struct RowData
    {
        RowIndex index;
        std::vector<uint8_t> data;
    };

    // -------------------------------------------------------------------------------------

    class File
    {
    public:
        File();
        explicit File(const QString& fileName);
        virtual ~File();

        const QString& fileName() const { return m_fileName; }
        void setFileName(const QString& fileName) { m_fileName = fileName; }

        QByteArray& data() { return m_data; }

        // Addional Params
        //
        void calcAddionalParams(uint32_t width);

        uint32_t realRowWidth() { return m_realRowWidth; }
        uint32_t paddingSize()  { return m_paddingSize; }
        uint32_t blockCount()  { return m_blockCount; }

    protected:

        //
        //
        QString m_fileName;
        QByteArray m_data;

        // Addional Params
        //
        uint32_t m_realRowWidth = 0;
        uint32_t m_paddingSize = 0;
        uint32_t m_blockCount = 0;
    };

    // -------------------------------------------------------------------------------------

    std::string removeExtension(std::string const& filename);

    std::string getWorkDir(int &argc, char** argv);

    // -------------------------------------------------------------------------------------
 }

#endif // ARCHFILE_H
