#ifndef MIRRORLISTPARSING_H
#define MIRRORLISTPARSING_H

#include <QString>

namespace MirrorListParsing {

struct ParsedLine {
    // "Server" UI column: a mirror URL, an INFO message, or the raw line if nothing matched.
    QString server;
    // "Rate" UI column: an actual rate (e.g. "5.2 MiB/s") for a matched server line, or the
    // severity label ("WARNING"/"ERROR") otherwise, matching the existing UI's column reuse.
    QString rate;
    // "Time" UI column.
    QString time;
    // "", "WARNING", or "ERROR" — lets the caller pick an appropriate log level.
    QString logType;
};

// Parses one line of reflector's --verbose output, e.g.:
//   [2024-01-15 10:30:45] INFO: https://mirror.example.com 5.2 MiB/s 0.5 s
//   [2024-01-15 10:30:45] WARNING: some message
// Falls back to logType="" with the raw line in `server` if nothing matches.
ParsedLine parseLine(const QString &logLine);

} // namespace MirrorListParsing

#endif // MIRRORLISTPARSING_H
