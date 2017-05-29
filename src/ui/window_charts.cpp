#include "ui/window_charts.h"
#include "ui_window_charts.h"

WindowCharts::WindowCharts(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowCharts)
{
    ui->setupUi(this);

    //fill graph colors
    m_graphColors.append(QColor("#03a9f4")); //lightblue
    m_graphColors.append(QColor("#f44336")); //red
    m_graphColors.append(QColor("#4caf50")); //green
    m_graphColors.append(QColor("#ff9800")); //orange
    m_graphColors.append(QColor("#9c27b0")); //purple
    m_graphColors.append(QColor("#673ab7")); //deeppurple
    m_graphColors.append(QColor("#cddc39")); //lime
    m_graphColors.append(QColor("#b2ff59")); //lightgreen

    m_plot.legend->setVisible(true);
    m_plot.axisRect()->setRangeDrag(Qt::Horizontal);
    m_plot.setInteraction(QCP::iRangeDrag, true);
    ui->verticalLayout->addWidget(&m_plot);
}

WindowCharts::~WindowCharts()
{
    delete ui;
}

void WindowCharts::onCalculationFinished(const QHash<QString, QVariantList> &data)
{
    m_plot.clearGraphs();

    QStringList names = data.keys();
    int graphIndex = 0;
    for(const QString& name : names)
    {
        QVariantList values = data.value(name);
        auto newGraph = m_plot.addGraph();
        newGraph->setAntialiased(true);
        newGraph->setPen(graphIndex < m_graphColors.size() ? m_graphColors.at(graphIndex) : m_graphColors.last());
        newGraph->setName(beautifyVarName(name));
        double dataIndex = 0;
        for(auto const& value : values)
        {
            newGraph->addData(dataIndex++, value.toDouble());
        }
        newGraph->rescaleKeyAxis(true);
        newGraph->rescaleValueAxis(true, false);
        graphIndex++;
    }
    m_plot.replot();
    show();
}

QString WindowCharts::beautifyVarName(const QString &name)
{
    QString copy(name);
    copy[0].toUpper();
    copy = copy.replace('_', ' ');
    return copy;
}

void WindowCharts::closeEvent(QCloseEvent *)
{
    hide();
}
