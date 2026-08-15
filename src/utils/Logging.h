#ifndef LOGGING_H
#define LOGGING_H

#include <spdlog/common.h>

namespace Logging {

// Default level when no verbosity flag is given.
constexpr spdlog::level::level_enum kDefaultLevel = spdlog::level::info;

// Sets up the process-wide default spdlog logger: a colored stderr sink plus a file sink
// (preferring /var/log/archer/, falling back to a writable temp location). Must be called
// before QApplication is constructed so even the earliest startup lines are captured at the
// right verbosity.
void init(spdlog::level::level_enum level);

// Scans raw argv for -d/--debug, repeatable -v/-vv/--verbose, and --log-level <name>, and
// returns the resulting level (--log-level wins if present). Done on raw argv rather than
// QCommandLineParser because logging needs to be live before QApplication exists;
// QCommandLineParser is still registered with the same flags afterwards, purely for --help
// discovery and argument validation.
spdlog::level::level_enum resolveLevelFromArgv(int argc, char *argv[]);

} // namespace Logging

#endif // LOGGING_H
