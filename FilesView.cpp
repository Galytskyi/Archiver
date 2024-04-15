#include "FilesView.h"

#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>

// -------------------------------------------------------------------------------------------------------------------

FilesModel::FilesModel(QObject*)
{
}

FilesModel::~FilesModel()
{
    QMutexLocker l(&m_filesMutex);

    m_filesList.clear();
}

int FilesModel::columnCount(const QModelIndex&) const
{
    return FILES_VIEW_COLUMN_COUNT;
}

int FilesModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_filesCount);
}

QVariant FilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	QVariant result = QVariant();

	if (orientation == Qt::Horizontal)
	{
        if (section >= 0 && section < FILES_VIEW_COLUMN_COUNT)
		{
            result = FilesViewColumn[section];
		}
	}

	if (orientation == Qt::Vertical)
	{
		result = QString("%1").arg(section + 1);
	}

	return result;
}

QVariant FilesModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid() == false)
	{
		return QVariant();
	}

	int rowIndex = index.row();
    if (rowIndex < 0 || rowIndex >= static_cast<int>(m_filesCount))
	{
		return QVariant();
	}

	int columnIndex = index.column();
    if (columnIndex < 0 || columnIndex > FILES_VIEW_COLUMN_COUNT)
	{
		return QVariant();
	}

    const FileItem& item = at(rowIndex);

	if (role == Qt::TextAlignmentRole)
	{
		int result = Qt::AlignLeft;

		switch (columnIndex)
		{
            case FILES_VIEW_COLUMN_LOCATION:	result = Qt::AlignLeft;	break;
            case FILES_VIEW_COLUMN_SIZE:		result = Qt::AlignCenter;	break;

			default:
				assert(0);
		}

		return result;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
        return text(rowIndex, columnIndex, item);
	}

	return QVariant();
}

QString FilesModel::text(int row, int column, const FileItem& item) const
{
    if (row < 0 || row >= static_cast<int>(m_filesCount))
	{
		return QString();
	}

    if (column < 0 || column > FILES_VIEW_COLUMN_COUNT)
	{
		return QString();
	}

	QString result;

	switch (column)
	{
        case FILES_VIEW_COLUMN_LOCATION:	result = item.fileName();                   break;
        case FILES_VIEW_COLUMN_SIZE:		result = QString::number(item.fileSize());  break;

		default:
			assert(0);
	}

	return result;

}

bool FilesModel::append(const FileItem& item)
{
    // append into FilesList
	//
    int indexTable = static_cast<int>(m_filesCount);

	beginInsertRows(QModelIndex(), indexTable, indexTable);

        m_filesMutex.lock();

        m_filesList.push_back(item);
        m_filesCount = m_filesList.size();

        m_filesMutex.unlock();

	endInsertRows();

	return true;
}

FileItem FilesModel::at(int index) const
{
    QMutexLocker l(&m_filesMutex);

    if (index < 0 || index >= static_cast<int>(m_filesCount))
	{
        return FileItem();
	}

    return m_filesList[static_cast<quint64>(index)];
}

void FilesModel::remove(const std::vector<int>& removeIndexList)
{
    // remove from FilesList
	//
	int count = static_cast<int>(removeIndexList.size());
	for(int index = count-1; index >= 0; index--)
	{
		int removeIndex = removeIndexList.at(static_cast<quint64>(index));

		beginRemoveRows(QModelIndex(), removeIndex, removeIndex);

            m_filesMutex.lock();

                m_filesList.erase(m_filesList.begin() + removeIndex);
                m_filesCount = m_filesList.size();

            m_filesMutex.unlock();

		endRemoveRows();
	}
}

void FilesModel::set(const std::vector<FileItem>& list_add)
{
	quint64 count = list_add.size();
	if (count == 0)
	{
		return;
	}

	beginInsertRows(QModelIndex(), 0, static_cast<int>(count - 1));

    m_filesMutex.lock();

    m_filesList = list_add;
    m_filesCount = m_filesList.size();

    m_filesMutex.unlock();

	endInsertRows();
}

void FilesModel::clear()
{
    quint64 count = m_filesCount;
	if (count == 0)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), 0, static_cast<int>(count - 1));

    m_filesMutex.lock();

    m_filesList.clear();
        m_filesCount = m_filesList.size();

    m_filesMutex.unlock();

	endRemoveRows();
}

// -------------------------------------------------------------------------------------------------------------------

