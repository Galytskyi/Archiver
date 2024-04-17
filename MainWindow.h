#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ArchFile.h"
#include "ArchWorker.h"
#include "WorkersBase.h"
#include "FilesView.h"

// ----------------------------------------------------------------------------------------------

#define WINDOW_GEOMETRY_OPTIONS_KEY "Options/Window/"

// ----------------------------------------------------------------------------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& path, QWidget *parent = nullptr);
    ~MainWindow();

private:

    QString m_path;

    std::vector<FileItem> m_files;
    FileItem m_selectedfile;
    
    WorkersBase m_workerThreads;

 private:

    // Actions of main menu
    //
    // menu - File
    //
    QAction* m_pEncodeAction = nullptr;
    QAction* m_pDecodeAction = nullptr;

    // menu - ?
    //
    QAction* m_pAboutQtAction = nullptr;
    QAction* m_pDocumentationAction = nullptr;

    //
    //
    QMenu* m_pContextMenu = nullptr;

private:

    // Elements of interface - Menu
    //
    QMenu* m_pFileMenu = nullptr;
    QMenu* m_pInfoMenu = nullptr;

    // Elements of interface - ToolBar
    //
    QToolBar* m_pControlToolBar = nullptr;

    // Elements of interface - Items of ToolBars
    //
    QComboBox* m_pFilterList = nullptr;
    int m_filterType = FileFilters_All;

    // Elements of interface - View
    //
    FilesView* m_filesView = nullptr;

    // Elements of interface - StatusBar
    //
    QLabel* m_statusEmpty = nullptr;
    QLabel* m_statusFileName = nullptr;

private:

    bool createInterface();

    void createActions();
    void createMenu();
    bool createToolBars();
    void createFilesView();
    void createStatusBar();

    void loadSettings();
    void saveSettings();

    void loadFiles();
    void startWorker(Arch::OperationType operationType);

protected:

    void closeEvent(QCloseEvent* e) override;

signals:

    void filesLoaded(const std::vector<FileItem>& files);

private slots:

    // Slots of main menu
    //

    // menu - File
    //
    void onEncode();
    void onDecode();

    // menu - ?
    //
    void OnAboutQt();
    void onDocumentation();

    // Slots of contex menu
    //
    void onContextMenu(QPoint);

    // Slots of control panels
    //
    void onFilesFilter(int index);

    // Slots for StatusBar
    //
    void onFileNameChanged(const QString& fileName);

    // Slot of FilesView
    //
    void onFileCliked(const FileItem& item);

    // slots of m_workers
    //
    void onWorkerFinished() { loadFiles(); }
};

// ----------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
