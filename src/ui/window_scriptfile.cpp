#include "ui/window_scriptfile.h"
#include "ui_window_scriptfile.h"

WindowScriptFile::WindowScriptFile(QSharedPointer<ScriptFile> file, QWidget *parent) : QDialog(parent),
    ui(new Ui::WindowScriptFile),
    m_file(file)
{
    ui->setupUi(this);
    //set window properties
    setWindowTitle(file->getName());
    //delete widget on close
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    editor = new WidgetLuaEditor(this);
    editor->setFilePath(file->getPath());
    connect(editor, SIGNAL(undoChanged(bool)), this, SIGNAL(undoChanged(bool)));
    connect(editor, SIGNAL(undoChanged(bool)), this, SLOT(onUndoChanged(bool)));
    connect(editor, SIGNAL(redoChanged(bool)), this, SIGNAL(redoChanged(bool)));
    connect(editor, SIGNAL(textChanged()), this, SLOT(onEditorTextChanged()));
    connect(file.data(), &ScriptFile::syntaxErrors, editor, &WidgetLuaEditor::setCheckResults);
    ui->verticalLayout->addWidget(editor);

    editor->blockSignals(true);
    editor->setText(file->getData());
    editor->blockSignals(false);

    editor->setCheckResults(file->getSyntaxErrors());
}

WindowScriptFile::~WindowScriptFile()
{
    delete ui;
}

QString WindowScriptFile::getSource() const
{
    return editor->text();
}

QString WindowScriptFile::getFilePath() const
{
    return m_file->getPath();
}

void WindowScriptFile::setSource(QString source)
{
    editor->setText(source);
    editor->setCursorPosition(0,0);
}

bool WindowScriptFile::hasUndo()
{
    return editor->hasUndo();
}

bool WindowScriptFile::hasRedo()
{
    return editor->hasRedo();
}

void WindowScriptFile::undo()
{
    editor->undo();
}

void WindowScriptFile::redo()
{
    editor->redo();
}

void WindowScriptFile::clearUndoStack()
{
    editor->resetUndo();
    //manually reset signals to disable undo/redo buttons
    emit undoChanged(false);
    emit redoChanged(false);
    //remove * sign from window title
    onUndoChanged(false);
}

void WindowScriptFile::onEditorTextChanged()
{
    //update file contents
    m_file->setData(editor->text());
    emit textChanged(editor->text());
}

void WindowScriptFile::onUndoChanged(bool hasUndo)
{
    if(hasUndo)
    {
        setWindowTitle(m_file->getName() + "*");
    }
    else
    {
        setWindowTitle(m_file->getName());
    }
}

void WindowScriptFile::closeEvent(QCloseEvent *event)
{
    if(!hasUndo())
    {
        event->accept();
    }
    else
    {
        int res = QMessageBox::question(this,tr("Unsaved changes"),tr("There are unsaved changes in this file, "
                                                                      "are you sure you want to close it ?"));
        if(res == QMessageBox::Yes)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

bool WindowScriptFile::load()
{
    QSharedPointer<ScriptFile> script = m_file.objectCast<ScriptFile>();
    bool loaded = script->load();
    editor->blockSignals(true);
    editor->setText(script->getData());
    editor->setCheckResults(script->getSyntaxErrors());
    editor->blockSignals(false);
    return loaded;
}

bool WindowScriptFile::save()
{
    m_file->setData(editor->text());
    if(m_file->save())
    {
        clearUndoStack();
        return true;
    }
    return false;
}
