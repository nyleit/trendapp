#ifndef CHART_H
#define CHART_H

#include <QtCharts>

class Chart : public QtCharts::QChart
{
    Q_OBJECT
public:
    explicit Chart(QGraphicsItem *parent = Q_NULLPTR, Qt::WindowFlags wFlags = Qt::WindowFlags());

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
};

#endif // CHART_H
