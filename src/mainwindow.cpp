#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QsLog.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QLabel>
#include <QCheckBox>
#include <script/luarocks.h>

const QString APP_NAME = "SaveMetrics";
const QString VERSION = "0.8.0";
const int STATUS_MESSAGE_TIMEOUT = 2000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_iconsProvider(new FilesIconProvider()),
    m_chartsWindow(new WindowCharts(this))
{
    ui->setupUi(this);
    m_chartsWindow->hide();
    populateToolbar();
    updateTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateToolbar()
{
    ui->toolBar->addAction(ui->actionNewProject);
    ui->toolBar->addAction(ui->actionNewFile);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addAction(ui->actionSave_as);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionUndo);
    ui->toolBar->addAction(ui->actionRedo);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionRun);
    ui->toolBar->addAction(ui->actionStop);
    ui->toolBar->addWidget(new QLabel("  Years from "));
    m_yearsFrom = new QSpinBox(this);
    m_yearsFrom->setMinimum(1);
    m_yearsFrom->setMaximum(100);
    m_yearsFrom->setEnabled(false);
    connect(m_yearsFrom, &QSpinBox::editingFinished, this, [this](){
        if(m_yearsFrom->value() >= m_yearsTo->value())
        {
            m_yearsTo->setValue(m_yearsFrom->value() + 1);
        }
        m_project->setYearFrom(m_yearsFrom->value());
        m_project->setYearTo(m_yearsFrom->value());
    });
    m_yearsTo = new QSpinBox(this);
    m_yearsTo->setMinimum(2);
    m_yearsTo->setMaximum(100);
    m_yearsTo->setEnabled(false);
    connect(m_yearsTo, &QSpinBox::editingFinished, this, [this](){
        if(m_yearsTo->value() <= m_yearsFrom->value())
        {
            m_yearsTo->setValue(m_yearsFrom->value() + 1);
        }
        m_project->setYearTo(m_yearsTo->value());
    });
    ui->toolBar->addWidget(m_yearsFrom);
    ui->toolBar->addWidget(new QLabel(" to "));
    ui->toolBar->addWidget(m_yearsTo);
    ui->toolBar->addSeparator();
    m_runOnSave = new QCheckBox("Run on save", this);
    m_runOnSave->setEnabled(false);
    m_runOnSave->setChecked(true);
    connect(m_runOnSave, &QCheckBox::toggled, this, [this](bool checked) {
        m_project->setRunOnSave(checked);
    });
    ui->toolBar->addWidget(m_runOnSave);
}

void MainWindow::enableProjectUI()
{
    ui->actionNewFile->setEnabled(true);
    ui->actionProjectSettings->setEnabled(true);
    ui->actionClose->setEnabled(true);
    ui->actionRun->setEnabled(true);
    m_yearsFrom->setEnabled(true);
    m_yearsTo->setEnabled(true);
    m_runOnSave->setEnabled(true);
}

void MainWindow::disableProjectUI()
{
    ui->actionNewFile->setEnabled(false);
    ui->actionProjectSettings->setEnabled(false);
    ui->actionClose->setEnabled(false);
    ui->actionRun->setEnabled(false);
    ui->filesView->setModel(nullptr);
    m_yearsFrom->setEnabled(false);
    m_yearsTo->setEnabled(false);
    m_runOnSave->setEnabled(false);
}

void MainWindow::closeProject()
{
    disableProjectUI();
    QString projectName = m_project->name();
    if(m_scriptEngine)
    {
        m_scriptEngine->stop();
        m_scriptEngine.reset();
    }
    m_project.reset();
    m_luaCheck.reset();
    m_fileModel.reset();
    updateTitle();
    QLOG_INFO() << "Project closed:" << projectName;
}

