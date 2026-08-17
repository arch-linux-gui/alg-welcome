#include "Updates.h"

#include <spdlog/spdlog.h>

#include <QProcess>
#include <QProcessEnvironment>

namespace Updates
{

namespace
{

void
runInTerminal( const QString& desktopEnv, const Command& command )
{
    if ( command.program.isEmpty() )
    {
        spdlog::warn( "Unsupported desktop environment: {}", desktopEnv.toStdString() );
        return;
    }

    if ( desktopEnv == "kde" )
    {
        // Remove problematic environment variables for KDE
        auto env = QProcessEnvironment::systemEnvironment();
        env.remove( "LD_LIBRARY_PATH" );
        env.remove( "QT_PLUGIN_PATH" );
        env.remove( "QT_QPA_PLATFORM_THEME" );

        QProcess process;
        process.setProcessEnvironment( env );
        process.startDetached( command.program, command.arguments );
    }
    else
    {
        QProcess::startDetached( command.program, command.arguments );
    }
}

}  // namespace

Command
commandFor( const QString& desktopEnv )
{
    if ( desktopEnv == "xfce" )
    {
        return { "xfce4-terminal", { "-x", "pkexec", "pacman", "--noconfirm", "-Syu" } };
    }
    if ( desktopEnv == "gnome" )
    {
        return { "kgx", { "--", "sudo", "pacman", "--noconfirm", "-Syu" } };
    }
    if ( desktopEnv == "kde" )
    {
        return { "konsole", { "-e", "sudo", "pacman", "--noconfirm", "-Syu" } };
    }
    return { };
}

Command
syncCommandFor( const QString& desktopEnv )
{
    if ( desktopEnv == "xfce" )
    {
        return { "xfce4-terminal", { "-x", "pkexec", "pacman", "--noconfirm", "-Syy" } };
    }
    if ( desktopEnv == "gnome" )
    {
        return { "kgx", { "--", "sudo", "pacman", "--noconfirm", "-Syy" } };
    }
    if ( desktopEnv == "kde" )
    {
        return { "konsole", { "-e", "sudo", "pacman", "--noconfirm", "-Syy" } };
    }
    return { };
}

void
updateSystem( const QString& desktopEnv )
{
    runInTerminal( desktopEnv, commandFor( desktopEnv ) );
}

void
syncDatabases( const QString& desktopEnv )
{
    runInTerminal( desktopEnv, syncCommandFor( desktopEnv ) );
}

}  // namespace Updates
