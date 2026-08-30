#pragma once
#include <QObject>
#include <QDateTime>
#include <QMutex>
#include <QMessageBox>

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();
    static void installLogger();

private:
    static QString levelToString(QtMsgType type);
    static void logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    static QMutex mutex;
    static QtMessageHandler originalHandler;
};