void MainWindow::initProject(std::unique_ptr<QFile> file)
{
    if(m_project != nullptr)
    {
        closeProject();
    }
    QString path = file->fileName();
    QString dirPath = QFileInfo(path).absolutePath();
    m_project = std::make_unique<Project>(std::move(file), path);
    m_fileModel = std::make_unique<QFileSystemModel>();
    m_fileModel->setRootPath(dirPath);
    m_fileModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_fileModel->setNameFilterDisables(false);
    m_fileModel->setNameFilters(QStringList() << "*.lua");
    m_fileModel->setIconProvider(m_iconsProvider.get());
    m_luaCheck = std::make_unique<LuaCheck>(new LuaCheck());
    ui->filesView->setModel(m_fileModel.get());
    ui->filesView->setRootIndex(m_fileModel->index(dirPath));
    ui->filesView->hideColumn(1);
    ui->filesView->hideColumn(2);
    ui->filesView->hideColumn(3);
    ui->filesView->hideColumn(4);
    connect(m_fileModel.get(), &QFileSystemModel::directoryLoaded, this, [this]() {
        ui->filesView->resizeColumnToContents(0);
    });
    enableProjectUI();
    updateTitle();

    m_yearsTo->blockSignals(true);
    m_yearsFrom->blockSignals(true);
    m_runOnSave->blockSignals(true);
    m_yearsTo->setValue(m_project->getYearTo());
    m_yearsFrom->setValue(m_project->getYearFrom());
    m_runOnSave->setChecked(m_project->getRunOnSave());
    m_yearsTo->blockSignals(false);
    m_yearsFrom->blockSignals(false);
    m_runOnSave->blockSignals(false);
}

void MainWindow::updateTitle()
{
    QString append = m_project == nullptr ? "" : QString(" - %1").arg(m_project->name());
    QString title = QString("%1 v.%2 %3").arg(APP_NAME, VERSION, append);
    setWindowTitle(title);
}

WindowScriptFile *MainWindow::getActiveWidget()
{
    QMdiSubWindow *wnd = ui->mdiArea->currentSubWindow();
    if(wnd == nullptr)
    {
        return nullptr;
    }
    WindowScriptFile *widget = qobject_cast<WindowScriptFile*>(wnd->widget());
    if(widget == nullptr)
    {
        QLOG_ERROR() << "Unknown window widget";
        return nullptr;
    }
    return widget;
}

bool MainWindow::setActiveSubWindow(const QString &path)
{
    for(auto subWindow : ui->mdiArea->subWindowList())
    {
        WindowScriptFile *widget = qobject_cast<WindowScriptFile*>(subWindow->widget());
        if(widget == nullptr)
        {
            continue;
        }
        if(widget->getFilePath() == path)
        {
            ui->mdiArea->setActiveSubWindow(subWindow);
            return true;
        }
    }
    return false;
}

void MainWindow::on_actionNewProject_triggered()
{
    QString path = QFileDialog::getSaveFileName(this, "New project", QString(), "Project files (*.json)");
    if(path.isEmpty())
    {
        return;
    }
    auto projectFile = std::make_unique<QFile>(path);
    initProject(std::move(projectFile));
    QLOG_INFO() << "New project created at:" << path;
}

void MainWindow::on_actionClose_triggered()
{
    closeProject();
}

void MainWindow::on_actionNewFile_triggered()
{
    QString path = QFileDialog::getSaveFileName(this, "New file", QString(), "Lua files (*.lua)");
    if(path.isEmpty())
    {
        return;
    }
    QFile projectFile(path);
    if(!projectFile.open(QIODevice::WriteOnly))
    {
        QString message = "Couldn't create lua file at " + path;
        QLOG_ERROR() << message;
        QMessageBox::critical(this, "Error", message);
        return;
    }
    QLOG_INFO() << "New file created at:" << path;
}

void MainWindow::on_actionOpen_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "Open file or project", QString(),
                                                "Project files (*.json);; Lua files (*.lua)");
    if(path.isEmpty())
    {
        return;
    }
    QFileInfo fileInfo(path);
    auto extension = fileInfo.completeSuffix();
    if(extension == "json")
    {
        auto projectFile = std::make_unique<QFile>(path);
        initProject(std::move(projectFile));
        QLOG_INFO() << "Project opened:" << path;
    }
    else if(extension == "lua")
    {
        QMessageBox::critical(this, "Error", "Operation unsupported");
    }
}

