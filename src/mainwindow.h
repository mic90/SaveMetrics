#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QFileSystemModel>
#include <QSpinBox>
#include <project/project.h>
#include <script/scriptenginerunner.h>
#include <ui/filesiconprovider.h>
#include <ui/window_scriptfile.h>
#include <ui/window_charts.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNewProject_triggered();
    void on_actionClose_triggered();
    void on_actionNewFile_triggered();
    void on_actionOpen_triggered();
    void on_actionRun_triggered();
    void on_actionStop_triggered();
    void on_filesView_doubleClicked(const QModelIndex &index);
    void on_actionSave_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

private:
    void populateToolbar();
    void enableProjectUI();
    void disableProjectUI();
    void closeProject();
    void initProject(std::unique_ptr<QFile> file);
    void updateTitle();
    WindowScriptFile *getActiveWidget();
    bool setActiveSubWindow(const QString& path);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<FilesIconProvider> m_iconsProvider = nullptr;
    std::unique_ptr<Project> m_project = nullptr;
    std::unique_ptr<ScriptEngineRunner> m_scriptEngine = nullptr;
    std::unique_ptr<QFileSystemModel> m_fileModel = nullptr;
    std::unique_ptr<LuaCheck> m_luaCheck = nullptr;
    std::unique_ptr<WindowCharts> m_chartsWindow = nullptr;

    QSpinBox *m_yearsFrom;
    QSpinBox *m_yearsTo;
    QCheckBox *m_runOnSave;
};

#endif // MAINWINDOW_H
