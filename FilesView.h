#ifndef FILESVIEW_H
#define FILESVIEW_H

#include <QTableView>
#include <QMutex>

// ----------------------------------------------------------------------------------------------

const char* const FilesViewColumn[] =
{
    QT_TRANSLATE_NOOP("FilesView.h", "File name"),
    QT_TRANSLATE_NOOP("FilesView.h", "File size"),

};

const int   FILES_VIEW_COLUMN_COUNT	 = sizeof(FilesViewColumn)/sizeof(FilesViewColumn[0]);

const int   FILES_VIEW_COLUMN_LOCATION	= 0,
            FILES_VIEW_COLUMN_SIZE		= 1;

const int FilesViewWidth[FILES_VIEW_COLUMN_COUNT] =
{
    150,	// FILES_VIEW_COLUMN_LOCATION
    100,	// FILES_VIEW_COLUMN_SIZE
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
};

// ==============================================================================================

class FilesView : public QTableView
{
	Q_OBJECT

public:

    explicit FilesView(QWidget* parent = nullptr);
    virtual ~FilesView() override;

private:

	//
	//
	QMenu* m_pContextMenu = nullptr;

    QAction* m_pEncodeAction = nullptr;
    QAction* m_pDecodeAction = nullptr;

	QMenu* m_headerContextMenu = nullptr;
	std::vector<QAction*> m_pColumnActionList;

	//
	//
    FilesModel m_model;

	void createContextMenu();

	void createHeaderContextMenu();
    void hideListColumn(int column, bool hide);
	int firstVisibleColumn();

protected:

	bool eventFilter(QObject* object, QEvent* e) override;

signals:

    void fileSelected(const FileItem& item);
    void fileEncode();
    void fileDecode();

public slots:

	//
	//
    void onFilesLoaded(const std::vector<FileItem>& list_add);

    void onEncode() { emit fileEncode(); }
    void onDecode() { emit fileDecode(); }

	// slots of menu
	//
	void onContextMenu(const QPoint&);

	// Slot of Click
	//
    void onListCliked(const QModelIndex& index);

	// slots for list header, to hide or show columns
	//
	void onHeaderContextMenu(QPoint);
	void onColumnAction(QAction* action);
};

// ==============================================================================================

#endif // FILESVIEW_H