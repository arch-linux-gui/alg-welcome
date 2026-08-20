#include "Themes.h"

#include <spdlog/spdlog.h>

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

namespace Themes
{

// ============================================================================
// Helper Functions
// ============================================================================

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

// ============================================================================
// KDETheme Implementation
// ============================================================================

KDETheme::KDETheme()
{
    homeDir = QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
    configFiles
        = { homeDir + "/.config/kdeglobals", homeDir + "/.kde4/share/config/kdeglobals", "/etc/kde/kdeglobals" };
}

QString
KDETheme::getColorSchemeFromFile( const QString& configFile, const QString& sectionName, const QString& keyName )
{
    QFile file( configFile );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        return QString();
    }

    QTextStream in( &file );
    QString currentSection;

    while ( !in.atEnd() )
    {
        QString line = in.readLine().trimmed();

        // Skip empty lines and comments
        if ( line.isEmpty() || line.startsWith( '#' ) )
        {
            continue;
        }

        // Check for section headers
        if ( line.startsWith( '[' ) && line.endsWith( ']' ) )
        {
            currentSection = line;
            continue;
        }

        // Check if we're in the target section and found the key
        if ( currentSection == sectionName && line.startsWith( keyName + "=" ) )
        {
            const auto value = line.section( '=', 1 );
            file.close();
            return value;
        }
    }

    file.close();
    return QString();
}

QString
KDETheme::formatColorScheme( const QString& colorScheme )
{
    static const QMap< QString, QString > schemeMap = { { "org.kde.breeze.desktop", "org.kde.breeze.desktop" },
                                                        { "org.kde.breezedark.desktop", "org.kde.breezedark.desktop" },
                                                        { "breeze", "org.kde.breeze.desktop" },
                                                        { "breezedark", "org.kde.breezedark.desktop" } };

    if ( schemeMap.contains( colorScheme ) )
    {
        return schemeMap[ colorScheme ];
    }

    // Handle themed KDE
    if ( colorScheme.endsWith( ".colors" ) )
    {
        return QFileInfo( colorScheme ).baseName();
    }

    return colorScheme;
}

QString
KDETheme::getLookAndFeelPackage()
{
    for ( const auto& configFile : configFiles )
    {
        // Try pure KDE approach
        auto colorScheme = getColorSchemeFromFile( configFile, "[KDE]", "LookAndFeelPackage" );
        if ( !colorScheme.isEmpty() )
        {
            const auto formatted = formatColorScheme( colorScheme );
            if ( formatted != "org.kde.breeze.desktop" && formatted != "org.kde.breezedark.desktop" )
            {
                // Try themed approach
                colorScheme = getColorSchemeFromFile( configFile, "[General]", "ColorScheme" );
                if ( !colorScheme.isEmpty() )
                {
                    return formatColorScheme( colorScheme );
                }
            }
            else
            {
                return formatted;
            }
        }

        // Try themed KDE approach
        colorScheme = getColorSchemeFromFile( configFile, "[General]", "ColorScheme" );
        if ( !colorScheme.isEmpty() )
        {
            return formatColorScheme( colorScheme );
        }
    }

    return DEFAULT_COLOR_SCHEME;
}

QVector< ThemePreset >
KDETheme::availablePresets() const
{
    return {
        { "default-light", "Breeze Light", "KDE Defaults", false },
        { "default-dark", "Breeze Dark", "KDE Defaults", true },
        { "alg-light", "Qogir Light", "ALG Themes", false },
        { "alg-dark", "Qogir Dark", "ALG Themes", true },
    };
}

QString
KDETheme::currentPresetId()
{
    const auto package = getLookAndFeelPackage();
    spdlog::debug( "Current KDE Theme: {}", package.toStdString() );

    if ( package == "org.kde.breeze.desktop" )
    {
        return "default-light";
    }
    if ( package == "org.kde.breezedark.desktop" )
    {
        return "default-dark";
    }
    return isDarkTheme( package ) ? "alg-dark" : "alg-light";
}

