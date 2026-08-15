#include "Logging.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include <algorithm>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace Logging
{

namespace
{

QString
resolveLogFilePath()
{
    const QString systemDir = "/var/log/archer";
    QDir dir( systemDir );
    if ( dir.exists() || dir.mkpath( "." ) )
    {
        QFile probe( systemDir + "/.write-test" );
        if ( probe.open( QIODevice::WriteOnly ) )
        {
            probe.close();
            probe.remove();
            return systemDir + "/archer.log";
        }
    }

    const QString fallbackDir = QStandardPaths::writableLocation( QStandardPaths::TempLocation );
    return fallbackDir + "/archer.log";
}

// Recognizes -v, -vv, -vvv, ... as a single bundled token.
bool
isBundledVerboseFlag( const std::string& arg )
{
    if ( arg.size() < 2 || arg[ 0 ] != '-' )
    {
        return false;
    }
    return std::all_of( arg.begin() + 1, arg.end(), []( char c ) { return c == 'v'; } );
}

// Explicit allow-list rather than spdlog::level::from_str(), which silently maps an
// unrecognized name to "off" instead of reporting an error.
bool
parseLevelName( const std::string& name, spdlog::level::level_enum& out )
{
    static const std::vector< std::pair< std::string, spdlog::level::level_enum > > kNames = {
        { "trace", spdlog::level::trace },    { "debug", spdlog::level::debug },
        { "info", spdlog::level::info },      { "warn", spdlog::level::warn },
        { "warning", spdlog::level::warn },   { "error", spdlog::level::err },
        { "err", spdlog::level::err },        { "critical", spdlog::level::critical },
        { "fatal", spdlog::level::critical }, { "off", spdlog::level::off },
    };

    for ( const auto& [ name_, level ] : kNames )
    {
        if ( name_ == name )
        {
            out = level;
            return true;
        }
    }
    return false;
}

}  // namespace

void
init( spdlog::level::level_enum level )
{
    std::vector< spdlog::sink_ptr > sinks;
    sinks.push_back( std::make_shared< spdlog::sinks::stderr_color_sink_mt >() );

    const QString logPath = resolveLogFilePath();
    try
    {
        sinks.push_back(
            std::make_shared< spdlog::sinks::basic_file_sink_mt >( logPath.toStdString(), /*truncate=*/false ) );
    }
    catch ( const spdlog::spdlog_ex& ex )
    {
        // The file sink is a nice-to-have; fall back to stderr-only logging if it can't be
        // opened (e.g. neither /var/log/archer/ nor the temp dir are writable).
        std::fprintf( stderr, "archer: could not open log file %s: %s\n", logPath.toStdString().c_str(), ex.what() );
    }

    auto logger = std::make_shared< spdlog::logger >( "archer", sinks.begin(), sinks.end() );
    logger->set_pattern( "[%H:%M:%S.%e] [%^%l%$] %v" );
    logger->set_level( level );
    logger->flush_on( spdlog::level::warn );

    spdlog::set_default_logger( logger );
    spdlog::debug( "Logging to {}", logPath.toStdString() );
}

spdlog::level::level_enum
resolveLevelFromArgv( int argc, char* argv[] )
{
    int verboseCount = 0;
    bool debugFlag = false;
    spdlog::level::level_enum explicitLevel = kDefaultLevel;
    bool hasExplicit = false;

    for ( int i = 1; i < argc; ++i )
    {
        const std::string arg = argv[ i ];

        if ( arg == "-d" || arg == "--debug" )
        {
            debugFlag = true;
        }
        else if ( arg == "--verbose" )
        {
            ++verboseCount;
        }
        else if ( isBundledVerboseFlag( arg ) )
        {
            verboseCount += static_cast< int >( arg.size() - 1 );
        }
        else if ( arg == "--log-level" && i + 1 < argc )
        {
            const std::string value = argv[ i + 1 ];
            ++i;
            if ( parseLevelName( value, explicitLevel ) )
            {
                hasExplicit = true;
            }
            else
            {
                std::fprintf( stderr, "archer: unrecognized --log-level value '%s', ignoring\n", value.c_str() );
            }
        }
        else if ( arg.rfind( "--log-level=", 0 ) == 0 )
        {
            const std::string value = arg.substr( std::string( "--log-level=" ).size() );
            if ( parseLevelName( value, explicitLevel ) )
            {
                hasExplicit = true;
            }
            else
            {
                std::fprintf( stderr, "archer: unrecognized --log-level value '%s', ignoring\n", value.c_str() );
            }
        }
    }

    if ( hasExplicit )
    {
        return explicitLevel;
    }

    if ( verboseCount >= 2 )
    {
        return spdlog::level::trace;
    }
    if ( verboseCount == 1 || debugFlag )
    {
        return spdlog::level::debug;
    }
    return kDefaultLevel;
}

}  // namespace Logging
