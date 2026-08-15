#include "WelcomeWindow.h"
#include "utils/Autostart.h"
#include "utils/Logging.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QCommandLineParser>

#include <cstdio>

int
main( int argc, char* argv[] )
{
    // Resolved from raw argv, before QApplication exists, so logging is live from the very
    // first line and captures anything Qt itself logs during app construction.
    Logging::init( Logging::resolveLevelFromArgv( argc, argv ) );
    spdlog::debug( "Archer version: {}", ARCHER_VERSION );

    QApplication app( argc, argv );
    app.setApplicationVersion( ARCHER_VERSION );
    app.setDesktopFileName( "archer" );
    app.setOrganizationName( "Arka Linux GUI" );
    app.setOrganizationDomain( "arkalinuxgui.org" );

    // Setup command line parser. -v is reclaimed for verbosity (see below), so --version is
    // registered manually instead of via addVersionOption().
    QCommandLineParser parser;
    parser.setApplicationDescription( "Archer - A welcome screen for Arka Linux GUI" );
    parser.addHelpOption();

    QCommandLineOption versionOption( QStringList() << "version", "Display version information" );
    parser.addOption( versionOption );

    QCommandLineOption noAutostartOption( QStringList() << "n" << "no-autostart",
                                          "Don't check autostart status on launch" );
    parser.addOption( noAutostartOption );

    QCommandLineOption debugOption( QStringList() << "d" << "debug", "Enable debug-level logging (equivalent to -v)" );
    parser.addOption( debugOption );

    QCommandLineOption verboseOption( QStringList() << "v" << "verbose",
                                      "Increase log verbosity (-v for debug, -vv for trace)" );
    parser.addOption( verboseOption );

    QCommandLineOption logLevelOption( QStringList() << "log-level",
                                       "Set the log level explicitly (trace, debug, info, warn, error, critical, off)",
                                       "level" );
    parser.addOption( logLevelOption );

    // Process command line (also handles -h/--help and reports unknown options).
    parser.process( app );

    if ( parser.isSet( versionOption ) )
    {
        printf( "%s %s\n", qPrintable( app.applicationName() ), qPrintable( app.applicationVersion() ) );
        return 0;
    }

    // Always ensure autostart is enabled so the welcome app displays on boot (important for the
    // live ISO), unless the caller explicitly opted out.
    if ( !parser.isSet( noAutostartOption ) )
    {
        Autostart::toggleAutostart( true );
    }

    WelcomeWindow window;
    window.show();

    return app.exec();
}
