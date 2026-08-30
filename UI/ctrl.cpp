#include "ctrl.h"

#ifdef Q_OS_WASM
#include <QTimer>
#else
constexpr const char* ESP_ADDRESS   = "192.168.4.1";
constexpr int ESP_PORT              = 12345;
constexpr const char* LOCAL_ADDRESS = "127.0.0.1";
constexpr int CTRL_PORT             = 12345;
#endif

CTRL::CTRL(QObject *parent)
    :QObject(parent),
    ready(false)
{
#ifndef Q_OS_WASM
    socket.bind(CTRL_PORT);
    QObject::connect(&socket,&QUdpSocket::readyRead,[this](){
        while(socket.hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(socket.pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            socket.readDatagram(
                datagram.data(),
                datagram.size(),
                &sender,
                &senderPort
                );
            qDebug() << "ctrl.rx:" << datagram;
            if(datagram.isEmpty()) {
                qWarning() << "Empty ctrl packet";
            }



            if(datagram.size()==4){
                if((uint8_t)datagram[1] == 'R'){ // LSB
                    emit online();
                    ready = true;
                }else if ((uint8_t)datagram[0] == 'N'){
                    emit online();
                    qWarning() << "FPGA not responding" << datagram;
                    ready = false;
                }else{
                    qWarning() << "Unknown ctrl command return" << datagram;
                }
            }else{
                qWarning() << "Unknown ctrl package size" << datagram.size();
                // QTextStream out(&file);
                // out << static_cast<quint8>(datagram[0]);
                // for (int i = 1; i + 1< datagram.size(); i += 2){
                //     quint16 raw =
                //         static_cast<quint8>(datagram[i]) |
                //         (static_cast<quint8>(datagram[i + 1]) << 8);
                //     float val = raw * STEPSIZE;
                //     out << "," << val;
                // }
                // out << "\n";
            }
        }
    });
#endif
}

bool CTRL::tx(const QByteArray &cmd){
#ifdef Q_OS_WASM
    Q_UNUSED(cmd);
    QTimer::singleShot(350, this, [this]() {
        ready = true;
        emit online();
    });
    return true;
#else
    QByteArray packet = cmd;
    while (packet.size() < 4){
        packet.append('\0');
    }
    qDebug() << "ctrl.tx:" << packet;
    socket.writeDatagram(packet, QHostAddress(ESP_ADDRESS), ESP_PORT);
    return true;
#endif
}

CTRL::~CTRL(){
#ifndef Q_OS_WASM
    socket.close();
#endif
}







