#include "chart.h"

Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags):QtCharts::QChart(parent, wFlags)
{

}

void Chart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QtCharts::QChart::paint(painter, option, widget);
}
