#ifndef LUALOGGER_H
#define LUALOGGER_H

#include <QObject>

class LuaLogger : public QObject
{
    Q_OBJECT
public:
    explicit LuaLogger(QObject *parent = 0);

    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

signals:
    void logInfo(const std::string& message);
    void logWarn(const std::string& message);
    void logError(const std::string& message);

private slots:
    void onLogInfo(const std::string& message);
    void onLogWarn(const std::string& message);
    void onLogError(const std::string& message);
};

#endif // LUALOGGER_H
