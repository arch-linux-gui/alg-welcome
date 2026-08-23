#include "Themes.h"
#include "themes/gnome.h"
#include "themes/kde.h"
#include "themes/xfce.h"

#include <spdlog/spdlog.h>

namespace Themes
{

bool
isDarkTheme( const QString& theme )
{
    const auto themeLower = theme.toLower();
    const QStringList darkKeywords = { "dark", "breezedark", "qogirdark", "prefer-dark", "orchis-dark" };

    for ( const auto& keyword : darkKeywords )
    {
        if ( themeLower.contains( keyword ) )
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr< ThemeManager >
getThemeManager( const QString& desktopEnv )
{
    if ( desktopEnv == "kde" )
    {
        return std::make_unique< KDETheme >();
    }
    else if ( desktopEnv == "gnome" )
    {
        return std::make_unique< GNOMETheme >();
    }
    else if ( desktopEnv == "xfce" )
    {
        return std::make_unique< XFCETheme >();
    }
    else
    {
        spdlog::warn( "Unsupported desktop environment: {}", desktopEnv.toStdString() );
        return nullptr;
    }
}

bool
isSystemDark( const QString& desktopEnv )
{
    auto manager = getThemeManager( desktopEnv );
    if ( !manager )
    {
        return true;
    }
    return manager->currentPresetId().endsWith( "-dark" );
}

}  // namespace Themes
