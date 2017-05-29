#include "luarocks.h"
#include <QsLog.h>
#include <QCoreApplication>
#include <QFile>
#include <QProcess>

LuaRocks::LuaRocks(QObject *parent) : QObject(parent),
    m_luarocksPath(qApp->applicationDirPath() + "/luarocks/luarocks.bat")
{

}

QList<Rock> LuaRocks::getInstalledRocks()
{
    QList<Rock> rocks;
    QString output = runLuaRocks(QStringList() << "list");
    QStringList splitted = output.split("\r\n");
    //remove luarocks header
    splitted.removeFirst();
    splitted.removeFirst();
    splitted.removeFirst();
    splitted.removeFirst();
    splitted.removeLast();

    for(int i=0; i<splitted.size(); i+=3) {
        QString rockName = splitted.at(i);
        QString rockInfo = splitted.at(i+1);
        Rock rock;
        QStringList infoSplitted = rockInfo.trimmed().split(' ');
        QString version = infoSplitted.at(0);
        QString status = infoSplitted.at(1);
        QString path = infoSplitted.at(3);

        rock.name = rockName;
        rock.version = version;
        rock.path = path;
        rock.status = status;
        rocks.append(rock);
    }
    return rocks;
}

QString LuaRocks::removeRock(const QString &name)
{
    return runLuaRocks(QStringList() << "remove" << name);
}

QString LuaRocks::installRock(const QString &name)
{
    return runLuaRocks(QStringList() << "install" << name);
}

QStringList LuaRocks::searchForRock(const QString &name)
{
    QStringList results;
    QString output = runLuaRocks(QStringList() << "search" << name);
    QStringList outputSplitted = output.trimmed().split("\r\n");
    QLOG_DEBUG() << outputSplitted;
    return results;
}

QString LuaRocks::runLuaRocks(const QStringList &arguments)
{
    QFile luaRockBinary(m_luarocksPath);
    if(!luaRockBinary.exists()) {
        QLOG_ERROR() << "Couldn't find luarock binary at" << m_luarocksPath;
        return QString();
    }
    QProcess luaRock;
    QLOG_INFO() << "Running process" << m_luarocksPath << "with arguments:" << arguments;
    luaRock.start(m_luarocksPath, arguments);
    luaRock.waitForFinished();
    return luaRock.readAll();
}
