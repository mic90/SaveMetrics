#include "ui/window_charts.h"
#include "ui_window_charts.h"
#include <QsLog.h>

const unsigned long long FIVE_YEARS_IN_SEC = 60 * 60 * 24 * 365 * 5;

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

    m_horLineIndicator = new QCPItemStraightLine(&m_plot);
    m_horLineIndicator->point1->setCoords(0, 0);
    m_horLineIndicator->point2->setCoords(0, 0);
    m_horLineIndicator->setPen(QPen(Qt::black, 1.0, Qt::DashLine));
    m_verLineIndicator = new QCPItemStraightLine(&m_plot);
    m_verLineIndicator->point1->setCoords(0, 0);
    m_verLineIndicator->point2->setCoords(0, 0);
    m_verLineIndicator->setPen(QPen(Qt::black, 1.0, Qt::DashLine));

    m_plot.legend->setVisible(true);
    m_plot.setInteraction(QCP::iRangeDrag, true);
    m_plot.setInteraction(QCP::iRangeZoom, true);
    m_plot.setInteraction(QCP::iSelectLegend, true);
    m_plot.setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
    m_plot.xAxis->setTicker(dateTicker);
    //show/hide scatter based on current zoom level
    connect(m_plot.xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), this, [this](const QCPRange &newRange) {
        if(newRange.size() < FIVE_YEARS_IN_SEC && !m_scatterVisible)
        {
            m_scatterVisible = true;
            updateScatterStyle();
        }
        else if(newRange.size() >= FIVE_YEARS_IN_SEC && m_scatterVisible)
        {
            m_scatterVisible = false;
            updateScatterStyle();
        }
    });
    connect(&m_plot, &QCustomPlot::mouseMove, this, [this](QMouseEvent *event){
        if(m_plot.graphCount() == 0)
        {
            return;
        }

        double value = 0.0;
        double key = 0.0;
        m_plot.graph(0)->pixelsToCoords(QPointF(event->pos()), key, value);
        for(int i=0; i<m_tracers.size(); i++)
        {
            m_tracers.at(i)->setGraphKey(key);
            m_tracers.at(i)->updatePosition();
            auto value = m_tracers.at(i)->position->value();
            m_tracers.at(i)->graph()->setName(QString("%1 : %2").arg(m_graphNames.at(i)).arg(value));
        }
        m_horLineIndicator->point1->setCoords(key, 0);
        m_horLineIndicator->point2->setCoords(key, 1);
        m_verLineIndicator->point1->setCoords(0, value);
        m_verLineIndicator->point2->setCoords(1, value);
        m_plot.replot();

    });
    ui->verticalLayout->addWidget(&m_plot);
}

WindowCharts::~WindowCharts()
{
    delete ui;
}

void WindowCharts::onCalculationFinished(const QHash<QString, QVariantList> &data)
{
    if(data.isEmpty())
    {
        return;
    }
    while(!m_tracers.isEmpty())
    {
        m_plot.removeItem(m_tracers.takeFirst());
    }
    m_plot.clearGraphs();
    m_graphNames.clear();

    QStringList names = data.keys();
    int graphIndex = 0;
    for(const QString& name : names)
    {
        m_graphNames.append(beautifyVarName(name));
        QVariantList values = data.value(name);
        auto newGraph = m_plot.addGraph();
        newGraph->setAntialiased(true);
        newGraph->setPen(graphIndex < m_graphColors.size() ? m_graphColors.at(graphIndex) : m_graphColors.last());
        newGraph->setName(m_graphNames.last());
        QCPItemTracer *tracer = new QCPItemTracer(&m_plot);
        tracer->setGraph(newGraph);
        tracer->setStyle(QCPItemTracer::tsPlus);
        m_tracers.append(tracer);

        auto date = QDateTime::currentDateTime().addMonths(1).date();
        date.setDate(date.year(), date.month(), 1);
        for(auto const& value : values)
        {
            auto raw = value.toDouble();
            newGraph->addData(QDateTime(date).toTime_t(), raw);
            date = date.addMonths(1);
        }
        graphIndex++;
    }
    m_plot.xAxis->rescale(true);
    m_plot.yAxis->rescale(true);
    updateScatterStyle();
    m_plot.replot();
    show();
}

QString WindowCharts::beautifyVarName(const QString &name)
{
    QString copy(name);
    copy[0] = copy[0].toUpper();
    copy = copy.replace('_', ' ');
    return copy;
}

void WindowCharts::updateScatterStyle()
{
    QCPScatterStyle style = m_scatterVisible ? QCPScatterStyle(QCPScatterStyle::ssDisc, 5) :
                                               QCPScatterStyle(QCPScatterStyle::ssNone, 0);
    for(int i=0; i<m_plot.graphCount(); i++)
    {
        m_plot.graph(i)->setScatterStyle(style);
    }
}

void WindowCharts::closeEvent(QCloseEvent *)
{
    hide();
}
