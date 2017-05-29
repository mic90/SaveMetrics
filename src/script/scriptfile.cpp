#include "scriptfile.h"
#include <QsLog.h>

ScriptFile::ScriptFile(QString const &path, ScriptFile::ScriptType type, QObject *parent) : ProjectFile(path, parent),
    m_type(type)
{
}

ScriptFile::~ScriptFile()
{
}

ScriptFile::ScriptType ScriptFile::getType() const
{
    return m_type;
}

const QList<LuaCheckResult> &ScriptFile::getSyntaxErrors() const
{
    return m_syntaxErrors;
}

void ScriptFile::setSyntaxErrors(const QList<LuaCheckResult> &results)
{
    m_syntaxErrors = results;
    emit syntaxErrors(m_syntaxErrors);
}

