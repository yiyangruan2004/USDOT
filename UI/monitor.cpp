#include "monitor.h"
#include <cmath>

#ifndef Q_OS_WASM
constexpr const char* ESP_ADDRESS   = "192.168.4.1";
constexpr int ESP_PORT              = 12345;
constexpr int MONITOR_PORT          = 67890;
#endif

QByteArray generateDemoDatagram(int channel, quint64 frame)
{
    QByteArray datagram(1024, '\0');
    datagram[0] = static_cast<char>(0x10 | (channel & 0x0f));

    constexpr int headerSize = 10;
    constexpr double pi = 3.14159265358979323846;
    const double frequency = 1.0 + static_cast<double>(channel) * 0.08;
    const double amplitude = 0.45 + static_cast<double>(channel) * 0.025;
    const double framePhase = static_cast<double>(frame) * 0.12;

    for (int offset = headerSize, sample = 0;
         offset + 1 < datagram.size();
         offset += 2, ++sample) {
        const double position = static_cast<double>(sample) / 506.0;
        const double primary = std::sin(
            2.0 * pi * frequency * position
            + static_cast<double>(channel) * 0.31
            + framePhase);
        const double harmonic = std::sin(
            2.0 * pi * (frequency * 0.35) * position
            + framePhase * 0.5);
        const double normalized = amplitude * (0.82 * primary + 0.18 * harmonic);
        const qint16 value = static_cast<qint16>(normalized * 30000.0);
        const quint16 encoded = static_cast<quint16>(value);

        datagram[offset] = static_cast<char>((encoded >> 8) & 0xff);
        datagram[offset + 1] = static_cast<char>(encoded & 0xff);
    }

    return datagram;
}

MONITOR::MONITOR(QObject *parent)
    :QObject(parent)
{
#ifndef Q_OS_WASM
    socket.setParent(this);
#endif
}



void MONITOR::start(){
    while(!stop){
        for (int i = 1; i <= CH_CNT; ++i){
            QThread::msleep(10);
            if(chON[i]){
                emit data(tx(i), i);
            }else{
                emit data({},i);
            }

        }
    }
    emit finished();
}

QByteArray MONITOR::tx(int idx){
#ifdef Q_OS_WASM
    return generateDemoDatagram(idx, frameCounter++);
#else
    // qDebug() << "monitor.fake.tx:" << idx;
    // QByteArray datagram(1024, '\0');
    // for (int i = 0; i + 1 < datagram.size(); i += 2) {
    //     qint16 sample = static_cast<qint16>((idx * 1000) + (i / 2));
    //     datagram[i] = static_cast<char>(sample & 0xFF);
    //     datagram[i + 1] = static_cast<char>((sample >> 8) & 0xFF);
    // }
    // return datagram;

    qDebug() << "monitor.tx:" << idx;
    QByteArray packet(1024, static_cast<char>(idx));
    packet[0] = static_cast<char>(0x10 | idx);
    packet[1] = '\0';
    packet[2] = '\0';
    packet[3] = '\0';
    packet[4] = '\0';
    packet[packet.size() - 1] = '\0';

    socket.writeDatagram(packet, QHostAddress(ESP_ADDRESS), ESP_PORT);
    if (!socket.waitForReadyRead(2000)) {
        qWarning() << "Monitor timeout";
        return {};
    }
    QByteArray datagram;
    while (socket.hasPendingDatagrams()) {
        datagram.resize(socket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket.readDatagram(
            datagram.data(),
            datagram.size(),
            &sender,
            &senderPort
            );
        qDebug() << "monitor.rx:";
    }
    if (datagram.isEmpty()) {
        qWarning() << "Empty monitor packet";
        return {};
    }
    if (datagram.size() == 1024) {
        return datagram;
    }else{
        qWarning() << "Unknown monitor package size" << datagram.size();
        return {};
    }
#endif
}

MONITOR::~MONITOR(){
#ifndef Q_OS_WASM
    socket.close();
#endif
}
