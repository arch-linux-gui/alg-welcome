#include "WelcomeWindow.h"
#include "version.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Welcome to ALG");
    app.setApplicationVersion(ALG_WELCOME_VERSION);
    app.setOrganizationName("Arch Linux GUI");
    app.setOrganizationDomain("arkalinuxgui.org");
    
    // Setup command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("ALG Welcome - A welcome screen for Arch Linux GUI");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add custom options
    QCommandLineOption noAutostartOption(
        QStringList() << "n" << "no-autostart",
        "Don't check autostart status on launch"
    );
    parser.addOption(noAutostartOption);
    
    QCommandLineOption debugOption(
        QStringList() << "d" << "debug",
        "Enable debug output"
    );
    parser.addOption(debugOption);
    
    // Process command line
    parser.process(app);
    
    // Handle debug option
    if (parser.isSet(debugOption)) {
        qDebug() << "Debug mode enabled";
        qDebug() << "ALG Welcome version:" << ALG_WELCOME_VERSION;
    }
    
    WelcomeWindow window;
    window.show();
    
    return app.exec();
}
