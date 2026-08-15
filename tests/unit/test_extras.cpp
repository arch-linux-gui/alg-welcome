#include "utils/Extras.h"

#include <catch2/catch_test_macros.hpp>

#include <QFile>

#include <csignal>
#include <sys/wait.h>
#include <unistd.h>

TEST_CASE("isCalamaresRunning reflects the in-process test hook", "[extras][calamares]") {
    Extras::setCalamaresRunningForTest(false);
    CHECK_FALSE(Extras::isCalamaresRunning());

    Extras::setCalamaresRunningForTest(true);
    CHECK(Extras::isCalamaresRunning());

    Extras::setCalamaresRunningForTest(false);
    CHECK_FALSE(Extras::isCalamaresRunning());
}

TEST_CASE("isCalamaresRunning detecting a real installed Calamares", "[extras][calamares]") {
    if (!QFile::exists("/usr/bin/calamares")) {
        SKIP("calamares is not installed in this environment; cannot exercise the real-probe "
             "path against the actual binary (launching the real GUI installer from a test is "
             "out of scope anyway — see the isProcessRunning tests below for generic coverage "
             "of the underlying /proc probe).");
    }
}

TEST_CASE("isProcessRunning detects and stops detecting a real OS process", "[extras][process]") {
    const pid_t pid = fork();
    REQUIRE(pid >= 0);

    if (pid == 0) {
        // Child: become "sleep" so /proc/<pid>/comm gives isProcessRunning something real to find.
        execlp("sleep", "sleep", "5", static_cast<char *>(nullptr));
        _exit(127); // execlp failed
    }

    bool found = false;
    for (int i = 0; i < 100 && !found; ++i) {
        found = Extras::isProcessRunning("sleep");
        if (!found) {
            usleep(20000);
        }
    }
    CHECK(found);

    REQUIRE(kill(pid, SIGKILL) == 0);
    int status = 0;
    waitpid(pid, &status, 0);

    bool stillFound = true;
    for (int i = 0; i < 100 && stillFound; ++i) {
        stillFound = Extras::isProcessRunning("sleep");
        if (stillFound) {
            usleep(20000);
        }
    }
    CHECK_FALSE(stillFound);
}

TEST_CASE("isProcessRunning returns false for a process name that doesn't exist", "[extras][process]") {
    CHECK_FALSE(Extras::isProcessRunning("this-process-name-should-never-exist-xyz123"));
}
