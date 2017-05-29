#ifndef WINDOW_CHARTS_H
#define WINDOW_CHARTS_H

#include <QMainWindow>
#include <qcustomplot.h>

namespace Ui {
class WindowCharts;
}

class WindowCharts : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowCharts(QWidget *parent = 0);
    ~WindowCharts();

public slots:
    void onCalculationFinished(const QHash<QString, QVariantList>&);

private:
    QString beautifyVarName(const QString& name);
    void updateScatterStyle();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::WindowCharts *ui;
    QCustomPlot m_plot;
    QCPItemStraightLine *m_horLineIndicator;
    QCPItemStraightLine *m_verLineIndicator;
    QList<QCPItemTracer*> m_tracers;
    QStringList m_graphNames;
    QList<QColor> m_graphColors;
    bool m_scatterVisible = false;
};

#endif // WINDOW_CHARTS_H
