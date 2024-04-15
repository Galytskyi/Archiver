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
    
    void append(Arch::Worker* pWorker);
    void remove(Arch::Worker* pWorker);

private:
    QMutex m_mutex;
    std::vector<Arch::Worker*> m_workers;
};

#endif // WORKERSBASE_H
