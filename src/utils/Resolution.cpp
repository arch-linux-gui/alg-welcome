#include "Resolution.h"
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>

namespace Resolution {

void screenResolution(const QString &desktopEnv) {
    if (desktopEnv == "xfce") {
        QProcess::startDetached("bash", QStringList() << "-c" << "xfce4-display-settings");
        
    } else if (desktopEnv == "gnome") {
        QProcess::startDetached("gnome-control-center", QStringList() << "display");
        
    } else if (desktopEnv == "kde") {
        // Remove problematic environment variables for KDE
        auto env = QProcessEnvironment::systemEnvironment();
        env.remove("LD_LIBRARY_PATH");
        
        QProcess process;
        process.setProcessEnvironment(env);
        process.startDetached("kcmshell6", QStringList() << "kcm_kscreen");
        
    } else {
        qDebug() << "Unsupported desktop environment:" << desktopEnv;
    }
}

} // namespace Resolution