void
KDETheme::applyPreset( const QString& id )
{
    if ( id == "default-light" || id == "default-dark" )
    {
        // The stock Breeze look-and-feel package resets icons/cursor to Breeze's own as part of
        // applying the full package, not just the color scheme.
        const QString style = ( id == "default-dark" ) ? "org.kde.breezedark.desktop" : "org.kde.breeze.desktop";
        QProcess::execute( "lookandfeeltool", QStringList() << "--apply" << style );
        spdlog::info( "KDE theme changed to {}", style.toStdString() );
        return;
    }

    if ( id != "alg-light" && id != "alg-dark" )
    {
        spdlog::warn( "Unknown KDE theme preset: {}", id.toStdString() );
        return;
    }

    const bool dark = ( id == "alg-dark" );
    const QString colorScheme = dark ? "Qogirdark" : "Qogirlight";
    const QString winDeco = dark ? "__aurorae__svg__Qogir-dark-circle" : "__aurorae__svg__Qogir-light-circle";
    const QString iconTheme = dark ? "Tela-circle-dark" : "Tela-circle";

    const QString cmd = QString( "plasma-apply-colorscheme %1 && "
                                 "kwriteconfig6 --file %2/.config/kwinrc "
                                 "--group org.kde.kdecoration2 --key theme %3 && "
                                 "kwriteconfig6 --file %2/.config/kdeglobals --group Icons --key Theme %4 && "
                                 "qdbus6 org.kde.KWin /KWin reconfigure" )
                            .arg( colorScheme, homeDir, winDeco, iconTheme );
    QProcess::execute( "sh", QStringList() << "-c" << cmd );

    // Best-effort live refresh: not every system has these on PATH, but a failure here shouldn't
    // undo the config already written above.
    QProcess::execute( "plasma-changeicons", QStringList() << iconTheme );
    QProcess::execute( "plasma-apply-cursortheme", QStringList() << "McMojave-cursors" );

    spdlog::info( "KDE theme changed to Qogir {}", dark ? "Dark" : "Light" );
}

// ============================================================================
// GNOMETheme Implementation
// ============================================================================

const GNOMETheme::ThemeConfig GNOMETheme::DEFAULT_LIGHT_THEME = { "Adwaita", "", "Adwaita", "prefer-light", "Adwaita" };

const GNOMETheme::ThemeConfig GNOMETheme::DEFAULT_DARK_THEME
    = { "Adwaita", "", "Adwaita-dark", "prefer-dark", "Adwaita" };

const GNOMETheme::ThemeConfig GNOMETheme::ALG_LIGHT_THEME
    = { "Tela-circle", "Orchis-Red-Light", "Orchis-Red-Light", "prefer-light", "McMojave-cursors" };

const GNOMETheme::ThemeConfig GNOMETheme::ALG_DARK_THEME
    = { "Tela-circle-dark", "Orchis-Red-Dark", "Orchis-Red-Dark", "prefer-dark", "McMojave-cursors" };

const GNOMETheme::ThemeConfig*
GNOMETheme::configFor( const QString& id )
{
    if ( id == "default-light" )
    {
        return &DEFAULT_LIGHT_THEME;
    }
    if ( id == "default-dark" )
    {
        return &DEFAULT_DARK_THEME;
    }
    if ( id == "alg-light" )
    {
        return &ALG_LIGHT_THEME;
    }
    if ( id == "alg-dark" )
    {
        return &ALG_DARK_THEME;
    }
    return nullptr;
}

GNOMETheme::GNOMETheme()
{
    // Constructor
}

QString
GNOMETheme::getGSetting( const QString& schema, const QString& key )
{
    QProcess process;
    process.start( "gsettings", QStringList() << "get" << schema << key );
    process.waitForFinished();

    auto output = QString::fromUtf8( process.readAllStandardOutput() ).trimmed();
    // Remove quotes
    output.remove( '\'' );
    output.remove( '\"' );
    return output;
}

void
GNOMETheme::setGSetting( const QString& schema, const QString& key, const QString& value )
{
    QProcess::execute( "gsettings", QStringList() << "set" << schema << key << value );
}

QVector< ThemePreset >
GNOMETheme::availablePresets() const
{
    return {
        { "default-light", "Adwaita Light", "GNOME Defaults", false },
        { "default-dark", "Adwaita Dark", "GNOME Defaults", true },
        { "alg-light", "Orchis Red Light", "ALG Themes", false },
        { "alg-dark", "Orchis Red Dark", "ALG Themes", true },
    };
}

QString
GNOMETheme::currentPresetId()
{
    const auto colorScheme = getGSetting( "org.gnome.desktop.interface", "color-scheme" );
    const auto gtkTheme = getGSetting( "org.gnome.desktop.interface", "gtk-theme" );
    const auto theme = !colorScheme.isEmpty() ? colorScheme : gtkTheme;
    spdlog::debug( "Current GNOME Theme: {}", theme.toStdString() );

    const bool alg = gtkTheme.contains( "orchis", Qt::CaseInsensitive );
    const bool dark = isDarkTheme( theme );

    if ( alg )
    {
        return dark ? "alg-dark" : "alg-light";
    }
    return dark ? "default-dark" : "default-light";
}

