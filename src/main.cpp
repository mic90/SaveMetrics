#include "mainwindow.h"
#include <QApplication>
#include <QsLog.h>
#include <QsLogDest.h>
#include <QDir>

using namespace QsLogging;

void setupLogger();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setupLogger();
    MainWindow w;
    w.show();

    return a.exec();
}

void setupLogger()
{
    //Setup logger, logs will be saved to app_dir/logs/log.txt files
    Logger& logger = Logger::instance();
#ifdef QT_DEBUG
    logger.setLoggingLevel(QsLogging::TraceLevel);
#else
    logger.setLoggingLevel(QsLogging::InfoLevel);
#endif
    QDir logsDir(qApp->applicationDirPath() + "/logs/");
    if(!logsDir.exists()) {
        logsDir.mkdir(logsDir.path());
    };
    const QString sLogPath(logsDir.filePath("log.txt"));

    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
       sLogPath, EnableLogRotation, MaxSizeBytes(2000000), MaxOldLogCount(2)));
    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);
}
