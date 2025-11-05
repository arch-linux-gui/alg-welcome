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
#ifdef ENABLE_TEST_HOOKS
    // Test-only helper to simulate Calamares state (available only when ENABLE_TEST_HOOKS is defined)
    void setCalamaresRunningForTest(bool running);
#endif
}

#endif // EXTRAS_H
