#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QObject>
#include <QChartView>

class ChartView : public QtCharts::QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent=0);
};

#endif // CHARTVIEW_H
