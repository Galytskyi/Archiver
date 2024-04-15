#include "FilesBase.h"

#include <QElapsedTimer>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------

FileItem::FileItem()
{
    FileItem::clear();
}

// -------------------------------------------------------------------------------------------------------------------

FileItem::~FileItem()
{
}

// -------------------------------------------------------------------------------------------------------------------

void FileItem::clear()
{
	m_packetID = -1;
	m_IMEI = 0;

	m_packetTime.setDate(QDate());

	m_imageType = ImageType::NoImageType;
	m_imageSize = 0;
	m_imageWidth = 0;
	m_imageHeight = 0;

	m_brightness = 0;
	m_location = QT_TRANSLATE_NOOP("DataPacket", "Unknown");

	m_imageData.clear();
}

// -------------------------------------------------------------------------------------------------------------------

QString FileItem::imeiStr(int digits) const
{
    if (m_IMEI == 0)
	{
		return QT_TRANSLATE_NOOP("PacketBase", "Unknown");
	}

	return QString::number(m_IMEI).right(digits);
}

// -------------------------------------------------------------------------------------------------------------------

QString FileItem::brightnessStr() const
{
	double result = round(static_cast<double>(m_brightness) / 255.0 * 100.0);

	return QString::number(result, 'f', 0) + " %";
}

// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------

FilesBase::FilesBase(QObject *parent)
	: QObject{parent}
{
}

// -------------------------------------------------------------------------------------------------------------------


