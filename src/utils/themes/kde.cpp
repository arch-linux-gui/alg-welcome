#include "kde.h"

#include <spdlog/spdlog.h>

#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

namespace Themes
{

namespace
{

// Arch packages this at /usr/lib/plasma-changeicons rather than /usr/bin, so plain PATH
// resolution (what QProcess::execute(QString, ...) does) never finds it - confirmed by running
// into exactly that on a real system: the config write succeeded but no running app ever
// refreshed its icons since this call silently failed to even launch. Try the known install
// location first and only fall back to PATH resolution for other distros' packaging.
void
liveRefreshIconTheme( const QString& iconTheme )
{
    static const QString knownPath = "/usr/lib/plasma-changeicons";
    const QString program = QFile::exists( knownPath ) ? knownPath : QString( "plasma-changeicons" );
    QProcess::execute( program, QStringList() << iconTheme );
}

}  // namespace

// clang-format off
const KDETheme::PresetConfig KDETheme::DEFAULT_LIGHT_CONFIG = {
    "org.kde.breeze.desktop", "BreezeLight", "Breeze", "breeze-light",
    "org.kde.breeze", "", "breeze", "breeze_cursors"
};

const KDETheme::PresetConfig KDETheme::DEFAULT_DARK_CONFIG = {
    "org.kde.breezedark.desktop", "BreezeDark", "Breeze", "breeze-dark",
    "org.kde.breeze", "", "breeze-dark", "Breeze_Light"
};

const KDETheme::PresetConfig KDETheme::ALG_LIGHT_CONFIG = {
    "", "Qogirlight", "Breeze", "Qogir-dark",
    "org.kde.kwin.aurorae.v2", "__aurorae__svg__Qogir-light-circle", "Tela-circle", "mcmojave-cursors"
};

const KDETheme::PresetConfig KDETheme::ALG_DARK_CONFIG = {
    "", "Qogirdark", "Breeze", "Qogir-dark",
    "org.kde.kwin.aurorae.v2", "__aurorae__svg__Qogir-dark-circle", "Tela-circle-dark", "mcmojave-cursors"
};
// clang-format on

const KDETheme::PresetConfig*
KDETheme::configFor( const QString& id )
{
    if ( id == "default-dark" )
    {
        return &DEFAULT_DARK_CONFIG;
    }
    if ( id == "alg-light" )
    {
        return &ALG_LIGHT_CONFIG;
    }
    if ( id == "alg-dark" )
    {
        return &ALG_DARK_CONFIG;
    }
    if ( id == "default-light" )
    {
        return &DEFAULT_LIGHT_CONFIG;
    }
    return nullptr;
}

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
    const auto* config = configFor( id );
    if ( !config )
    {
        spdlog::warn( "Unknown KDE theme preset: {}", id.toStdString() );
        return;
    }

    // The stock Breeze look-and-feel package resets most of the below to Breeze's own as part of
    // applying the full bundle - but not the cursor theme the way this app wants it set (see
    // DEFAULT_LIGHT_CONFIG/DEFAULT_DARK_CONFIG's deliberately-swapped cursorTheme), so every
    // category below is still set explicitly afterwards rather than trusting the bundle alone.
    if ( !config->lookAndFeel.isEmpty() )
    {
        QProcess::execute( "lookandfeeltool", QStringList() << "--apply" << config->lookAndFeel );
    }

    QProcess::execute( "plasma-apply-colorscheme", QStringList() << config->colorScheme );

    QProcess::execute( "kwriteconfig6",
                       QStringList() << "--file" << homeDir + "/.config/kdeglobals" << "--group" << "KDE" << "--key"
                                     << "widgetStyle" << config->widgetStyle );

    QProcess::execute( "kwriteconfig6",
                       QStringList() << "--file" << homeDir + "/.config/plasmarc" << "--group" << "Theme" << "--key"
                                     << "name" << config->plasmaTheme );

    // Decoration *engine* (library) has to be set explicitly too, not just the Aurorae theme id -
    // otherwise switching to/from an Aurorae theme silently no-ops if the engine was left on the
    // other one.
    QProcess::execute( "kwriteconfig6",
                       QStringList() << "--file" << homeDir + "/.config/kwinrc" << "--group"
                                     << "org.kde.kdecoration2" << "--key" << "library" << config->decorationLibrary );
    if ( !config->decorationTheme.isEmpty() )
    {
        QProcess::execute( "kwriteconfig6",
                           QStringList() << "--file" << homeDir + "/.config/kwinrc" << "--group"
                                         << "org.kde.kdecoration2" << "--key" << "theme" << config->decorationTheme );
    }
    QProcess::execute( "qdbus6", QStringList() << "org.kde.KWin" << "/KWin" << "reconfigure" );

    QProcess::execute( "kwriteconfig6",
                       QStringList() << "--file" << homeDir + "/.config/kdeglobals" << "--group" << "Icons" << "--key"
                                     << "Theme" << config->iconTheme );
    liveRefreshIconTheme( config->iconTheme );

    QProcess::execute( "plasma-apply-cursortheme", QStringList() << config->cursorTheme );

    spdlog::info( "KDE theme changed to preset: {}", id.toStdString() );
}

}  // namespace Themes
