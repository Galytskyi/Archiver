#ifndef WORKERSBASE_H
#define WORKERSBASE_H

#include <QObject>

#include "ArchWorker.h"

class WorkersBase : public QObject
{
    Q_OBJECT

public:
    explicit WorkersBase(QObject *parent = nullptr);
    virtual ~WorkersBase() override;

    void clear();

    bool isEmpty();
    
    void appendWorkerThread(const QString& fileName, Arch::OperationType operationType);

private:
    QMutex m_mutex;
    std::vector<Arch::Worker*> m_workers;

signals:

    void workerFinished();

public slots:

    void onFinished();

};

#endif // WORKERSBASE_H
