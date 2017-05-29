#include "luacheck.h"
#include <QCoreApplication>
#include <QProcess>
#include <QFileInfo>
#include <QsLog.h>

const QString LUACHECK_PATH = "/luarocks/systree/bin/luacheck.bat";

LuaCheck::LuaCheck(QObject *parent) : QObject(parent),
    m_globals(QStringList() << "path" << "log"),
    m_silencedErrors(QStringList() << "setting non-standard global variable 'setup'"
                                   << "setting non-standard global variable 'process'")
{

}

bool LuaCheck::removeGlobal(const QString &name)
{
    return m_globals.removeOne(name);
}

void LuaCheck::addGlobal(const QString &name)
{
    m_globals.append(name);
}

void LuaCheck::addSilencedError(const QString &error)
{
    m_silencedErrors.append(error);
}

const QStringList& LuaCheck::getGlobals() const
{
    return m_globals;
}

const QStringList& LuaCheck::getSilencedMessages() const
{
    return m_silencedErrors;
}

bool LuaCheck::isCheckPositive() const
{
    for(const QList<LuaCheckResult>& results : m_results.values())
    {
        for(const LuaCheckResult& result : results)
        {
            if(result.errorCode.startsWith('E'))
            {
                return false;
            }
        }
    }
    return true;
}

QList<LuaCheckResult> LuaCheck::checkFile(const QString &filePath)
{
    QString luaCheckPath = qApp->applicationDirPath() + LUACHECK_PATH;
    QFile luaCheckFile(luaCheckPath);
    if(!luaCheckFile.exists())
    {
        QLOG_ERROR() << "Couldn't find luacheck binary at" << luaCheckPath;
        return QList<LuaCheckResult>();
    }

    QList<LuaCheckResult> results;
    QProcess luacheck;
    QStringList globals;
    for(const QString &global : getGlobals())
    {
        globals.append("--globals");
        globals.append(global);
    }
    luacheck.start(luaCheckPath, QStringList() << filePath << "--codes" << globals);
    luacheck.waitForFinished();
    QString output = luacheck.readAll();
    for(const QString &line : output.split("\r\n"))
    {
        if(line.trimmed().startsWith(filePath))
        {
            QString checkedLine = QString(line).remove(filePath).trimmed().remove(0, 1);
            int semiCPos  = checkedLine.indexOf(':');
            QString lineNumber = checkedLine.left(semiCPos);
            checkedLine = checkedLine.remove(0, semiCPos + 1);
            semiCPos = checkedLine.indexOf(':');
            checkedLine = checkedLine.remove(0, semiCPos + 1).trimmed();
            checkedLine.remove(0, 1);
            int bracketPos = checkedLine.indexOf(')');

            QString errCode = checkedLine.left(bracketPos);
            QString message = checkedLine.remove(0, bracketPos +1).trimmed();
            int lineNumberInt = lineNumber.toInt();
            QFileInfo fInfo(filePath);
            LuaCheckResult result;
            if(!getSilencedMessages().contains(message) && !errCode.startsWith("W")) //silence warnings
            {
                result.fileName = fInfo.completeBaseName();
                result.line = lineNumberInt;
                result.errorCode = errCode;
                result.message = message;
                results.append(result);
            }
        }
    }
    QLOG_INFO() << "Checked file" << filePath << ", found" << results.size() << "issues";
    for(const auto& result : results)
    {
        QLOG_DEBUG() << result.errorCode << result.line << result.message;
    }
    m_results.insert(filePath, results);
    return results;
}

QList<LuaCheckResult> LuaCheck::checkFiles(const QStringList &filePaths)
{
    QList<LuaCheckResult> results;
    for(const QString& filePath : filePaths)
    {
        results.append(checkFile(filePath));
    }
    return results;
}
