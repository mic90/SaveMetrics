#ifndef SCRIPTENGINELUA_H
#define SCRIPTENGINELUA_H

#include <QObject>
#include <QElapsedTimer>
#include <QSharedPointer>

#include <script/lualogger.h>
#include <script/scriptfile.h>
#include <lua.hpp>
#include <sol.hpp>

class ScriptEngineLua : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ScriptEngineLua)
public:
    enum State { Stopped = 0, Running = 1};
    ScriptEngineLua(const QString& scriptPath, int yearsFrom, int yearsTo, QObject *parent = 0);

    void addScriptFile(QSharedPointer<ScriptFile> file);
    State getState() const;

public slots:
    bool start();
    void stop();

signals:
    void stateChanged(ScriptEngineLua::State);
    void processingStarted();
    void processingFinished(double);
    void exceptionOccured(const QString&);
    void dataCalculated(const QHash<QString, QVariantList>&);

private:
    void runLuaFunction(const sol::protected_function &fun);
    QStringList getGlobals();

private:
    QString m_scriptPath;
    State m_state;

    sol::state m_lua;
    sol::protected_function m_setup;
    sol::protected_function m_process;
    LuaLogger m_logger;

    int m_yearsFrom;
    int m_yearsTo;
    int m_months;

    QList<QSharedPointer<ScriptFile>> m_files;
};

#endif // SCRIPTENGINELUA_H
