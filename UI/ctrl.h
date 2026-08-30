#pragma once
#include <QObject>
#include <QDebug>
#include <QDir>

#ifndef Q_OS_WASM
#include <QUdpSocket>
#endif

class CTRL : public QObject
{
    Q_OBJECT

public:
    explicit CTRL(QObject *parent = nullptr);
    ~CTRL();
    bool tx(const QByteArray &cmd);
    QFile file;
    bool ready;

signals:
    void online();
    // void ldDone();
    // void data(QByteArray *datagram);
private:
#ifndef Q_OS_WASM
    QUdpSocket socket;
#endif
    
};



