#ifndef SCRIPTFILE_H
#define SCRIPTFILE_H

#include <project/projectfile.h>
#include <script/luacheck.h>

class ScriptFile : public ProjectFile
{
    Q_OBJECT
    Q_DISABLE_COPY(ScriptFile)
public:
    enum ScriptType { Source = 0, Test = 1 };
    ScriptFile(QString const &path, ScriptFile::ScriptType = Source, QObject *parent = 0);
    ~ScriptFile();

    ScriptType getType() const;
    const QList<LuaCheckResult>& getSyntaxErrors() const;
    void setSyntaxErrors(const QList<LuaCheckResult>& results);

signals:
    void syntaxErrors(const QList<LuaCheckResult>& results);

private:
    ScriptType m_type;
    QList<LuaCheckResult> m_syntaxErrors;
};

#endif // SCRIPTFILE_H