FilesView::FilesView(QWidget* parent) :
	QTableView(parent)
{
	setModel(&m_model);

    connect(this, &FilesView::clicked, this, &FilesView::onListCliked, Qt::QueuedConnection);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setWordWrap(false);

	QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
	verticalHeader()->setDefaultSectionSize(cellSize.height());

    for(int column = 0; column < FILES_VIEW_COLUMN_COUNT; column++)
	{
        setColumnWidth(column, FilesViewWidth[column]);
	}

	createHeaderContextMenu();

	installEventFilter(this);

	createContextMenu();
}

FilesView::~FilesView()
{
}

void FilesView::createContextMenu()
{
	// create context menu
	//
    m_pContextMenu = new QMenu(tr("&Files"), this);

    m_pEncodeAction = m_pContextMenu->addAction(tr("&Encode ..."));
    m_pEncodeAction->setIcon(QIcon(":/icons/Encode.png"));
    m_pDecodeAction = m_pContextMenu->addAction(tr("&Decode ..."));
    m_pDecodeAction->setIcon(QIcon(":/icons/Decode.png"));

    connect(m_pEncodeAction, &QAction::triggered, this, &FilesView::onEncode, Qt::QueuedConnection);
    connect(m_pDecodeAction, &QAction::triggered, this, &FilesView::onDecode, Qt::QueuedConnection);

	// init context menu
	//
	setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested, this, &FilesView::onContextMenu, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::createHeaderContextMenu()
{
	m_headerContextMenu = new QMenu(this);
	if (m_headerContextMenu == nullptr)
	{
		return;
	}

	// init header context menu
	//
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &FilesView::onHeaderContextMenu);

    for(int column = 0; column < FILES_VIEW_COLUMN_COUNT; column++)
	{
        QAction* pAction = m_headerContextMenu->addAction(FilesViewColumn[column]);
		if (pAction == nullptr)
		{
			continue;
		}

		pAction->setCheckable(true);
		pAction->setChecked(true);

		m_pColumnActionList.push_back(pAction);
	}

    connect(m_headerContextMenu, static_cast<void (QMenu::*)(QAction*)>(&QMenu::triggered),	this, &FilesView::onColumnAction);
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::hideListColumn(int column, bool hide)
{
	int columnCount = static_cast<int>(m_pColumnActionList.size());
	if (column < 0 || column >= columnCount)
	{
		return;
	}

	if (hide == true)
	{
		hideColumn(column);
		m_pColumnActionList[static_cast<quint64>(column)]->setChecked(false);
	}
	else
	{
		showColumn(column);
		m_pColumnActionList[static_cast<quint64>(column)]->setChecked(true);
	}
}

// -------------------------------------------------------------------------------------------------------------------

int FilesView::firstVisibleColumn()
{
	int visibleColumn = 0;

	int columnCount = model()->columnCount();
	for(int column = 0; column < columnCount; column++)
	{
		if (isColumnHidden(column) == true)
		{
			continue;
		}

		visibleColumn = column;

		break;
	}

	return visibleColumn;
}

// -------------------------------------------------------------------------------------------------------------------

bool FilesView::eventFilter(QObject* object, QEvent* e)
{
	if (e->type() == QEvent::KeyRelease)
	{
		QKeyEvent* ke = static_cast<QKeyEvent* >(e);

		if (	ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
				ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown)
		{
            onListCliked(QModelIndex());
		}
	}

	return QTableView::eventFilter(object, e);
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::onFilesLoaded(const std::vector<FileItem>& list_add)
{
	m_model.clear();
	m_model.set(list_add);

	scrollToBottom();

	setCurrentIndex(model()->index(m_model.count() - 1, firstVisibleColumn()));
    onListCliked(QModelIndex());
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::onContextMenu(const QPoint&)
{
	if (m_pContextMenu == nullptr)
	{
		return;
	}

	m_pContextMenu->exec(QCursor::pos());
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::onListCliked(const QModelIndex &index)
{
	Q_UNUSED(index)

    int indexItem = currentIndex().row();
    if (indexItem < 0 || indexItem >= m_model.count())
	{
		return;
	}

    const FileItem& item = m_model.at(indexItem);

    emit fileSelected(item);
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::onHeaderContextMenu(QPoint)
{
	if (m_headerContextMenu == nullptr)
	{
		return;
	}

	m_headerContextMenu->exec(QCursor::pos());
}

// -------------------------------------------------------------------------------------------------------------------

void FilesView::onColumnAction(QAction* action)
{
	if (action == nullptr)
	{
		return;
	}

	int columnCount = static_cast<int>(m_pColumnActionList.size());
	for(int column = 0; column < columnCount; column++)
	{
		if (m_pColumnActionList[static_cast<quint64>(column)] == action)
		{
            hideListColumn(column, !action->isChecked());

			break;
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
