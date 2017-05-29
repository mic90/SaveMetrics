#ifndef LUACHECK_H
#define LUACHECK_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHash>

struct LuaCheckResult
{
    QString fileName;
    int line;
    QString errorCode;
    QString message;
};

class LuaCheck : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LuaCheck)
public:
    LuaCheck(QObject *parent = 0);

    bool removeGlobal(const QString& name);
    void addGlobal(const QString& name);
    void addSilencedError(const QString& error);

    const QStringList& getGlobals() const;
    const QStringList& getSilencedMessages() const;
    bool isCheckPositive() const;

public slots:
    QList<LuaCheckResult> checkFile(const QString& filePath);
    QList<LuaCheckResult> checkFiles(const QStringList& filePaths);

private:
    QStringList m_globals;
    QStringList m_silencedErrors;
    QHash<QString, QList<LuaCheckResult>> m_results;
};

#endif // LUACHECK_H
