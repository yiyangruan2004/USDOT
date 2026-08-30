#pragma once
#include <QObject>
#include <QDebug>
#include <QThread>
#include <atomic>

#ifndef Q_OS_WASM
#include <QUdpSocket>
#endif

inline constexpr int CH_CNT = 15;

QByteArray generateDemoDatagram(int channel, quint64 frame);

class MONITOR : public QObject
{
    Q_OBJECT

public:
    explicit MONITOR(QObject *parent = nullptr);
    ~MONITOR();
    std::atomic_bool chON[CH_CNT + 1] = {};
    std::atomic_bool stop = true;
signals:
    void data(QByteArray datagram, int idx);
    void finished();
public slots:
    void start();
private:
#ifdef Q_OS_WASM
    quint64 frameCounter = 0;
#else
    QUdpSocket socket;
#endif
    QByteArray tx(int idx);
};
