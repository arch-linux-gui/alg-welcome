#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDebug>

namespace Logger {
    inline void info(const QString &message) {
        qDebug() << "[INFO]" << message;
    }
    
    inline void warning(const QString &message) {
        qWarning() << "[WARNING]" << message;
    }
    
    inline void error(const QString &message) {
        qCritical() << "[ERROR]" << message;
    }
    
    inline void debug(const QString &message) {
        qDebug() << "[DEBUG]" << message;
    }
}

#endif // LOGGER_H
