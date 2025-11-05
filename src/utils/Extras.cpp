#include "Extras.h"
#include "Logger.h"
#include <QProcess>
#include <QFile>
#include <QProcessEnvironment>
#include <QDebug>
#include <thread>
#include <mutex>

namespace {
    std::atomic<bool> calamaresRunning{false};
    std::mutex calamaresLock;
}

namespace Extras {

QString getDesktopEnvironment() {
    auto env = QProcessEnvironment::systemEnvironment();
    return env.value("XDG_CURRENT_DESKTOP", "").toLower();
}

void openUrl(const QString &url) {
    QProcess::startDetached("xdg-open", {url});
}

bool checkIfLiveISO() {
    return QFile::exists("/run/archiso");
}

bool isCalamaresRunning() {
    return calamaresRunning.load();
}

void runCalamaresIfLiveISO(bool isLiveISO) {
    if (!isLiveISO) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(calamaresLock);
        if (calamaresRunning) {
            Logger::info("Calamares is already running");
            return;
        }
        calamaresRunning = true;
    }
    
    // Run in separate thread to avoid blocking UI
    std::thread([]{
        QProcess process;
        process.start("bash", QStringList() << "-c" << "/etc/calamares/launch.sh");
        process.waitForFinished(-1); // Wait indefinitely
        
        if (process.exitCode() != 0) {
            Logger::error(QString("Calamares exit code: %1").arg(process.exitCode()));
            Logger::error(QString("Error: %1").arg(QString::fromUtf8(process.readAllStandardError())));
        } else {
            Logger::info(QString::fromUtf8(process.readAllStandardOutput()));
        }
        
        {
            std::lock_guard<std::mutex> lock(calamaresLock);
            calamaresRunning = false;
        }
    }).detach();
}

} // namespace Extras
