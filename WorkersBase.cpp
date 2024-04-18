#include "../WorkersBase.h"

WorkersBase::WorkersBase(QObject* parent)
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

void WorkersBase::appendEncodeFile(const QString& fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    appendWorkerThread(fileName, Arch::OperationType::Encode);
}

void WorkersBase::appendDecodeFile(const QString& fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    appendWorkerThread(fileName, Arch::OperationType::Decode);
}


void WorkersBase::appendWorkerThread(const QString& fileName, Arch::OperationType operationType)
{
    if (fileName.isEmpty())
    {
        return;
    }

    Arch::Worker* pWorker = new Arch::Worker(fileName);
    if (pWorker == nullptr)
    {
        return;
    }

    QThread* pThread = new QThread();
    if (pThread == nullptr)
    {
        return;
    }

    m_mutex.lock();
        m_workers.push_back(pWorker);
    m_mutex.unlock();

    switch(operationType)
    {
    case Arch::OperationType::Encode:
        connect(pThread, &QThread::started, pWorker, &Arch::Worker::encode);
        break;

    case Arch::OperationType::Decode:
        connect(pThread, &QThread::started, pWorker, &Arch::Worker::decode);
        break;
    }

    connect(pWorker, &Arch::Worker::finished, this, &WorkersBase::onFinished);
    connect(pWorker, &Arch::Worker::finished, pThread, &QThread::terminate);

    pWorker->moveToThread(pThread);
    pThread->start();
}

void WorkersBase::onFinished()
{
    Arch::Worker* pWorker = qobject_cast<Arch::Worker*>(sender());
    if (pWorker == nullptr)
    {
        return;
    }

    Arch::WorkerErrors result = pWorker->lastError();
    if (result == Arch::WorkerErrors::NoErrors)
    {
        emit workerFinished(); // update list of files in the main window
    }

    QMutexLocker locker(&m_mutex);

    const auto it = std::remove(m_workers.begin(), m_workers.end(), pWorker);
    m_workers.erase(it, m_workers.end());

    delete pWorker;
}
