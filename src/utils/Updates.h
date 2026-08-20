#ifndef UPDATES_H
#define UPDATES_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <atomic>
#include <thread>

namespace Updates
{

// Pure: the pacman arguments for a full system upgrade / a database-only sync. DE-agnostic -
// both run via pkexec (see Runner), which already talks to whichever polkit agent the desktop
// environment provides, so there's no per-DE dispatch needed here.
QStringList updateArgs();
QStringList syncArgs();

// Runs `pkexec pacman <args>` in a background thread, streaming merged stdout/stderr
// line-by-line via lineOutput() and emitting finished(exitCode) when the process exits. Handles
// one operation at a time - start() while already running is a no-op.
class Runner : public QObject
{
    Q_OBJECT

public:
    explicit Runner( QObject* parent = nullptr );

    bool isRunning() const;
    void start( const QStringList& pacmanArgs );

Q_SIGNALS:
    void lineOutput( const QString& line );
    void finished( int exitCode );

private:
    std::atomic< bool > running { false };
    std::jthread thread;
};

}  // namespace Updates

#endif  // UPDATES_H
