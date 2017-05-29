#ifndef LUAROCKS_H
#define LUAROCKS_H

#include <QObject>

struct Rock
{
    QString name;
    QString version;
    QString path;
    QString status;
};

class LuaRocks : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LuaRocks)
public:
    explicit LuaRocks(QObject *parent = 0);

    QList<Rock> getInstalledRocks();
    QString removeRock(const QString& name);
    QString installRock(const QString& name);
    QStringList searchForRock(const QString& name);

private:
    QString runLuaRocks(const QStringList& arguments);

private:
    const QString m_luarocksPath;
};

#endif // LUAROCKS_H
