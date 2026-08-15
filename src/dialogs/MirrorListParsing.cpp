#include "MirrorListParsing.h"

#include <QRegularExpression>

namespace MirrorListParsing
{

ParsedLine
parseLine( const QString& logLine )
{
    static const QRegularExpression logPattern( R"(^\[.*?\]\s+(INFO|WARNING|ERROR):\s+(.+)$)" );
    const auto matches = logPattern.match( logLine );

    if ( !matches.hasMatch() )
    {
        return ParsedLine { logLine, "", "", "" };
    }

    const QString logType = matches.captured( 1 );
    const QString content = matches.captured( 2 );

    if ( logType != "INFO" )
    {
        return ParsedLine { content, logType, "N/A", logType };
    }

    static const QRegularExpression serverPattern( R"(^(https?://\S+)\s+(\S+\s+\S+/s)\s+(\S+\s+s)$)" );
    const auto serverMatches = serverPattern.match( content );
    if ( serverMatches.hasMatch() )
    {
        return ParsedLine { serverMatches.captured( 1 ), serverMatches.captured( 2 ), serverMatches.captured( 3 ), "" };
    }

    return ParsedLine { content, "", "", "" };
}

}  // namespace MirrorListParsing
