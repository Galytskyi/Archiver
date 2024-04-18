#ifndef WORKERSBASE_H
#define WORKERSBASE_H

#include "ArchWorker.h"

class WorkersBase : public QObject
{
    Q_OBJECT

public:
    explicit WorkersBase(QObject* parent = nullptr);
    virtual ~WorkersBase() override;

    void clear();
    bool isEmpty();
    
    Q_INVOKABLE void appendEncodeFile(const QString& fileName);
    Q_INVOKABLE void appendDecodeFile(const QString& fileName);

private:
    QMutex m_mutex;
    std::vector<Arch::Worker*> m_workers;

    void appendWorkerThread(const QString& fileName, Arch::OperationType operationType);

signals:

    void workerFinished();

public slots:

    void onFinished();

};

#endif // WORKERSBASE_H
