#include "Resolution.h"

#include <spdlog/spdlog.h>

#include <QProcess>
#include <QProcessEnvironment>

namespace Resolution
{

Command
commandFor( const QString& desktopEnv )
{
    if ( desktopEnv == "xfce" )
    {
        return { "bash", { "-c", "xfce4-display-settings" } };
    }
    if ( desktopEnv == "gnome" )
    {
        return { "gnome-control-center", { "display" } };
    }
    if ( desktopEnv == "kde" )
    {
        return { "kcmshell6", { "kcm_kscreen" } };
    }
    return { };
}

void
screenResolution( const QString& desktopEnv )
{
    const auto command = commandFor( desktopEnv );
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

        QProcess process;
        process.setProcessEnvironment( env );
        process.startDetached( command.program, command.arguments );
    }
    else
    {
        QProcess::startDetached( command.program, command.arguments );
    }
}

}  // namespace Resolution
