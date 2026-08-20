#include "Updates.h"

#include <spdlog/spdlog.h>

#include <QMetaObject>
#include <QProcess>
#include <QProcessEnvironment>

namespace Updates
{

QStringList
updateArgs()
{
    return { "pacman", "--noconfirm", "-Syu" };
}

QStringList
syncArgs()
{
    return { "pacman", "--noconfirm", "-Syy" };
}

Runner::Runner( QObject* parent )
    : QObject( parent )
{
}

bool
Runner::isRunning() const
{
    return running.load();
}

void
Runner::start( const QStringList& pacmanArgs )
{
    if ( running.load() )
    {
        spdlog::warn( "Updates::Runner::start() called while already running; ignoring" );
        return;
    }

    running = true;

    // jthread requests-stop-and-joins any previous one on assignment, matching MirrorlistPage's
    // update thread.
    thread = std::jthread(
        [ this, pacmanArgs ]( std::stop_token )
        {
            // Clean environment to avoid Qt library conflicts, same as MirrorlistPage's reflector
            // call and the old KDE-specific terminal launch this replaces.
            auto env = QProcessEnvironment::systemEnvironment();
            env.remove( "LD_LIBRARY_PATH" );
            env.remove( "QT_PLUGIN_PATH" );
            env.remove( "QT_QPA_PLATFORM_THEME" );

            spdlog::debug( "Executing: pkexec {}", pacmanArgs.join( " " ).toStdString() );

            QProcess process;
            process.setProcessEnvironment( env );
            process.setProcessChannelMode( QProcess::MergedChannels );
            process.start( "pkexec", pacmanArgs );
            process.waitForStarted();

            while ( process.state() != QProcess::NotRunning || process.canReadLine() )
            {
                if ( process.canReadLine() )
                {
                    const QString line = QString::fromUtf8( process.readLine() ).trimmed();
                    if ( !line.isEmpty() )
                    {
                        QMetaObject::invokeMethod( this, "lineOutput", Qt::QueuedConnection, Q_ARG( QString, line ) );
                    }
                }
                else
                {
                    process.waitForReadyRead( 100 );
                }
            }

            while ( process.canReadLine() )
            {
                const QString line = QString::fromUtf8( process.readLine() ).trimmed();
                if ( !line.isEmpty() )
                {
                    QMetaObject::invokeMethod( this, "lineOutput", Qt::QueuedConnection, Q_ARG( QString, line ) );
                }
            }

            const int exitCode = process.exitCode();
            spdlog::debug( "pacman process completed with exit code: {}", exitCode );

            running = false;
            QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection, Q_ARG( int, exitCode ) );
        } );
}

}  // namespace Updates
