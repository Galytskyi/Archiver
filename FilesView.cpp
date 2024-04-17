#include "FilesView.h"

FilesView::FilesView(QWidget* parent) :
    QTableView(parent)
{
    setModel(&m_model);

    connect(this, &FilesView::clicked, this, &FilesView::onListCliked, Qt::QueuedConnection);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setWordWrap(false);

    QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
    verticalHeader()->setDefaultSectionSize(cellSize.height());

    for(int column = 0; column < FILES_MODEL_COLUMN_COUNT; column++)
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

    for(int column = 0; column < FILES_MODEL_COLUMN_COUNT; column++)
    {
        QAction* pAction = m_headerContextMenu->addAction(FilesModelColumn[column]);
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

void FilesView::onFilesLoaded(const std::vector<FileItem>& list_add)
{
    m_model.clear();
    m_model.set(list_add);

    scrollToBottom();

    setCurrentIndex(model()->index(m_model.count() - 1, firstVisibleColumn()));
    onListCliked(QModelIndex());
}

void FilesView::onContextMenu(const QPoint&)
{
    if (m_pContextMenu == nullptr)
    {
        return;
    }

    m_pContextMenu->exec(QCursor::pos());
}

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

void FilesView::onHeaderContextMenu(QPoint)
{
    if (m_headerContextMenu == nullptr)
    {
        return;
    }

    m_headerContextMenu->exec(QCursor::pos());
}

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