void MainWindow::on_actionRun_triggered()
{
    auto luaFiles = m_project->getLuaFiles();
    QList<QSharedPointer<ScriptFile>> files;
    for(auto const& filePath : luaFiles)
    {
        QSharedPointer<ScriptFile> newFile(new ScriptFile(filePath));
        if(!newFile->load())
        {
            QLOG_ERROR() << "Couldn't load lua file at:" << filePath;
            continue;
        }
        files.append(newFile);
    }
    if(files.isEmpty())
    {
        QString message = "Can't run simulation, no lua files were found";
        QLOG_WARN() << message;
        QMessageBox::warning(this, "Warning", message);
        return;
    }

    if(m_scriptEngine)
    {
        m_scriptEngine->stop();
        m_scriptEngine.reset();
    }
    m_scriptEngine = std::make_unique<ScriptEngineRunner>(m_project->dirPath(), m_yearsFrom->value(), m_yearsTo->value());
    connect(m_scriptEngine.get(), &ScriptEngineRunner::exceptionOccured, this, [this](QString const& message) {
        QMessageBox::critical(this, "Exception", "Lua exception: " + message);
    });
    connect(m_scriptEngine.get(), &ScriptEngineRunner::engineStateChanged, this, [this](ScriptEngineLua::State state) {
        QLOG_DEBUG() << "Engine state changed:" << state;
        bool isRunning = state == ScriptEngineLua::Running;
        ui->actionRun->setEnabled(!isRunning);
        ui->actionStop->setEnabled(isRunning);
    });
    connect(m_scriptEngine.get(), &ScriptEngineRunner::processingFinished, this, [this](double execTime) {
        ui->statusBar->showMessage(QString("Script execution finished with time %1 ms").arg(execTime), STATUS_MESSAGE_TIMEOUT);
    });
    connect(m_scriptEngine.get(), &ScriptEngineRunner::dataCalculated, m_chartsWindow.get(), &WindowCharts::onCalculationFinished);
    m_scriptEngine->addScriptFiles(files);
    m_scriptEngine->start();
}

void MainWindow::on_actionStop_triggered()
{
    m_scriptEngine->stop();
}

void MainWindow::on_filesView_doubleClicked(const QModelIndex &index)
{
    auto path = m_fileModel->filePath(index);
    QFileInfo fileInfo(path);
    if(fileInfo.isDir())
    {
        return;
    }
    if(setActiveSubWindow(path))
    {
        return;
    }
    QSharedPointer<ScriptFile> scriptFile(new ScriptFile(path));
    if(!scriptFile->load())
    {
        QString message = "Couldn't load file from: " + path;
        QLOG_ERROR() << message;
        QMessageBox::critical(this, "Error", message);
        return;
    }
    scriptFile->setSyntaxErrors(m_luaCheck->checkFile(path));
    connect(scriptFile.data(), &ScriptFile::fileSaved, this, [this]() {
        ScriptFile *file = qobject_cast<ScriptFile*>(sender());
        if(!file)
        {
            return;
        }
        file->setSyntaxErrors(m_luaCheck->checkFile(file->getPath()));
        if(file->getSyntaxErrors().isEmpty() && m_runOnSave->isChecked())
        {
            on_actionRun_triggered();
        }
    });
    WindowScriptFile *widget = new WindowScriptFile(scriptFile);
    QMdiSubWindow *wnd = ui->mdiArea->addSubWindow(widget);
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->setWindowIcon(QIcon("://icons/script/script_code.png"));
    wnd->show();
    connect(widget, &WindowScriptFile::undoChanged, this, [this](bool undo){
        ui->actionSave->setEnabled(undo);
        ui->actionUndo->setEnabled(undo);
    });
    connect(widget, &WindowScriptFile::redoChanged, this, [this](bool redo){
        ui->actionRedo->setEnabled(redo);
    });
}

void MainWindow::on_actionSave_triggered()
{
    auto widget = getActiveWidget();
    if(!widget->save())
    {
        QString message = "Couldn't save file at: " + widget->getFilePath();
        QLOG_ERROR() << message;
        QMessageBox::critical(this, "Error", message);
    }
    ui->statusBar->showMessage("File saved: " + widget->getFilePath(), STATUS_MESSAGE_TIMEOUT);
}

void MainWindow::on_actionUndo_triggered()
{
    auto widget = getActiveWidget();
    widget->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    auto widget = getActiveWidget();
    widget->redo();
}
