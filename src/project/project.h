#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <QString>
#include <QJsonObject>
#include <QIODevice>

class Project
{
    Q_DISABLE_COPY(Project)
public:
    Project(std::unique_ptr<QIODevice> device, const QString& path);
    virtual ~Project() = default;

    QString path() const;
    QString name() const;
    QString dirPath() const;
    QStringList getLuaFiles() const;

    bool getRunOnSave() const;
    void setRunOnSave(bool runOnSave);

    int getYearFrom() const;
    void setYearFrom(int yearFrom);

    int getYearTo() const;
    void setYearTo(int yearTo);

private:
    QString toJson() const;
    bool fromJson(const QString &jsonString);
    bool save();

private:
    std::unique_ptr<QIODevice> m_device;
    QString m_path;
    QString m_dirPath;
    QString m_name;

    bool m_runOnSave = true;
    int m_yearFrom = 1;
    int m_yearTo = 2;
};

#endif // PROJECT_H
