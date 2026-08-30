#include "mainwindow.h"

#ifdef Q_OS_WASM
#include <QDateTime>
#endif


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    chBox[0] = nullptr;
    chBox[1] = ui->CH1;
    chBox[2] = ui->CH2;
    chBox[3] = ui->CH3;
    chBox[4] = ui->CH4;
    chBox[5] = ui->CH5;
    chBox[6] = ui->CH6;
    chBox[7] = ui->CH7;
    chBox[8] = ui->CH8;
    chBox[9] = ui->CH9;
    chBox[10] = ui->CH10;
    chBox[11] = ui->CH11;
    chBox[12] = ui->CH12;
    chBox[13] = ui->CH13;
    chBox[14] = ui->CH14;
    chBox[15] = ui->CH15;



    QTimer *wifitimer = new QTimer(this);
    wifitimer->setSingleShot(true);
    QTimer::singleShot(0, ui->wifiConnection, &QPushButton::click);
    connect(ui->wifiConnection, &QPushButton::clicked, this, [this, wifitimer]() {
        ui->wifiState->setText("Checking");
        ui->wifiState->setCheckState(Qt::PartiallyChecked);
        ctrl.tx("R");
        wifitimer->start(2000);
    });
    connect(wifitimer, &QTimer::timeout, this, [this]() {
        ui->wifiConnection->setText("Check Connection");
        ui->wifiState->setCheckState(Qt::Unchecked);
        ui->wifiState->setText("Offline");
        qWarning() << "wifi not found";
    });
    connect(&ctrl, &CTRL::online, this, [this, wifitimer](){
        wifitimer->stop();
        ui->wifiConnection->setText("Check Connection");
        ui->wifiState->setCheckState(Qt::Checked);
        ui->wifiState->setText("Online");
        qDebug() << "ESP online";
    });

#ifdef Q_OS_WASM
    connect(ui->pushButton_2, &QPushButton::clicked, this,
            [this, captureCount = 0]() mutable {
        ++captureCount;
        ui->textBrowser->setText(
            QStringLiteral("Simulated capture %1 - %2")
                .arg(captureCount)
                .arg(QDateTime::currentDateTime().toString(
                    QStringLiteral("yyyy-MM-dd HH:mm:ss"))));
    });
#endif


    monitor.moveToThread(&monitorThread);
    connect(&monitorThread, &QThread::started, &monitor, &MONITOR::start);
    connect(&monitor, &MONITOR::finished, &monitorThread, &QThread::quit);
    connect(ui->monitor, &QCheckBox::clicked, this,[this](bool checked) {
        if(!ctrl.ready){
            qWarning() << "FPGA not ready";
            ui->monitor->setCheckState(Qt::Unchecked);
            return;
        }
        if(checked){
            monitor.stop.store(false);
            if (!monitorThread.isRunning()) {
                monitorThread.start();
            }
        }else{
            monitor.stop.store(true);
        }
    });

    connect(ui->CHALL, &QCheckBox::clicked, this, [this](bool checked) {
        for (int channel = 1; channel <= CH_CNT; ++channel) {
            chBox[channel]->setChecked(checked);
        }
    });
    for (int i = 1; i <= CH_CNT; ++i) {
        connect(chBox[i], &QCheckBox::toggled, this, [this, i](bool checked) {
            monitor.chON[i].store(checked);
        });
    }



    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    axisX->setTitleText("Sample index");
    axisY->setTitleText("Amplitude (V)");
    QChartView *chartView = new QChartView(chart);
    chartView->setObjectName(QStringLiteral("signalChart"));
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->display->setLayout(new QVBoxLayout());
    ui->display->layout()->addWidget(chartView);
    axisX->setRange(0, 512);
    axisY->setRange(-10, 10);
    applyDashboardTheme();
    connect(ui->range, &QSpinBox::valueChanged, this, [this](int value) {
        axisY->setRange(-value, value);
    });


    connect(&monitor, &MONITOR::data, this, [this](const QByteArray &datagram, int idx) {
        QLineSeries *series = hash.value(idx, nullptr);
        const bool selected = chBox[idx]->isChecked();

        if (datagram.isEmpty() || !selected) {
            if (series) {
                series->clear();
                series->setVisible(false);

                for (QLegendMarker *marker :
                     chart->legend()->markers(series)) {
                    marker->setVisible(false);
                }
            }
            return;
        }

        if (!series) {
            series = new QLineSeries(this);
            series->setName(QString("CH%1").arg(idx));

            static const QColor channelColors[] = {
                QColor("#2563eb"), QColor("#0f9f8f"),
                QColor("#7c3aed"), QColor("#e07a22"),
                QColor("#db2777"), QColor("#0891b2"),
                QColor("#65a30d"), QColor("#4f46e5"),
                QColor("#c2410c"), QColor("#0d9488"),
                QColor("#9333ea"), QColor("#ca8a04"),
                QColor("#e11d48"), QColor("#0284c7"),
                QColor("#16a34a")
            };
            series->setColor(channelColors[(idx - 1) % CH_CNT]);

            chart->addSeries(series);
            series->attachAxis(axisX);
            series->attachAxis(axisY);

            hash.insert(idx, series);
        }

        series->setVisible(true);
        for (QLegendMarker *marker: chart->legend()->markers(series)) {
            marker->setVisible(true);
        }

        QVector<QPointF> points;
        points.reserve(datagram.size() / 2);

        for (int i = 10; i + 1 < datagram.size(); i += 2) {
            const quint16 rawUnsigned =
                (static_cast<quint16>(
                     static_cast<quint8>(datagram[i])) << 8) |
                static_cast<quint8>(datagram[i + 1]);

            const qint16 rawSigned = static_cast<qint16>(rawUnsigned);
            const double value =
                static_cast<double>(rawSigned) * 10.24 / 32768.0;

            points.append(QPointF((i - 10) / 2, value));
        }
        series->replace(points);
    });




                
        // start a worker thread that sends pacakge /0x01 or 0x02 and wait for the received package of 1024 byte
        // 2 byte will be interpreted as one data point, the first data point will be thrown away, these data will then refreash on a qwidget


    // connect(ui->ldTrig, &QCheckBox::toggled, this, [this](bool checked) {
    //     ui->ldTrig->setChecked(ctrl.tx(QByteArray("\xF0\x00", 2)));
    //     if(checked){
    //         ui->ldTrig->setChecked(ctrl.tx(QByteArray("\xF0\x00", 2)));

    //         QDir folderPath = QDir::current().filePath("../Data");
    //         if (!folderPath.exists())
    //             folderPath.mkpath(".");
    //         QString baseName = ui->ldTarget->currentText();
    //         QString filePath = folderPath.filePath(baseName + ".csv");

    //         int counter = 1;
    //         while (QFile::exists(filePath)) {
    //             filePath = folderPath.filePath(baseName + "_" + QString::number(counter) + ".csv");
    //             counter++;
    //         }
    //         ctrl.file.setFileName(filePath);
    //         if (!ctrl.file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //             qWarning() << "Failed to open file";
    //         }
    //     }else{
    //         ui->wifiState->setCheckState(Qt::Checked);
    //     }
    // });

    // connect(&ctrl, &CTRL::ldDone, this, [this](){
    //     ui->ldTrig->setChecked(0);
    //     ctrl.file.close();
    // });


}

