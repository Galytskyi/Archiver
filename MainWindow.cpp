#include "MainWindow.h"

#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableView>
#include <QDockWidget>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFile>


MainWindow::MainWindow(const QString& path, QWidget *parent)
    : QMainWindow(parent)
    , m_path(path)
{

    // init interface
    //
    createInterface();

    //
    //
    loadFilesList();
}

MainWindow::~MainWindow()
{
}

bool MainWindow::createInterface()
{
    setWindowTitle(tr("Archiver"));
    setWindowIcon(QIcon(":/icons/Archiver.ico"));

    QRect screen = QGuiApplication::primaryScreen()->availableGeometry();
    resize(static_cast<int>(screen.width() * 0.5), static_cast<int>(screen.height() * 0.5));
    move(screen.center() - rect().center());

    createActions();
    createMenu();
    createToolBars();
    createFilesView();
    createStatusBar();

    loadSettings();

    return true;
}

void MainWindow::createActions()
{
    // File
    //
    m_pEncodeAction = new QAction(tr("&Encode (Bmp to Barch) ..."), this);
    m_pEncodeAction->setShortcut(QKeySequence{Qt::CTRL | Qt::Key_E});
    m_pEncodeAction->setIcon(QIcon(":/icons/Encode.png"));
    m_pEncodeAction->setToolTip(tr("Encode file"));
    connect(m_pEncodeAction, &QAction::triggered, this, &MainWindow::onEncode);

    m_pDecodeAction = new QAction(tr("&Decode (Barch to Bmp) ..."), this);
    m_pDecodeAction->setShortcut(QKeySequence{Qt::CTRL | Qt::Key_D});
    m_pDecodeAction->setIcon(QIcon(":/icons/Decode.png"));
    m_pDecodeAction->setToolTip(tr("Decode file"));
    connect(m_pDecodeAction, &QAction::triggered, this, &MainWindow::onDecode);

    // ?
    //
    m_pAboutQtAction = new QAction(tr("About Qt ..."), this);
    m_pAboutQtAction->setIcon(QIcon(":/icons/About.png"));
    m_pAboutQtAction->setToolTip(tr("Show Qt information"));
    connect(m_pAboutQtAction, &QAction::triggered, this, &MainWindow::OnAboutQt);

    m_pDocumentationAction = new QAction(tr("Documentation ..."), this);
    m_pDocumentationAction->setIcon(QIcon(":/icons/About.png"));
    m_pDocumentationAction->setToolTip(tr("Show documentation"));
    connect(m_pDocumentationAction, &QAction::triggered, this, &MainWindow::onDocumentation);
}

void MainWindow::createMenu()
{
    QMenuBar* pMenuBar = menuBar();
    if (pMenuBar == nullptr)
    {
        return;
    }

    // File
    //
    m_pFileMenu = pMenuBar->addMenu(tr("&File"));

    m_pFileMenu->addAction(m_pEncodeAction);
    m_pFileMenu->addAction(m_pDecodeAction);

    // ?
    //
    m_pInfoMenu = pMenuBar->addMenu(tr("&?"));

    m_pInfoMenu->addAction(m_pAboutQtAction);
    m_pInfoMenu->addAction(m_pDocumentationAction);
}


bool MainWindow::createToolBars()
{
    // Control panel
    //
    m_pControlToolBar = new QToolBar(this);
    if (m_pControlToolBar != nullptr)
    {
        m_pControlToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
        m_pControlToolBar->setWindowTitle(tr("Control panel"));
        m_pControlToolBar->setObjectName(m_pControlToolBar->windowTitle());
        addToolBarBreak(Qt::TopToolBarArea);
        addToolBar(m_pControlToolBar);

        m_pControlToolBar->addAction(m_pEncodeAction);
        m_pControlToolBar->addAction(m_pDecodeAction);
        m_pControlToolBar->addSeparator();

        QLabel* pFilterLabel = new QLabel(m_pControlToolBar);
        m_pFilterList = new QComboBox(m_pControlToolBar);

        if (m_pFilterList != nullptr)
        {
            m_pControlToolBar->addWidget(pFilterLabel);
            m_pControlToolBar->addWidget(m_pFilterList);

            pFilterLabel->setText(tr(" Filter "));
            pFilterLabel->setEnabled(false);

            for(int t = 0; t < FilesFiltersCount; t++)
            {
                m_pFilterList->addItem(FilesFilters[t]);
            }

            connect(m_pFilterList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                    this, &MainWindow::onFilesFilter);
        }
    }

    return true;
}

void MainWindow::createFilesView()
{
    m_filesView = new FilesView(this);
    if (m_filesView == nullptr)
    {
        return;
    }

    m_filesView->setFrameStyle(QFrame::NoFrame);

    //
    //
    connect(this, &MainWindow::filesLoaded, m_filesView, &FilesView::onFilesLoaded, Qt::QueuedConnection);

    //connect(m_pDecodeAction, &QAction::triggered, m_filesView, &FilesView::onDecode, Qt::QueuedConnection);

    //connect(m_filesView, &FilesView::removeFromBase, &m_filesBase, &FilesBase::removeFromBase, Qt::QueuedConnection);

    //connect(&m_filesBase, &FilesBase::filesLoaded, m_filesView, &FilesView::onFilesLoaded, Qt::QueuedConnection);

    connect(m_filesView, &FilesView::fileSelected, this, &MainWindow::onFileCliked, Qt::QueuedConnection);
    connect(m_filesView, &FilesView::fileEncode, this, &MainWindow::onEncode, Qt::QueuedConnection);
    connect(m_filesView, &FilesView::fileDecode, this, &MainWindow::onDecode, Qt::QueuedConnection);

    //
    //
    setCentralWidget(m_filesView);
}

