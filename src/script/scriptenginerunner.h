#ifndef SCRIPTENGINERUNNER_H
#define SCRIPTENGINERUNNER_H

#include <QObject>
#include <QThread>
#include <script/scriptenginelua.h>

class ScriptEngineRunner : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ScriptEngineRunner)
public:
    ScriptEngineRunner(const QString& scriptPath, int yearsFrom, int yearsTo, QObject *parent = 0);
    ~ScriptEngineRunner();

    void start();
    bool startSynced();
    void stop();

    void addScriptFile(QSharedPointer<ScriptFile> file);
    void addScriptFiles(QList<QSharedPointer<ScriptFile>> files);

private slots:
    void onExceptionOccured(const QString&);

signals:
    void exceptionOccured(const QString&);
    void engineStateChanged(ScriptEngineLua::State);
    void processingFinished(double);
    void startProcessing();
    void dataCalculated(const QHash<QString, QVariantList>&);

private:
    QThread m_scriptThread;
    ScriptEngineLua *m_engine;
    QString m_scriptPath;
};

#endif // SCRIPTENGINERUNNER_H
