#ifndef WIDGETLUAEDITOR_H
#define WIDGETLUAEDITOR_H

#include <QObject>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <script/luacheck.h>

class WidgetLuaEditor : public QsciScintilla
{
    Q_OBJECT
    Q_DISABLE_COPY(WidgetLuaEditor)
public:
    explicit WidgetLuaEditor(QWidget *parent = 0);

    void setFilePath(const QString& path);

    bool hasUndo() const;
    bool hasRedo() const;
    void resetUndo();

    void setText(const QString &text) override;

public slots:
    void setCheckResults(const QList<LuaCheckResult>&);

private slots:
    void onTextChanged();

signals:
    void undoChanged(bool);
    void redoChanged(bool);

private:
    QsciLexerLua m_lexer;
    QString m_filePath;
    QList<LuaCheckResult> m_checkResults;
};

#endif // WIDGETLUAEDITOR_H
