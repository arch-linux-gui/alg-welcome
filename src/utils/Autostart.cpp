#include "Autostart.h"

#include <spdlog/spdlog.h>

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>

namespace Autostart
{

void
toggleAutostart( bool enable )
{
    const auto homeDir = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
    const QString autostartDir = homeDir + "/.config/autostart";
    const QString autostartFile = autostartDir + "/archer.desktop";
    const QString sourceFile = "/usr/share/applications/archer.desktop";

    // Create autostart directory if it doesn't exist
    QDir dir( autostartDir );
    if ( !dir.exists() )
    {
        if ( !dir.mkpath( autostartDir ) )
        {
            spdlog::error( "Error creating directory: {}", autostartDir.toStdString() );
            return;
        }
    }

    if ( enable )
    {
        // Enable autostart
        if ( QFile::exists( autostartFile ) )
        {
            spdlog::debug( "Autostart is already enabled" );
            return;
        }

        spdlog::debug( "Enabling autostart..." );
        if ( QFile::exists( sourceFile ) )
        {
            if ( QFile::copy( sourceFile, autostartFile ) )
            {
                spdlog::info( "Autostart enabled" );
            }
            else
            {
                spdlog::error( "Error copying {} to {}", sourceFile.toStdString(), autostartFile.toStdString() );
            }
        }
        else
        {
            spdlog::error( "Source file {} not found", sourceFile.toStdString() );
        }
    }
    else
    {
        // Disable autostart
        if ( !QFile::exists( autostartFile ) )
        {
            spdlog::debug( "Autostart is already disabled" );
            return;
        }

        spdlog::debug( "Disabling autostart..." );
        // Use pkexec to remove file (in case permissions needed)
        QProcess::execute( "pkexec", QStringList() << "rm" << autostartFile );
        spdlog::info( "Autostart disabled" );
    }
}

bool
checkFileExists()
{
    const auto homeDir = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
    const QString autostartFile = homeDir + "/.config/autostart/archer.desktop";
    return QFile::exists( autostartFile );
}

}  // namespace Autostart
