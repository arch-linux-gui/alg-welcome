#include "Autostart.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>

namespace Autostart {

void toggleAutostart(bool enable) {
    const auto homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    const QString autostartDir = homeDir + "/.config/autostart";
    const QString autostartFile = autostartDir + "/welcome.desktop";
    const QString sourceFile = "/usr/share/applications/welcome.desktop";
    
    // Create autostart directory if it doesn't exist
    QDir dir(autostartDir);
    if (!dir.exists()) {
        if (!dir.mkpath(autostartDir)) {
            qDebug() << "Error creating directory:" << autostartDir;
            return;
        }
    }
    
    if (enable) {
        // Enable autostart
        if (QFile::exists(autostartFile)) {
            qDebug() << "Autostart is already enabled";
            return;
        }
        
        qDebug() << "Enabling autostart...";
        if (QFile::exists(sourceFile)) {
            if (QFile::copy(sourceFile, autostartFile)) {
                qDebug() << "Autostart enabled";
            } else {
                qDebug() << "Error copying file";
            }
        } else {
            qDebug() << "Source file" << sourceFile << "not found";
        }
    } else {
        // Disable autostart
        if (!QFile::exists(autostartFile)) {
            qDebug() << "Autostart is already disabled";
            return;
        }
        
        qDebug() << "Disabling autostart...";
        // Use pkexec to remove file (in case permissions needed)
        QProcess::execute("pkexec", QStringList() << "rm" << autostartFile);
        qDebug() << "Autostart disabled";
    }
}

bool checkFileExists() {
    const auto homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    const QString autostartFile = homeDir + "/.config/autostart/welcome.desktop";
    return QFile::exists(autostartFile);
}

} // namespace Autostart
