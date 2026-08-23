#include "gnome.h"

#include <spdlog/spdlog.h>

#include <QProcess>

namespace Themes
{

const GNOMETheme::ThemeConfig GNOMETheme::DEFAULT_LIGHT_THEME = { "Adwaita", "", "Adwaita", "prefer-light", "Adwaita" };

const GNOMETheme::ThemeConfig GNOMETheme::DEFAULT_DARK_THEME
    = { "Adwaita", "", "Adwaita-dark", "prefer-dark", "Adwaita" };

const GNOMETheme::ThemeConfig GNOMETheme::ALG_LIGHT_THEME
    = { "Tela-circle", "Orchis-Red-Light", "Orchis-Red-Light", "prefer-light", "mcmojave-cursors" };

const GNOMETheme::ThemeConfig GNOMETheme::ALG_DARK_THEME
    = { "Tela-circle-dark", "Orchis-Red-Dark", "Orchis-Red-Dark", "prefer-dark", "mcmojave-cursors" };

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

}  // namespace Themes
