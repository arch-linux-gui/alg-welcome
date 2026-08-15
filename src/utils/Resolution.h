#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <QString>
#include <QStringList>

namespace Resolution {

struct Command {
    QString program;
    QStringList arguments;
};

// Pure: selects the display-settings command for a desktop environment, without running it.
// Returns a Command with an empty `program` for an unsupported desktopEnv.
Command commandFor(const QString &desktopEnv);

void screenResolution(const QString &desktopEnv);

} // namespace Resolution

#endif // RESOLUTION_H
