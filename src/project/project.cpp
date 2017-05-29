#include "project.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QDir>
#include <QsLog.h>
#include <QDirIterator>

Project::Project(std::unique_ptr<QIODevice> device, const QString &path) :
    m_device(std::move(device)),
    m_path(path)
{
    QFileInfo fileInfo(path);
    m_dirPath = fileInfo.absolutePath();
    QDir dir(m_dirPath);
    m_name = dir.dirName();

    m_device->open(QIODevice::ReadWrite);
    if(!m_device->isOpen())
    {
        return;
    }
    QByteArray data = m_device->readAll();
    data.isEmpty() == true ? m_device->write(toJson().toUtf8()) : fromJson(data);
    m_device->close();
}

QString Project::toJson() const
{
    QJsonObject json
    {
        {"runOnSave", m_runOnSave},
        {"yearFrom", m_yearFrom},
        {"yearTo", m_yearTo}
    };
    QJsonDocument doc(json);
    return QString::fromUtf8(doc.toJson());
}

QString Project::name() const
{
    return m_name;
}

QStringList Project::getLuaFiles() const
{
    QStringList paths;
    QDirIterator it(m_dirPath, QStringList() << "*.lua", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        paths.append(it.next());
    }
    return paths;
}

bool Project::getRunOnSave() const
{
    return m_runOnSave;
}

void Project::setRunOnSave(bool runOnSave)
{
    m_runOnSave = runOnSave;
    save();
}

int Project::getYearFrom() const
{
    return m_yearFrom;
}

void Project::setYearFrom(int yearFrom)
{
    m_yearFrom = yearFrom;
    save();
}

int Project::getYearTo() const
{
    return m_yearTo;
}

void Project::setYearTo(int yearTo)
{
    m_yearTo = yearTo;
    save();
}

bool Project::fromJson(const QString &jsonString)
{
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    if(error.error != QJsonParseError::NoError)
    {
        QLOG_ERROR() << "Couldn't parse json string:" << error.errorString();
        return false;
    }
    QJsonObject jsonObject = jsonDocument.object();
    if(!jsonObject.contains("runOnSave") ||
            !jsonObject.contains("yearFrom") ||
            !jsonObject.contains("yearTo"))
    {
        QLOG_ERROR() << "Json object is incomplete";
        return false;
    }
    QLOG_DEBUG() << "Project file parsed:" << jsonDocument.toJson(QJsonDocument::Compact);
    m_runOnSave = jsonObject.value("runOnSave").toBool();
    m_yearFrom = jsonObject.value("yearFrom").toInt();
    m_yearTo = jsonObject.value("yearTo").toInt();
    return true;
}

bool Project::save()
{
    if(!m_device->open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
        m_device->close();
    }
    m_device->write(toJson().toUtf8());
    m_device->close();
    return true;
}

QString Project::dirPath() const
{
    return m_dirPath;
}
