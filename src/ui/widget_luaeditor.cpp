#include "widget_luaeditor.h"
#include <QsLog.h>
#include <QApplication>
#include <QProcess>
#include <script/luacheck.h>

WidgetLuaEditor::WidgetLuaEditor(QWidget *parent) : QsciScintilla(parent)
{
    m_lexer.setFoldCompact(true);
    m_lexer.setColor(QColor(128, 128, 255), 5);
    m_lexer.setColor(QColor(100, 100, 100), 1);
    m_lexer.setColor(QColor(100, 100, 100), 2);
    m_lexer.setColor(QColor(Qt::red), 6);
    m_lexer.setColor(QColor(Qt::red), 8);
    m_lexer.setColor(QColor(255, 128, 0), 13);
    m_lexer.setColor(QColor(51, 102, 255), 15);
    m_lexer.setColor(QColor(72, 61, 139), 10);
    m_lexer.setFont(QFont("Consolas", 11, QFont::Normal));
    setLexer(&m_lexer);

    this->setUtf8(true);
    this->setMarginLineNumbers(1, true);
    this->setMarginWidth(1, 35);
    this->setTabIndents(true);
    this->setIndentationGuides(true);
    this->setIndentationsUseTabs(true);
    this->setAutoIndent(true);
    this->setTabWidth(4);
    this->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    this->setBraceMatching(QsciScintilla::StrictBraceMatch);
    this->setWrapMode(QsciScintilla::WrapNone);

    setAutoCompletionSource(QsciScintilla::AcsAll);
    setAutoCompletionCaseSensitivity(false);
    setAutoCompletionFillupsEnabled(true);
    setAutoCompletionThreshold(2);
    setAutoCompletionShowSingle(true);

    SendScintilla(SCI_MARKERDEFINE, 1, SC_MARK_LEFTRECT); //warnings
    SendScintilla(SCI_MARKERSETFORE, 1, 0x33BEFF);
    SendScintilla(SCI_MARKERSETBACK, 1, 0x33BEFF);
    SendScintilla(SCI_MARKERDEFINE, 2, SC_MARK_MINUS); //errors
    SendScintilla(SCI_MARKERSETFORE, 2, 0x0000FF);
    SendScintilla(SCI_MARKERSETBACK, 2, 0x0000FF);

    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void WidgetLuaEditor::setFilePath(const QString &path)
{
    m_filePath = path;
}

bool WidgetLuaEditor::hasUndo() const
{
    return isUndoAvailable();
}

bool WidgetLuaEditor::hasRedo() const
{
    return isRedoAvailable();
}

void WidgetLuaEditor::resetUndo()
{
    //FIXME: Dirty hack to reset undo/redo stack
    int line, column;
    getCursorPosition(&line, &column);
    blockSignals(true);
    setText(text());
    blockSignals(false);
    setCursorPosition(line, column);
    setCheckResults(m_checkResults);
}

void WidgetLuaEditor::setText(const QString &text)
{
    QsciScintilla::setText(text);
}

void WidgetLuaEditor::setCheckResults(const QList<LuaCheckResult>& results)
{
    m_checkResults = results;
    for(const LuaCheckResult& result : m_checkResults) {
        if(result.errorCode.startsWith("E")) {
            annotate(result.line - 1, result.message, 1);
            SendScintilla(SCI_MARKERADD, result.line - 1, 2);
        } else {
            SendScintilla(SCI_MARKERADD, result.line - 1, 1);
        }
    }
}

void WidgetLuaEditor::onTextChanged()
{
    emit undoChanged(hasUndo());
    emit redoChanged(hasRedo());
}