void MainWindow::createStatusBar()
{
    QStatusBar* pStatusBar = statusBar();
    if (pStatusBar == nullptr)
    {
        return;
    }

    // create
    //
    m_statusEmpty = new QLabel(pStatusBar);
    m_statusFileName = new QLabel(pStatusBar);

    // addWidget
    //
    pStatusBar->addWidget(m_statusFileName);
    pStatusBar->addWidget(m_statusEmpty);

    pStatusBar->setLayoutDirection(Qt::RightToLeft);

    // set default value
    //
    m_statusEmpty->setText(QString());
    onFileNameChanged(QString());
}

void MainWindow::onEncode()
{
    startWorker(Arch::OperationType::Encode);
}

void MainWindow::onDecode()
{
    startWorker(Arch::OperationType::Decode);
}

void MainWindow::OnAboutQt()
{
    QMessageBox::aboutQt(this, qAppName());
}

void MainWindow::onDocumentation()
{
    QString filePath = QApplication::applicationDirPath() + "/docs/Manual.pdf";

    QFile file(filePath);
    if (file.exists() == false)
    {
        QMessageBox::critical(this, qAppName(), QObject::tr("Help file '%1' does not exist!").arg(filePath));
        return;
    }

    QUrl url = QUrl::fromLocalFile(filePath);
    QDesktopServices::openUrl(url);
}

void MainWindow::onFileNameChanged(const QString& fileName)
{
    if (m_statusFileName == nullptr)
    {
        return;
    }

    m_statusFileName->setText(tr(" File: %1 ").arg(fileName));
}

void MainWindow::onContextMenu(QPoint)
{
    if (m_pContextMenu == nullptr)
    {
        return;
    }

    m_pContextMenu->exec(QCursor::pos());
}

void MainWindow::onFileCliked(const FileItem& item)
{
    if (item.fileName().isEmpty())
    {
        return;
    }

    m_selectedfile = item;

    onFileNameChanged(item.fileName());
}

void MainWindow::onFilesFilter(int index)
{
    if (m_pFilterList == nullptr)
    {
        return;
    }

    int filterType = index;
    if (filterType < 0 || filterType >= FilesFiltersCount)
    {
        return;
    }

    m_filterType = filterType;

    loadFilesList();
}

void MainWindow::startWorker(Arch::OperationType operationType)
{
    if (m_selectedfile.fileName().isEmpty())
    {
        return;
    }
    
    Arch::Worker* pWorker = new Arch::Worker(m_selectedfile.fileName());
    if (pWorker == nullptr)
    {
        return;
    }

    m_workers.append(pWorker);

    QThread* pThread = new QThread();
    if (pThread == nullptr)
    {
        return;
    }

    switch(operationType)
    {
        case Arch::OperationType::Encode:
            connect(pThread, &QThread::started, pWorker, &Arch::Worker::encode);
            break;
        
        case Arch::OperationType::Decode:
            connect(pThread, &QThread::started, pWorker, &Arch::Worker::decode);
            break;
    }

    connect(pWorker, &Arch::Worker::finished, this, &MainWindow::onWorkerFinished);
    connect(pWorker, &Arch::Worker::finished, pThread, &QThread::terminate);

    pWorker->moveToThread(pThread);
    pThread->start();
}


void MainWindow::onWorkerFinished()
{
    Arch::Worker* pWorker = qobject_cast<Arch::Worker*>(sender());
    if (pWorker == nullptr)
    {
        return;
    }

    Arch::WorkerErrors result = pWorker->lastError();

    m_workers.remove(pWorker);

    if (result == Arch::WorkerErrors::NoErrors)
        loadFilesList();
}

void MainWindow::loadSettings()
{
    QSettings s;

    QByteArray geometry = s.value(QString("%1MainWindow/geometry").arg(WINDOW_GEOMETRY_OPTIONS_KEY)).toByteArray();
    QByteArray state = s.value(QString("%1MainWindow/State").arg(WINDOW_GEOMETRY_OPTIONS_KEY)).toByteArray();

    restoreGeometry(geometry);
    restoreState(state);
}

void MainWindow::saveSettings()
{
    QSettings s;

    s.setValue(QString("%1MainWindow/Geometry").arg(WINDOW_GEOMETRY_OPTIONS_KEY), saveGeometry());
    s.setValue(QString("%1MainWindow/State").arg(WINDOW_GEOMETRY_OPTIONS_KEY), saveState());
}

void MainWindow::loadFilesList()
{
    if (m_filterType < 0 || m_filterType >= FilesFiltersCount)
        return;

    if (m_path.isEmpty())
        return;

    m_files.clear();

    QDir dir = QDir(m_path);

    QFileInfoList list = dir.entryInfoList(QStringList(FilesFilters[m_filterType]), QDir::Files | QDir::NoDotAndDotDot);
    foreach (QFileInfo finfo, list)
    {
        FileItem item(finfo.fileName(), finfo.size());
        m_files.push_back(item);
    }

    emit filesLoaded(m_files);
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    // proccess of decodering is not finised
    //
    if (!m_workers.isEmpty())
        return;

    saveSettings();

    QMainWindow::closeEvent(e);
}
