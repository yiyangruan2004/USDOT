#pragma once

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include "./ui_mainwindow.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QHash>
#include <QThread>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>

#include "ctrl.h"
#include "monitor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;


private:
    void applyDashboardTheme();

    Ui::MainWindow *ui;
    CTRL ctrl;
    MONITOR monitor;
    QThread monitorThread;
    QCheckBox* chBox[CH_CNT+1];

    QChart *chart = new QChart();
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    QHash<int, QLineSeries*> hash;
};

