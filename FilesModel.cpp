#include "FilesModel.h"

// ----------------------------------------------------------------------------------------------

std::vector<FileItem> loadFilesList(const QString& path, const QString& filter)
{
    if (path.isEmpty() || filter.isEmpty())
    {
        return std::vector<FileItem>();
    }

    std::vector<FileItem> files;

    QDir dir = QDir(path);

    QFileInfoList list = dir.entryInfoList(QStringList(filter), QDir::Files | QDir::NoDotAndDotDot);
    foreach (QFileInfo finfo, list)
    {
        FileItem item(finfo.fileName(), finfo.size());
        files.push_back(item);
    }

    return files;
}

// ----------------------------------------------------------------------------------------------

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
    return FILES_MODEL_COLUMN_COUNT;
}

int FilesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

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
        if (section >= 0 && section < FILES_MODEL_COLUMN_COUNT)
		{
            result = FilesModelColumn[section];
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

    if (role > Qt::UserRole)
    {
        return data(this->index(index.row(), role - Qt::UserRole - 1), Qt::DisplayRole);
    }

	int rowIndex = index.row();
    if (rowIndex < 0 || rowIndex >= static_cast<int>(m_filesCount))
	{
		return QVariant();
	}

	int columnIndex = index.column();
    if (columnIndex < 0 || columnIndex > FILES_MODEL_COLUMN_COUNT)
	{
		return QVariant();
	}

    const FileItem& item = at(rowIndex);

	if (role == Qt::TextAlignmentRole)
	{
		int result = Qt::AlignLeft;

		switch (columnIndex)
		{
            case FILES_MODEL_COLUMN_LOCATION:	result = Qt::AlignLeft;	break;
            case FILES_MODEL_COLUMN_SIZE:		result = Qt::AlignCenter;	break;

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

    if (column < 0 || column > FILES_MODEL_COLUMN_COUNT)
	{
		return QString();
	}

	QString result;

	switch (column)
	{
        case FILES_MODEL_COLUMN_LOCATION:	result = item.fileName();                   break;
        case FILES_MODEL_COLUMN_SIZE:		result = QString::number(item.fileSize());  break;

		default:
			assert(0);
	}

	return result;
}

QHash<int, QByteArray> FilesModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();

    for (int c = 0; c < FILES_MODEL_COLUMN_COUNT; c++)
    {
        QString columnName = FilesModelColumn[c];
        roles.insert(Qt::UserRole + c + 1,  columnName.toUtf8());
    }

    return roles;
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

// ----------------------------------------------------------------------------------------------