FilesBase::~FilesBase()
 {
 }

 // -------------------------------------------------------------------------------------------------------------------

 int FilesBase::count() const
 {
	 QMutexLocker locker(&m_packetMutex);

     return static_cast<int>(m_filesList.size());
 }

 // -------------------------------------------------------------------------------------------------------------------

 void FilesBase::clear()
 {
	 QMutexLocker locker(&m_packetMutex);

     qDeleteAll(m_filesList);
 }

 // -------------------------------------------------------------------------------------------------------------------

 int FilesBase::load()
 {
     QElapsedTimer responseTime;
     responseTime.start();

     // if (theDatabase.isOpen() == false)
     // {
     //  return false;
     // }

     // SqlTable* pTable = theDatabase.openTable(SQL_TABLE_PICTURE_INFO);
     // if (pTable == nullptr)
     // {
     //  return false;
     // }

     // m_packetMutex.lock();

     //  int recordCount = pTable->recordCount();
     //  if (recordCount > 0)
     //  {
     // 	 m_packetList.resize(static_cast<quint64>(recordCount));

     // 	 DataPacket*	pPacketList = new DataPacket[static_cast<quint64>(recordCount)];;
     // 	 if (pPacketList != nullptr)
     // 	 {
     // 		int readedCount = pTable->read(pPacketList);
     // 		if (readedCount == recordCount)
     // 		{
     // 			for(int i = 0; i < recordCount; i++)
     // 			{
     // 				m_packetList[i] = pPacketList + i;
     // 			}
     // 		}
     // 		else
     // 		{
     // 			assert(0);
     // 			qDebug() << __FUNCTION__ << ": Error of reading" <<
     // 						" readed: " << readedCount <<
     // 						" form: " << recordCount;

     // 			delete [] pPacketList;
     // 		}
     // 	 }
     //  }

     // m_packetMutex.unlock();

     // pTable->close();

     // qDebug() << __FUNCTION__ << ": DataPackets"	<<
     // 		 ", Loaded records: " << count()	<<
     // 		 ", Time for load: " << responseTime.elapsed() << " ms";

     //emit packetBaseLoaded(m_packetList);


     QList <QString> files;

     QDir dir = QDir::currentPath();

     //void SearchFiles::scanDir(QDir dir)
     {
         if(!dir.exists())
         {
             //isFind = false;
             return 1;
         }

         //QFileInfoList pngList = folderDir.entryInfoList(QStringList("*.png"), QDir::Files | QDir::NoDotAndDotDot);

         QFileInfoList list = dir.entryInfoList(QStringList("*.rc"), QDir::Files | QDir::NoDotAndDotDot);
         foreach (QFileInfo finfo, list)
         {

             QString name = finfo.fileName();
             int size = finfo.size();

             int a  = 0;
             a++;
         }

     }

     return count();
 }

 // -------------------------------------------------------------------------------------------------------------------

 int FilesBase::append(FileItem* pPacket)
 {
	 if (pPacket == nullptr)
	 {
		 return -1;
	 }

	 int index = -1;

	 m_packetMutex.lock();

     m_filesList.push_back(pPacket);
     index = static_cast<int>(m_filesList.size() - 1);

	 m_packetMutex.unlock();

	 return index;
 }

 // -------------------------------------------------------------------------------------------------------------------

 void FilesBase::appendToBase(FileItem* pPacket)
 {
	 if (pPacket == nullptr)
	 {
		 return;
	 }

	 int index = append(pPacket);
	 if (index == -1)
	 {
		 QMessageBox::critical(nullptr, tr("Save packets"), tr("Error saving packets to memory"));
		 return;
	 }
 }

 // ----------------------------------------------------------------------------------------------

 FileItem* FilesBase::packet(int index) const
 {
	 QMutexLocker locker(&m_packetMutex);

     if (index < 0 || index >= static_cast<int>(m_filesList.size()))
	 {
		 return nullptr;
	 }

     return m_filesList[static_cast<quint64>(index)];
 }

 // -------------------------------------------------------------------------------------------------------------------

 bool FilesBase::remove(int index)
 {
	 QMutexLocker locker(&m_packetMutex);

     if (index < 0 || index >= static_cast<int>(m_filesList.size()))
	 {
		 return false;
	 }

     auto it = m_filesList.cbegin();

	 std::advance(it, index);

//	 auto pPacket = *it;
//	 if (pPacket != nullptr)
//	 {
//		 delete pPacket;
//	 }

     m_filesList.erase(it);

	 return true;
 }

 // -------------------------------------------------------------------------------------------------------------------

 bool FilesBase::remove(const std::vector<int>& keyList)
 {
	 int keyCount = static_cast<int>(keyList.size());
	 if (keyCount == 0)
	 {
		 return false;
	 }

	 int packetCount = count();
	 if (packetCount == 0)
	 {
		 return false;
	 }

	 int removed = 0;

	 m_packetMutex.lock();

		 for(int packetID : keyList)
		 {
         auto foundID = std::find_if(m_filesList.begin(), m_filesList.end(), [&packetID](FileItem* pPacket)
											 {
												 return pPacket->packetID() == packetID;
											 });

             if (foundID == m_filesList.end())
			 {
				 continue;
			 }

             m_filesList.erase(foundID);

			 removed++;
		 }

	m_packetMutex.unlock();

	if (removed != keyCount)
	{
		return false;
	}

	return true;
 }

 // -------------------------------------------------------------------------------------------------------------------

 void FilesBase::removeFromBase(const std::vector<int>& keyList)
 {
	 bool result = remove(keyList);
	 if (result == false)
	 {
		 QMessageBox::critical(nullptr, tr("Delete packets"), tr("Error remove packets from memory"));
	 }
 }

 // -------------------------------------------------------------------------------------------------------------------
 // -------------------------------------------------------------------------------------------------------------------
 // -------------------------------------------------------------------------------------------------------------------

 QString ImageTypeCaption(int imageType)
 {
	 QString caption;

	 switch (imageType)
	 {
		 case ImageType::Raw:	caption = "RAW";	break;
		 case ImageType::Bmp:	caption = "BMP";	break;
		 case ImageType::Jpg:	caption = "JPG";	break;

		 default:
			 Q_ASSERT(0);
			 caption = QT_TRANSLATE_NOOP("PacketBase", "Unknown");
	 }

	 return caption;
 };

 // -------------------------------------------------------------------------------------------------------------------
 // -------------------------------------------------------------------------------------------------------------------
 // -------------------------------------------------------------------------------------------------------------------

 QString timeToStr(const QDateTime& dateTime)
 {
	 QString timeStr;

	 timeStr = QString::asprintf("%02d-%02d-%04d %02d:%02d:%02d",

								 dateTime.date().day(),
								 dateTime.date().month(),
								 dateTime.date().year(),

								 dateTime.time().hour(),
								 dateTime.time().minute(),
								 dateTime.time().second());
	 return timeStr;
 }

 // -------------------------------------------------------------------------------------------------------------------

 QString imageDir(const QString& fileName)
 {
	 QString curDir = QDir::currentPath();
	 curDir.replace("/", QDir::separator() );

	 return curDir + QDir::separator() + IMAGE_DIRECTORY + QDir::separator() + fileName;
 };

 // -------------------------------------------------------------------------------------------------------------------
 // -------------------------------------------------------------------------------------------------------------------
 // -------------------------------------------------------------------------------------------------------------------
