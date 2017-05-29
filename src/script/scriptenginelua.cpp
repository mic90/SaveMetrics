#include "scriptenginelua.h"
#include <QsLog.h>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QThread>
#include <iostream>

const std::string SETUP_FUNCTION = "setup";
const std::string PROCESS_FUNCTION = "process";
const QSet<QString> GLOBAL_FUNCTIONS = QSet<QString>() << "error_handler" << "setup" << "process";

void luaLineHook(lua_State *L, lua_Debug *)
{
    sol::state_view luaView(L);
    if(luaView.get<bool>("_shouldStop"))
    {
        luaL_error(L, "");
    }
}

ScriptEngineLua::ScriptEngineLua(const QString& scriptPath, int yearsFrom, int yearsTo, QObject *parent) : QObject(parent),
    m_scriptPath(scriptPath),
    m_state(Stopped),
    m_yearsFrom(yearsFrom),
    m_yearsTo(yearsTo),
    m_months((yearsTo - yearsFrom) * 12)
{
    qRegisterMetaType<ScriptEngineLua::State>("ScriptEngineLua::State");
    qRegisterMetaType<QHash<QString, QVariantList>>("QHash<QString, QVariantList>");

    m_lua.open_libraries();
    m_lua["path"] = m_scriptPath.toStdString().c_str();
    m_lua["log"] = &m_logger;
    m_lua["_shouldStop"] = false;
    m_lua.new_usertype<LuaLogger>("logger",
                                  "info", &LuaLogger::info,
                                  "warn", &LuaLogger::warn,
                                  "error", &LuaLogger::error
                                  );
    lua_sethook(m_lua.lua_state(), &luaLineHook, LUA_MASKCOUNT, 10);
}

void ScriptEngineLua::addScriptFile(QSharedPointer<ScriptFile> file)
{
    m_files.append(file);
    QLOG_DEBUG() << "Added script: " << file->getName() << ", files in memory:" << m_files.size();
}

ScriptEngineLua::State ScriptEngineLua::getState() const
{
    return m_state;
}

bool ScriptEngineLua::start()
{
    QLOG_INFO() << "Calculating for years span from" << m_yearsFrom << "to" << m_yearsTo;
    try
    {
        auto defaultGlobals = getGlobals();

        bool functionsFound = false;
        //add project path to script env
        m_lua.script("package.path = package.path .. ';' .. '" +
                     m_scriptPath.toStdString() + "' .. '/?.lua'");

        m_lua.script("package.path = package.path .. ';' .. '" +
                     qApp->applicationDirPath().toStdString() + "/luarocks/systree/share/lua/5.1" + "' .. '/?.lua'");
        m_lua.script("package.cpath = package.cpath .. ';' .. '" +
                     qApp->applicationDirPath().toStdString() + "/luarocks/systree/lib/lua/5.1" + "' .. '/?.dll'");
        m_lua.script("function error_handler(error_msg ) return error_msg; end");

        for(QSharedPointer<ScriptFile> file : m_files)
        {
            QString content = file->getData();

            m_lua.script_file(file->getPath().toStdString());
            m_setup = m_lua[SETUP_FUNCTION];
            m_process = m_lua[PROCESS_FUNCTION];
            if(m_setup.valid() && m_process.valid())
            {
                QLOG_INFO() << "setup() and process() functions were found in file:" << file->getName();
                functionsFound = true;
                break;
            }
            m_setup.error_handler = m_lua["error_handler"];
            m_process.error_handler = m_lua["error_handler"];
        }
        if(!functionsFound)
        {
            emit exceptionOccured("Couldn't find setup() or process() functions");
            return false;
        }
        m_state = Running;
        emit stateChanged(m_state);

        emit processingStarted();
        QElapsedTimer timer;
        timer.start();

        runLuaFunction(m_setup);
        int currentMonth = 1;
        int currentYear = m_yearsFrom;
        QHash<QString, QVariantList> globals;
        for(int i=1; i<m_months; i++)
        {
            //FIXME: change runLuaFunction to take arguments and remove this duplicated code
            sol::protected_function_result ret = m_process(currentMonth, currentYear);
            sol::call_status stat = ret.status();
            if((int)stat >= 2)
            {
                throw sol::error(ret.get<std::string>());
            }

            //get global variables values
            auto scriptGlobals = getGlobals();
            QStringList userGlobals = scriptGlobals.toSet()
                    .subtract(defaultGlobals.toSet())
                    .subtract(GLOBAL_FUNCTIONS)
                    .toList();
            for(const auto& varName : userGlobals)
            {
                sol::object luaObject = m_lua[varName.toStdString()];
                if(luaObject.get_type() != sol::type::number)
                {
                    continue;
                }
                double value = m_lua[varName.toStdString()];
                if(globals.contains(varName))
                {
                    auto values = globals.value(varName);
                    values.append(value);
                    globals.insert(varName, values);
                }
                else
                {
                    globals.insert(varName, QVariantList() << value);
                }
            }

            currentMonth++;
            if(currentMonth == 13)
            {
                currentMonth = 1;
                currentYear++;
            }
        }

        m_state = Stopped;
        double executionTime = static_cast<double>(timer.nsecsElapsed() / 1000000.0f); //convert to ms
        QLOG_DEBUG() << "Processing finished with time:" << executionTime << " ms";
        emit processingFinished(executionTime);
        emit stateChanged(m_state);
        emit dataCalculated(globals);
        return true;
    }
    catch(const sol::error &e)
    {
        m_state = Stopped;
        emit stateChanged(m_state);
        //empty reason means user requested stop
        QString reason(e.what());
        QLOG_DEBUG() << reason;
        if(reason != "lua: error: ")
        {
            emit exceptionOccured(QString(e.what()));
        }
        return false;
    }
}

void ScriptEngineLua::stop()
{
    m_lua["_shouldStop"] = true;
}

void ScriptEngineLua::runLuaFunction(const sol::protected_function &fun)
{
    sol::protected_function_result ret = fun();
    sol::call_status stat = ret.status();
    if((int)stat >= 2)
    {
        throw sol::error(ret.get<std::string>());
    }
}

QStringList ScriptEngineLua::getGlobals()
{
    sol::table globalData = m_lua.script("local data = {};\n"
                                         "for k,v in pairs(_G) do\n"
                                         "  table.insert(data, 1, k);\n"
                                         "end\n"
                                         "return data;");
    QStringList globals;
    for(unsigned int i=1; i<globalData.size(); i++)
    {
        std::string name = globalData[i];
        globals.append(QString::fromStdString(name));
    }
    return globals;
}
