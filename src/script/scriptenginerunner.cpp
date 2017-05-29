#include "scriptenginerunner.h"
#include <QsLog.h>
#include <script/scriptenginelua.h>
#include <QtConcurrent/QtConcurrent>

ScriptEngineRunner::ScriptEngineRunner(const QString &scriptPath, int yearsFrom, int yearsTo, QObject *parent) : QObject(parent),
    m_engine(nullptr),
    m_scriptPath(scriptPath)
{
    m_scriptThread.setObjectName("ScriptRunnerThread");
    m_scriptThread.start();

    m_engine = new ScriptEngineLua(m_scriptPath, yearsFrom, yearsTo);
    m_engine->moveToThread(&m_scriptThread);

    connect(m_engine, &ScriptEngineLua::exceptionOccured, this, &ScriptEngineRunner::exceptionOccured);
    connect(m_engine, &ScriptEngineLua::exceptionOccured, this, &ScriptEngineRunner::onExceptionOccured);
    connect(m_engine, &ScriptEngineLua::processingFinished, this, &ScriptEngineRunner::processingFinished);
    connect(m_engine, &ScriptEngineLua::stateChanged, this, &ScriptEngineRunner::engineStateChanged);
    connect(m_engine, &ScriptEngineLua::dataCalculated, this, &ScriptEngineRunner::dataCalculated);
    connect(&m_scriptThread, &QThread::finished, m_engine, &ScriptEngineLua::deleteLater);
    connect(this, &ScriptEngineRunner::startProcessing, m_engine, &ScriptEngineLua::start);
}

ScriptEngineRunner::~ScriptEngineRunner()
{
    if(m_engine)
    {
        m_engine->blockSignals(true);
    }
    stop();
    m_scriptThread.terminate();
}

void ScriptEngineRunner::start()
{
    emit startProcessing();
    QLOG_INFO() << "Engine started";
}

bool ScriptEngineRunner::startSynced()
{
    return m_engine->start();
}

void ScriptEngineRunner::stop()
{
    if(m_engine)
    {
        m_engine->stop();
        while(m_engine->getState() != ScriptEngineLua::Stopped){}
        QLOG_INFO() << "Engine stopped";
        delete m_engine;
        m_engine = nullptr;
    }
}

void ScriptEngineRunner::addScriptFile(QSharedPointer<ScriptFile> file)
{
    m_engine->addScriptFile(file);
}

void ScriptEngineRunner::addScriptFiles(QList<QSharedPointer<ScriptFile> > files)
{
    for(const QSharedPointer<ScriptFile>& file : files)
    {
        addScriptFile(file);
    }
}

void ScriptEngineRunner::onExceptionOccured(const QString &message)
{
    QLOG_ERROR() << "Engine exception occured, stopping execution:" << message;
    stop();
}
