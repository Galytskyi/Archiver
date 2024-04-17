#ifndef FILESMODEL_H
#define FILESMODEL_H

// ----------------------------------------------------------------------------------------------

const char* const FilesModelColumn[] =
{
    QT_TRANSLATE_NOOP("FilesModel.h", "FileName"),
    QT_TRANSLATE_NOOP("FilesModel.h", "FileSize"),

};

const int   FILES_MODEL_COLUMN_COUNT	 = sizeof(FilesModelColumn)/sizeof(FilesModelColumn[0]);

const int   FILES_MODEL_COLUMN_LOCATION	= 0,
            FILES_MODEL_COLUMN_SIZE		= 1;

const int FilesViewWidth[FILES_MODEL_COLUMN_COUNT] =
{
    150,	// FILES_MODEL_COLUMN_LOCATION
    100,	// FILES_MODEL_COLUMN_SIZE
};

// ----------------------------------------------------------------------------------------------

class FileItem
{

public:

    FileItem() { clear(); }
    FileItem(const QString& fileName, int size) : m_fileName(fileName),  m_fileSize(size) {}
    virtual ~FileItem() {}

public:

    void virtual clear()
    {
        m_fileName = QString();
        m_fileSize = 0;
    }

    QString fileName() const { return m_fileName; }
    void setFileName(const QString& fileName) { m_fileName = fileName; }

    int fileSize() const { return m_fileSize; }
    void setFileSize(int size) { m_fileSize = size; }

    bool running() const { return m_running; }
    void setRunning(bool running) { m_running = running; }

private:

    QString m_fileName;
    int m_fileSize = 0;
    bool m_running;
};

// ----------------------------------------------------------------------------------------------

std::vector<FileItem> loadFilesList(const QString& path, const QString& filter);

// ----------------------------------------------------------------------------------------------

class FilesModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    explicit FilesModel(QObject* parent = nullptr);
    virtual ~FilesModel() override;

public:

    int count() const { return static_cast<int>(m_filesCount); }

    QString text(int row, int column, const FileItem &item) const;

    bool append(const FileItem &item);
    FileItem at(int index) const;
    void remove(const std::vector<int>& removeIndexList);

    void set(const std::vector<FileItem> &list_add);
    void clear();

private:

    mutable QMutex m_filesMutex;
    std::vector<FileItem> m_filesList;
    quint64 m_filesCount = 0;

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;

    QVariant headerData(int section,Qt::Orientation orientation, int role=Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;   // for QML
};

// ----------------------------------------------------------------------------------------------

#endif // FILESMODEL_H
