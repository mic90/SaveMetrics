#ifndef WINDOW_SCRIPTFILE_H
#define WINDOW_SCRIPTFILE_H

#include <QDialog>
#include <QUndoStack>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

#include <script/scriptfile.h>
#include <ui/widget_luaeditor.h>

namespace Ui {
class WindowScriptFile;
}

class WindowScriptFile : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(WindowScriptFile)
public:
    WindowScriptFile(QSharedPointer<ScriptFile> file, QWidget *parent = 0);
    ~WindowScriptFile();

    bool load();
    bool save();
    bool hasUndo();
    bool hasRedo();

    QString getSource() const;
    QString getFilePath() const;
    void setSource(QString);

signals:
    void textChanged(QString);
    void undoChanged(bool);
    void redoChanged(bool);

public slots:
    void undo();
    void redo();
    void clearUndoStack();

private slots:
    void onEditorTextChanged();
    void onUndoChanged(bool);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::WindowScriptFile *ui;
    QSharedPointer<ProjectFile> m_file;
    WidgetLuaEditor *editor;
};

#endif // WINDOW_SCRIPTFILE_H
