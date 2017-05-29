#include "lualogger.h"
#include <QsLog.h>

LuaLogger::LuaLogger(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<std::string>("std::string");
//    connect(this, SIGNAL(logInfo(std::string)), this, SLOT(onLogInfo(std::string)));
//    connect(this, SIGNAL(logWarn(std::string)), this, SLOT(onLogWarn(std::string)));
//    connect(this, SIGNAL(logError(std::string)), this, SLOT(onLogError(std::string)));
}

void LuaLogger::info(const std::__cxx11::string &message)
{
    QLOG_INFO() << QString::fromStdString(message);
//    emit logInfo("[<OUT>]" + message);
}

void LuaLogger::warn(const std::__cxx11::string &message)
{
    QLOG_WARN() << QString::fromStdString(message);
//    emit logWarn("[<OUT>]" + message);
}

void LuaLogger::error(const std::__cxx11::string &message)
{
    QLOG_ERROR() << QString::fromStdString(message);
//    emit logError("[<OUT>]" + message);
}

void LuaLogger::onLogInfo(const std::__cxx11::string &message)
{
    QLOG_INFO() << message.c_str();
}

void LuaLogger::onLogWarn(const std::__cxx11::string &message)
{
    QLOG_WARN() << message.c_str();
}

void LuaLogger::onLogError(const std::__cxx11::string &message)
{
    QLOG_ERROR() << message.c_str();
}
