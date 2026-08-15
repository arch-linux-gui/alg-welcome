#ifndef UPDATES_H
#define UPDATES_H

#include <QString>
#include <QStringList>

namespace Updates
{

struct Command
{
    QString program;
    QStringList arguments;
};

// Pure: selects the system-update command for a desktop environment, without running it.
// Returns a Command with an empty `program` for an unsupported desktopEnv.
Command commandFor( const QString& desktopEnv );

void updateSystem( const QString& desktopEnv );

}  // namespace Updates

#endif  // UPDATES_H
