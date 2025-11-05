#ifndef EXTRAS_H
#define EXTRAS_H

#include <QString>
#include <atomic>

namespace Extras {
    QString getDesktopEnvironment();
    void openUrl(const QString &url);
    bool checkIfLiveISO();
    bool isCalamaresRunning();
    void runCalamaresIfLiveISO(bool isLiveISO);
}

#endif // EXTRAS_H
