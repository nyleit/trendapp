
#include <QLineSeries>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QDateTime>
#include <QMessageBox>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QPrinter>
#include <QPrintDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mChart = new QtCharts::QChart();

    mXaxis = new QtCharts::QDateTimeAxis();
    mXaxis->setTitleFont(QFont("", 12, QFont::Bold));
    mXaxis->setLabelsAngle(20);
    mChart->addAxis(mXaxis, Qt::AlignBottom);

    mYaxis = new QtCharts::QValueAxis();
    mYaxis->setTitleFont(QFont("", 12, QFont::Bold));
    mYaxis->setLabelFormat("%i");
    mYaxis->setTitleText("Values");

    mChart->addAxis(mYaxis, Qt::AlignLeft);

    ui->chartView->setChart(mChart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    setupActions();
    setupMenus();



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupActions()
{
    mOpenFile = new QAction("Open File", this);
    connect(mOpenFile, &QAction::triggered, [this](){
        QString fileName = QFileDialog::getOpenFileName(this, "Open CSV file", "", tr("CSV Files (*.csv)"));
        if (!fileName.isEmpty())
        {
            readCSVFile(fileName);
        }
    });

    mPrintAction = new QAction("Print");
    connect(mPrintAction, &QAction::triggered, [this](){
        QPrinter printer;

        QPrintDialog dialog(&printer, this);
        dialog.setWindowTitle(tr("Print Document"));

        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        printer.setOrientation(QPrinter::Landscape);
        QPainter painter;
        painter.begin(&printer);


        double xscale = printer.pageRect().width()/double(ui->chartView->width());
        double yscale = printer.pageRect().height()/double(ui->chartView->height());
        double scale = qMin(xscale, yscale);
        painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                          printer.paperRect().y() + printer.pageRect().height()/2);
        painter.scale(scale, scale);
        painter.translate(-ui->chartView->width()/2, -ui->chartView->height()/2);


        ui->chartView->render(&painter);
    });

}

void MainWindow::setupMenus()
{
    mFileMenu = new QMenu("File", this);
    mFileMenu->addAction(mOpenFile);

    mDataMenu = new QMenu("Data", this);
    mLinesMenu = new QMenu("Lines", this);
    mDataMenu->addMenu(mLinesMenu);

    mPrintMenu = new QMenu("Print", this);
    mPrintMenu->addAction(mPrintAction);

    ui->menuBar->addMenu(mFileMenu);
    ui->menuBar->addMenu(mDataMenu);
    ui->menuBar->addMenu(mPrintMenu);
}

void MainWindow::readCSVFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QStringList dataSeries;
        QMap<QString, QList<float>> dataValues;
        if (!file.atEnd())
        {
            //read header
            QString header = file.readLine();
            QStringList headers = header.split(",");
            for (int i=1; i<headers.size(); i++)
            {
                dataSeries.append(headers[i]);
                dataValues.insert(headers[i], QList<float>());
            }
        }
        QList<QDateTime> timeStamps;

        while(!file.atEnd())
        {
            QString line = file.readLine();
            QStringList data = line.split(",");
            QDateTime ts = QDateTime::fromString(data[0], "M/d/yyyy hh:mm:ss AP");
            if (!ts.isValid())
                continue;

            timeStamps.append(ts);
            if (data.size()-1 != dataSeries.size())
            {
                /*QMessageBox::critical(this, "Error", "Error in csv file data");
                return;*/
                continue;
            }
            for (int i=1; i<data.size(); i++)
            {
                dataValues[dataSeries[i-1]].append(data[i].toFloat());
            }

        }

        mSeriesMax.clear();
        mSeriesMin.clear();
        //min max calulcation
        QMapIterator<QString, QList<float>> i(dataValues);
        while (i.hasNext())
        {
            i.next();
            QString sName = i.key();
            QList<float> data = i.value();

            float min_ = *std::min_element(data.begin(), data.end());
            float max_ = *std::max_element(data.begin(), data.end());
            mSeriesMax[sName] = max_;
            mSeriesMin[sName] = min_;


        }
        mChart->removeAllSeries();
        mLinesMenu->clear();
        mShownSeries.clear();
        for (int i=0; i<dataSeries.size(); i++)
        {
            QString seriesName = dataSeries[i];
            QList<float> data = dataValues[seriesName];

            QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
            series->setName(seriesName);
            for (int j=0; j<timeStamps.size(); j++)
            {
                series->append(timeStamps[j].toMSecsSinceEpoch(), data[j]);
            }
            mChart->addSeries(series);
            series->attachAxis(mXaxis);
            series->attachAxis(mYaxis);
            QAction *showSeries = new QAction(seriesName);
            showSeries->setCheckable(true);
            showSeries->setChecked(true);
            mShownSeries.insert(seriesName);
            connect(showSeries, &QAction::triggered, [this, series, dataValues](bool checked){
                QString sName = series->name();
                if (checked)
                {
                    mChart->addSeries(series);
                    mShownSeries.insert(sName);
                }
                else
                {
                    mChart->removeSeries(series);
                    mShownSeries.remove(sName);
                }
                float maximum = 0;
                float minimum = 65000;
                foreach (QString seriesName, mShownSeries)
                {
                    if (seriesName == sName)
                        continue;
                    minimum = (mSeriesMin[seriesName] < minimum)? mSeriesMin[seriesName] : minimum;
                    maximum = (mSeriesMax[seriesName] > maximum)? mSeriesMax[seriesName] : maximum;
                }

                mYaxis->setRange(minimum, maximum);

            });

            mLinesMenu->addAction(showSeries);
        }
        ui->chartView->setChart(mChart);
        float maximum = 0;
        float minimum = 65000;
        foreach (QString seriesName, mShownSeries)
        {

            minimum = (mSeriesMin[seriesName] < minimum)? mSeriesMin[seriesName] : minimum;
            maximum = (mSeriesMax[seriesName] > maximum)? mSeriesMax[seriesName] : maximum;
        }
        QFileInfo fInfo(fileName);
        mChart->setTitleFont(QFont("Times New Roman", 14));
        mChart->setTitle("<b>" + fInfo.fileName()+ "</b>");
        mYaxis->setRange(minimum, maximum);
        mXaxis->setTickCount(10);
        mXaxis->setGridLineVisible();
        mXaxis->setFormat("yy-MM-d hh:mm");
        mXaxis->setTitleText("Date");
    }
}
