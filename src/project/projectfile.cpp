#include "projectfile.h"
#include <QDir>
#include <QsLog.h>

ProjectFile::ProjectFile(const QString &path, QObject *parent) : QObject(parent),
    m_path(path),
    m_data(""),
    m_tempData("")
{
    QFileInfo fInfo(path);
    m_extension = fInfo.suffix();
    m_name = fInfo.fileName();
}

bool ProjectFile::save()
{
    QMutexLocker locker(&mutex);

    QFile file(m_path);
    if(!file.open(QIODevice::WriteOnly)) {
        QLOG_ERROR() << "Can't open file for writing at:" << m_path;
        return false;
    }
    file.write(m_tempData.toUtf8());
    file.flush();
    file.close();
    QLOG_INFO() << "File" << m_name << "saved";
    m_data = m_tempData;
    emit fileSaved();
    return true;
}

bool ProjectFile::load()
{
    QMutexLocker locker(&mutex);

    QFile file(m_path);
    if(!file.exists() || !file.open(QIODevice::ReadOnly)) {
        QLOG_ERROR() << "Can't open file for reading at:" << m_path;
        return false;
    }
    QString fileData = QString::fromUtf8(file.readAll());
    QByteArray fileHash = QCryptographicHash::hash(fileData.toUtf8(), QCryptographicHash::Md5);
    QByteArray currentHash = QCryptographicHash::hash(m_data.toUtf8(), QCryptographicHash::Md5);
    //change internal file content only if it differs from physical file
    if(fileHash != currentHash){
        m_data = fileData;
        m_tempData = m_data;
        QLOG_INFO() << "File" << m_name << "loaded, file size:" << fileData.length();
        emit fileLoaded();
    }
    file.close();
    return true;
}

void ProjectFile::setData(const QString &data)
{
    QMutexLocker locker(&mutex);
    this->m_tempData = data;
}

QString ProjectFile::getData() const
{
    return m_data;
}

QString ProjectFile::getName() const
{
    return m_name;
}

QString ProjectFile::getExtension() const
{
    return m_extension;
}

QString ProjectFile::getPath() const
{
    return m_path;
}

bool ProjectFile::changeFileName(const QString& newFileName)
{
    QMutexLocker locker(&mutex);

    QFileInfo fInfo(m_path);
    QString newFilePath = fInfo.absolutePath() + "/" + newFileName;
    QFile file(newFilePath);
    if(file.exists()) {
        QLOG_ERROR() << "Can't change file name to" << newFileName <<", file already exists";
        return false;
    }
    load();
    QFile oldFile(m_path);
    if(!oldFile.remove()) {
        QLOG_ERROR() << "Can't remove old file";
        return false;
    }
    QLOG_INFO() << "File name changed from:" << m_name << "to:" << newFileName;
    m_name = newFileName;
    m_path = newFilePath;
    save();
    emit fileRenamed();
    return true;
}
