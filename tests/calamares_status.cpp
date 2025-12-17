#include "utils/Extras.h"
#include <iostream>
#include <stdexcept>

int main() {
    using namespace std;
    cout << "Testing Calamares monitoring functionality..." << endl;

    // Initial state
    bool initial = Extras::isCalamaresRunning();
    cout << "Initial Calamares status: " << (initial ? "true" : "false") << endl;
    if (initial) {
        cerr << "Expected Calamares not running initially" << endl;
        return 1;
    }

    // Simulate start
    cout << "Simulating Calamares start..." << endl;
    Extras::setCalamaresRunningForTest(true);
    bool afterStart = Extras::isCalamaresRunning();
    cout << "Calamares status after start: " << (afterStart ? "true" : "false") << endl;
    if (!afterStart) {
        cerr << "Expected Calamares running after start" << endl;
        return 2;
    }

    // Simulate stop
    cout << "Simulating Calamares stop..." << endl;
    Extras::setCalamaresRunningForTest(false);
    bool afterStop = Extras::isCalamaresRunning();
    cout << "Calamares status after stop: " << (afterStop ? "true" : "false") << endl;
    if (afterStop) {
        cerr << "Expected Calamares not running after stop" << endl;
        return 3;
    }

    cout << "All tests passed! Calamares monitoring functionality works correctly." << endl;
    return 0;
}
