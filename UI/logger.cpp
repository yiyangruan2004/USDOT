#include "logger.h"


QMutex Logger::mutex;
QtMessageHandler Logger::originalHandler = nullptr;

Logger::Logger(QObject *parent)
    : QObject(parent)
{}

QString Logger::levelToString(QtMsgType type){
    switch (type){
    case QtDebugMsg:    return "DEBUG";
    case QtInfoMsg:     return "INFO";
    case QtWarningMsg:  return "WARNING";
    case QtCriticalMsg: return "CRITICAL";
    case QtFatalMsg:    return "FATAL";
    }
    return "UNKNOWN";
}

void Logger::logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    QMutexLocker locker(&Logger::mutex);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString level = levelToString(type);

    QString formatted = QString("[%1] [%2] %3")
                            .arg(timestamp)
                            .arg(level)
                            .arg(msg);

    fprintf(stderr, "%s\n", formatted.toLocal8Bit().constData());

    QMessageBox *box = nullptr;
    switch (type){
    case QtInfoMsg:
        box = new QMessageBox(
            QMessageBox::Information,
            "Info",
            msg);
        break;
    case QtWarningMsg:
        box = new QMessageBox(
            QMessageBox::Warning,
            "Warning",
            msg);
        break;
    case QtDebugMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
        break;
    }
    if (box){
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->show();
    }



    // write to file
#ifndef Q_OS_WASM
    static FILE *f = fopen("log.txt", "a");
    if (f){
        fprintf(f, "%s\n", formatted.toUtf8().constData());
        fflush(f);
    }
#endif


}

void Logger::installLogger(){
    originalHandler = qInstallMessageHandler(Logger::logToFile);
    qDebug() << "Logger installed";
}

Logger::~Logger(){
}
