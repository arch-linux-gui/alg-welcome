#include "Updates.h"
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>

namespace Updates {

void updateSystem(const QString &desktopEnv) {
    if (desktopEnv == "xfce") {
        QProcess::startDetached("xfce4-terminal", QStringList() 
            << "-x" << "pkexec" << "pacman" << "--noconfirm" << "-Syu");
        
    } else if (desktopEnv == "gnome") {
        QProcess::startDetached("gnome-terminal", QStringList() 
            << "--" << "sudo" << "pacman" << "--noconfirm" << "-Syu");
        
    } else if (desktopEnv == "kde") {
        // Remove problematic environment variables for KDE
        auto env = QProcessEnvironment::systemEnvironment();
        env.remove("LD_LIBRARY_PATH");
        env.remove("QT_PLUGIN_PATH");
        env.remove("QT_QPA_PLATFORM_THEME");
        
        QProcess process;
        process.setProcessEnvironment(env);
        process.startDetached("konsole", QStringList() 
            << "-e" << "sudo" << "pacman" << "--noconfirm" << "-Syu");
        
    } else {
        qDebug() << "Unsupported desktop environment:" << desktopEnv;
    }
}

} // namespace Updates
