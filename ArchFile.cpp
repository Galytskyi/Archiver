#include "ArchFile.h"

#include "ArchWorker.h"

namespace Arch
{
    File::File()
    {
    }

    File::File(const QString& fileName)
        :  m_fileName(fileName)
    {
    }

    File::~File()
    {
    }

    void File::calcAddionalParams(uint32_t width)
    {
        if (!width)
        {
            m_realRowWidth = 0;
            m_paddingSize = 0;
            m_blockCount = 0;

            return;
        }

        if (width % BLOCK_SIZE == 0)
            m_realRowWidth = width;
        else
        {
            m_paddingSize = BLOCK_SIZE - (width % BLOCK_SIZE);
            m_realRowWidth = width + m_paddingSize;
        }

        m_blockCount = m_realRowWidth / BLOCK_SIZE;
    }

    std::string removeExtension( std::string const& filename )
    {
        std::string::const_reverse_iterator pivot = std::find( filename.rbegin(), filename.rend(), '.' );
        return pivot == filename.rend() ? filename : std::string( filename.begin(), pivot.base() - 1 );
    }
}
