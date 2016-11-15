#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QMap>


namespace Ui {
class MainWindow;
}

class QAction;
class QMenu;

namespace QtCharts{

class QDateTimeAxis;
class QValueAxis;

}
class Chart;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static void selectDataDialog(QWidget *parent, QList<QAction *> actions);

private slots:
    void on_pushButton_clicked();

private:
    void setupActions();
    void setupMenus();
    void readCSVFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QMenu *mFileMenu;
    QMenu *mDataMenu;
    QMenu *mLinesMenu;
    QMenu *mPrintMenu;

    QAction *mOpenFile;
    QAction *mPrintAction;
    Chart *mChart;
    QtCharts::QDateTimeAxis *mXaxis;
    QtCharts::QValueAxis *mYaxis;
    QSet<QString> mShownSeries;
    QMap<QString, float> mSeriesMin;
    QMap<QString, float> mSeriesMax;


};

#endif // MAINWINDOW_H
