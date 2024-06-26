#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "../ArchFile.h"
#include "../WorkersBase.h"
#include "../FilesModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Get work path from cmd line
    //
    QString path = Arch::getWorkDir(argc, argv).c_str();

    //
    //
    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    //
    //
    WorkersBase workerThreads;
    engine.rootContext()->setContextProperty("workerThreads", &workerThreads);

    //
    //
    QStringListModel filtersModel;
    QStringList filters;
    for(const auto& s : FilesFilters)
        filters << s;
    filtersModel.setStringList(filters);
    engine.rootContext()->setContextProperty("filtersModel", &filtersModel);

    //
    //
    QString initFilter = FilesFilters[FileFilters_All];

    FilesModel tableMode(path, initFilter) ;
    tableMode.set(loadFilesList(path, initFilter));
    engine.rootContext()->setContextProperty("tableModel", &tableMode);

    QObject::connect(&workerThreads, &WorkersBase::workerFinished, &tableMode, &FilesModel::onFilesUpdated, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
