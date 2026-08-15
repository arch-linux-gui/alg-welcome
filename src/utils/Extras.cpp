#include "Extras.h"

#include <spdlog/spdlog.h>

#include <QProcess>
#include <QFile>
#include <QProcessEnvironment>
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

#ifdef ENABLE_TEST_HOOKS
void setCalamaresRunningForTest(bool running) {
    std::lock_guard<std::mutex> lock(calamaresLock);
    calamaresRunning = running;
}
#endif

void runCalamaresIfLiveISO(bool isLiveISO) {

    if (!isLiveISO) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(calamaresLock);
        if (calamaresRunning) {
            spdlog::debug("Calamares is already running");
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
            spdlog::error("Calamares exit code: {}", process.exitCode());
            spdlog::error("Calamares stderr: {}", QString::fromUtf8(process.readAllStandardError()).toStdString());
        } else {
            spdlog::debug("Calamares stdout: {}", QString::fromUtf8(process.readAllStandardOutput()).toStdString());
        }
        
        {
            std::lock_guard<std::mutex> lock(calamaresLock);
            calamaresRunning = false;
        }
    }).detach();
}

} // namespace Extras
