#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QObject>
#include <QFileInfo>
#include <QMutex>
#include <QCryptographicHash>

#define FILE_EXT_SCRIPT "lua"

class ProjectFile : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ProjectFile)
public:
    explicit ProjectFile(QString const& path, QObject *parent = 0);
    virtual ~ProjectFile() = default;

    virtual bool save();
    virtual bool load();

    void setData(QString const& data);
    QString getData() const;

    QString getName() const;
    QString getExtension() const;
    QString getPath() const;

    bool changeFileName(const QString& newFileName);

signals:
    void fileSaved();
    void fileLoaded();
    void fileRenamed();

protected:
    QString m_name;
    QString m_extension;
    QString m_path;
    QString m_data;
    QString m_tempData;

private:
    QMutex mutex;
};

#endif // PROJECTFILE_H
