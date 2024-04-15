#include "WorkersBase.h"

WorkersBase::WorkersBase(QObject *parent)
    : QObject{parent}
{
}

WorkersBase::~WorkersBase()
{
    clear();
}

void WorkersBase::clear()
{
    QMutexLocker locker(&m_mutex);

    for(auto& worker : m_workers)
        delete worker;

    m_workers.clear();
}

bool WorkersBase::isEmpty()
{
    QMutexLocker locker(&m_mutex);
    return m_workers.empty();
}

void WorkersBase::append(Arch::Worker* pWorker)
{
    if (pWorker == nullptr)
    {
        return;
    }

    QMutexLocker locker(&m_mutex);

    m_workers.push_back(pWorker);
}

void WorkersBase::remove(Arch::Worker* pWorker)
{
    if (pWorker == nullptr)
    {
        return;
    }

    QMutexLocker locker(&m_mutex);

    const auto it = std::remove(m_workers.begin(), m_workers.end(), pWorker);
    m_workers.erase(it, m_workers.end());

    delete pWorker;
}