void
GNOMETheme::applyPreset( const QString& id )
{
    const auto* config = configFor( id );
    if ( !config )
    {
        spdlog::warn( "Unknown GNOME theme preset: {}", id.toStdString() );
        return;
    }

    // Set icon theme
    setGSetting( "org.gnome.desktop.interface", "icon-theme", config->icons );
    spdlog::debug( "GNOME icons set to: {}", config->icons.toStdString() );

    // Set GTK theme (Legacy Applications)
    setGSetting( "org.gnome.desktop.interface", "gtk-theme", config->gtk );
    spdlog::debug( "GNOME GTK theme set to: {}", config->gtk.toStdString() );

    // Set color scheme
    setGSetting( "org.gnome.desktop.interface", "color-scheme", config->colorScheme );
    spdlog::debug( "GNOME color scheme set to: {}", config->colorScheme.toStdString() );

    // Set cursor theme
    setGSetting( "org.gnome.desktop.interface", "cursor-theme", config->cursor );
    spdlog::debug( "GNOME cursor theme set to: {}", config->cursor.toStdString() );

    // Set shell theme (requires user-theme extension); empty resets to the built-in default.
    setGSetting( "org.gnome.shell.extensions.user-theme", "name", config->shell );

    spdlog::info( "GNOME theme changed to {}", config->gtk.toStdString() );
}

// ============================================================================
// XFCETheme Implementation
// ============================================================================

XFCETheme::XFCETheme()
{
    // Constructor
}

QString
XFCETheme::getXfconfValue( const QString& channel, const QString& propertyPath )
{
    QProcess process;
    process.start( "xfconf-query", QStringList() << "-c" << channel << "-p" << propertyPath );
    process.waitForFinished();

    return QString::fromUtf8( process.readAllStandardOutput() ).trimmed();
}

void
XFCETheme::setXfconfValue( const QString& channel, const QString& propertyPath, const QString& value )
{
    QProcess::execute( "xfconf-query", QStringList() << "-c" << channel << "-p" << propertyPath << "-s" << value );
}

QVector< ThemePreset >
XFCETheme::availablePresets() const
{
    return {
        { "default-light", "Adwaita Light", "Xfce Defaults", false },
        { "default-dark", "Adwaita Dark", "Xfce Defaults", true },
        { "alg-light", "Qogir Light", "ALG Themes", false },
        { "alg-dark", "Qogir Dark", "ALG Themes", true },
    };
}

QString
XFCETheme::getXsettingsThemeName()
{
    const auto theme = getXfconfValue( "xsettings", "/Net/ThemeName" );
    spdlog::debug( "Current XFCE Theme: {}", theme.toStdString() );
    return theme;
}

QString
XFCETheme::currentPresetId()
{
    const auto theme = getXsettingsThemeName();
    const bool alg = theme.contains( "qogir", Qt::CaseInsensitive );
    const bool dark = isDarkTheme( theme );

    if ( alg )
    {
        return dark ? "alg-dark" : "alg-light";
    }
    return dark ? "default-dark" : "default-light";
}

void
XFCETheme::applyPreset( const QString& id )
{
    QString theme, icon, cursor;

    if ( id == "default-light" )
    {
        theme = "Adwaita";
        icon = "Adwaita";
        cursor = "default";
    }
    else if ( id == "default-dark" )
    {
        theme = "Adwaita-dark";
        icon = "Adwaita";
        cursor = "default";
    }
    else if ( id == "alg-light" )
    {
        theme = "Qogir-Light";
        icon = "Tela-circle";
        cursor = "McMojave-cursors";
    }
    else if ( id == "alg-dark" )
    {
        theme = "Qogir-Dark";
        icon = "Tela-circle-dark";
        cursor = "McMojave-cursors";
    }
    else
    {
        spdlog::warn( "Unknown XFCE theme preset: {}", id.toStdString() );
        return;
    }

    // Set GTK theme
    setXfconfValue( "xsettings", "/Net/ThemeName", theme );
    spdlog::debug( "XFCE GTK theme set to: {}", theme.toStdString() );

    // Set icon theme
    setXfconfValue( "xsettings", "/Net/IconThemeName", icon );
    spdlog::debug( "XFCE icon theme set to: {}", icon.toStdString() );

    // Set cursor theme
    setXfconfValue( "xsettings", "/Gtk/CursorThemeName", cursor );
    spdlog::debug( "XFCE cursor theme set to: {}", cursor.toStdString() );

    // Set window manager theme
    setXfconfValue( "xfwm4", "/general/theme", theme );
    spdlog::info( "XFCE theme changed to {}", theme.toStdString() );
}

// ============================================================================
// Factory Function
// ============================================================================

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

}  // namespace Themes
