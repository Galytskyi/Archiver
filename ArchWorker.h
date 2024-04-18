#ifndef ARCHWORKER_H
#define ARCHWORKER_H

//#include <QObject>

namespace Arch
{
    // -------------------------------------------------------------------------------------

    #define BLOCK_SIZE  sizeof(uint32_t)

    #define WHITE_BLOCK  0xFFFFFFFF
    #define BLACK_BLOCK  0x00000000

    #define BITS_OF_BYTE  8

    // -------------------------------------------------------------------------------------

    enum OperationType
    {
        Encode,
        Decode,
    };

    //
    //
    enum WorkerErrors
    {
        NoErrors,
        ErrorFileName,
        ErrorInputFile,
        ErrorOutputFile,
    };

    // -------------------------------------------------------------------------------------

    class Worker : public QObject
    {
        Q_OBJECT

    public:
        explicit Worker(QObject *parent = nullptr);
        Worker(const QString& fileName, QObject *parent = nullptr);
        virtual ~Worker() override;

        QString fileName() const { return m_fileName; }
        void setFileName(const QString& fileName) { m_fileName = fileName; }

        WorkerErrors lastError() { return m_error; }

    private:

        //
        //
        QString m_fileName;
        WorkerErrors m_error = WorkerErrors::NoErrors;

    signals:
        void finished();

    public slots:

        void encode();  // BMP to BARCH
        void decode();  // BARCH to BMP
    };

    // -------------------------------------------------------------------------------------
}

#endif // ARCHWORKER_H