void MainWindow::applyDashboardTheme()
{
    ui->wifiConnection->setProperty("buttonRole", "primary");
    ui->pushButton_2->setProperty("buttonRole", "primary");
    ui->monitor->setProperty("controlRole", "monitor");
    ui->wifiState->setProperty("controlRole", "status");
    ui->CHALL->setProperty("channelRole", "all");

    for (int channel = 1; channel <= CH_CNT; ++channel) {
        chBox[channel]->setProperty("channelRole", "selector");
    }

    ui->groupBox_2->setProperty("panelState", "inactive");
    ui->groupBox_4->setProperty("panelState", "inactive");
    ui->groupBox_7->setProperty("panelState", "inactive");
    ui->groupBox_3->setProperty("panelState", "inactive");

    setStyleSheet(QStringLiteral(R"qss(
        QMainWindow {
            background-color: #edf2f7;
        }

        QWidget#centralwidget {
            background-color: #edf2f7;
            color: #1e293b;
            font-family: "Segoe UI";
            font-size: 9pt;
        }

        QTabWidget#tabWidget::pane {
            background-color: #f8fafc;
            border: 1px solid #cbd5e1;
            border-radius: 10px;
            top: -1px;
        }

        QTabWidget#tabWidget > QWidget > QWidget {
            background-color: #f8fafc;
        }

        QTabBar::tab {
            background-color: #dfe7f1;
            border: 1px solid #c5d0de;
            border-bottom: none;
            border-top-left-radius: 7px;
            border-top-right-radius: 7px;
            color: #475569;
            font-weight: 600;
            min-width: 104px;
            padding: 9px 18px;
        }

        QTabBar::tab:selected {
            background-color: #ffffff;
            color: #1d4ed8;
        }

        QTabBar::tab:hover:!selected {
            background-color: #eaf1fb;
            color: #1e3a5f;
        }

        QGroupBox {
            background-color: #ffffff;
            border: 1px solid #d2dce8;
            border-radius: 9px;
            color: #1e293b;
            font-weight: 600;
            margin-top: 15px;
            padding: 13px 10px 10px 10px;
        }

        QGroupBox::title {
            background-color: #ffffff;
            color: #1d4ed8;
            left: 11px;
            padding: 0 5px;
            subcontrol-origin: margin;
        }

        QGroupBox[panelState="inactive"] {
            background-color: #f8fafc;
            border-color: #dbe3ec;
        }

        QGroupBox[panelState="inactive"]::title {
            background-color: #f8fafc;
            color: #7c8999;
        }

        QLabel {
            color: #475569;
            font-weight: 500;
        }

        QLineEdit,
        QComboBox,
        QSpinBox,
        QDoubleSpinBox {
            background-color: #ffffff;
            border: 1px solid #c7d2df;
            border-radius: 6px;
            color: #1e293b;
            min-height: 24px;
            padding: 4px 8px;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
        }

        QSpinBox,
        QDoubleSpinBox {
            padding-right: 22px;
        }

        QLineEdit:focus,
        QComboBox:focus,
        QSpinBox:focus,
        QDoubleSpinBox:focus {
            border: 1px solid #2563eb;
        }

        QComboBox::drop-down {
            border: none;
            width: 24px;
        }

        QComboBox QAbstractItemView {
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            color: #1e293b;
            outline: 0;
            selection-background-color: #2563eb;
            selection-color: #ffffff;
        }

        QPushButton {
            background-color: #eef4fb;
            border: 1px solid #b8c6d8;
            border-radius: 7px;
            color: #27364a;
            font-weight: 600;
            min-height: 26px;
            padding: 6px 14px;
        }

        QPushButton:hover {
            background-color: #e1ecfa;
            border-color: #3b82f6;
            color: #1d4ed8;
        }

        QPushButton:pressed {
            background-color: #cfdef2;
        }

        QPushButton[buttonRole="primary"] {
            background-color: #15803d;
            border-color: #166534;
            color: #ffffff;
        }

        QPushButton[buttonRole="primary"]:hover {
            background-color: #166534;
            border-color: #14532d;
            color: #ffffff;
        }

        QPushButton[buttonRole="primary"]:pressed {
            background-color: #14532d;
        }

        QPushButton:disabled {
            background-color: #e5eaf0;
            border-color: #d6dde6;
            color: #98a4b3;
        }

        QCheckBox {
            color: #475569;
            spacing: 7px;
        }

        QCheckBox::indicator {
            background-color: #ffffff;
            border: 1px solid #94a3b8;
            border-radius: 4px;
            height: 15px;
            width: 15px;
        }

        QCheckBox::indicator:hover {
            border-color: #2563eb;
        }

        QCheckBox::indicator:checked {
            background-color: #15803d;
            border-color: #15803d;
        }

        QCheckBox::indicator:indeterminate {
            background-color: #f59e0b;
            border-color: #d97706;
        }

        QCheckBox[channelRole="selector"],
        QCheckBox[channelRole="all"] {
            background-color: #f8fafc;
            border: 1px solid #d7e0ea;
            border-radius: 6px;
            min-width: 38px;
            padding: 5px 7px;
        }

        QCheckBox[channelRole="selector"]:hover,
        QCheckBox[channelRole="all"]:hover {
            background-color: #edf4ff;
            border-color: #93b4df;
        }

        QCheckBox[channelRole="selector"]:checked,
        QCheckBox[channelRole="all"]:checked {
            background-color: #e7f0ff;
            border-color: #7ca5db;
            color: #1d4ed8;
            font-weight: 600;
        }

        QCheckBox[channelRole="all"] {
            color: #1d4ed8;
            font-weight: 700;
        }

        QCheckBox[controlRole="monitor"] {
            background-color: #e8f1ff;
            border: 1px solid #a9c3e8;
            border-radius: 7px;
            color: #1e4f8a;
            font-weight: 700;
            padding: 6px 11px;
        }

        QCheckBox[controlRole="monitor"]:checked {
            background-color: #dcfce7;
            border-color: #86c89b;
            color: #166534;
        }

        QCheckBox[controlRole="status"]:disabled {
            color: #64748b;
            font-weight: 600;
        }

        QCheckBox[controlRole="status"]::indicator:checked:disabled {
            background-color: #16a34a;
            border-color: #15803d;
        }

        QCheckBox[controlRole="status"]::indicator:indeterminate:disabled {
            background-color: #f59e0b;
            border-color: #d97706;
        }

        QTextBrowser {
            background-color: #ffffff;
            border: 1px solid #d2dce8;
            border-radius: 8px;
            color: #475569;
            padding: 8px;
        }

        QChartView#signalChart {
            background-color: #ffffff;
            border: 1px solid #d2dce8;
            border-radius: 9px;
        }

        QToolTip {
            background-color: #172033;
            border: 1px solid #334155;
            color: #ffffff;
            padding: 5px;
        }
    )qss"));

    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(QColor("#ffffff")));
    chart->setPlotAreaBackgroundPen(QPen(QColor("#dbe3ec")));
    chart->setMargins(QMargins(10, 8, 10, 8));
    chart->legend()->setAlignment(Qt::AlignTop);
    chart->legend()->setLabelColor(QColor("#475569"));
    chart->legend()->setBackgroundVisible(false);

    const QColor axisColor("#64748b");
    const QColor gridColor("#e5eaf0");
    QFont axisFont(QStringLiteral("Segoe UI"));
    axisFont.setPointSize(8);

    for (QValueAxis *axis : {axisX, axisY}) {
        axis->setLinePenColor(axisColor);
        axis->setGridLineColor(gridColor);
        axis->setLabelsColor(axisColor);
        axis->setLabelsFont(axisFont);
        axis->setTitleBrush(QBrush(QColor("#334155")));
        axis->setTitleFont(axisFont);
    }
}

MainWindow::~MainWindow(){
    monitor.stop.store(true);
    monitorThread.quit();
    monitorThread.wait();
    delete ui;
}
