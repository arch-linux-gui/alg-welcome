#include "xfce.h"

#include <spdlog/spdlog.h>

#include <QProcess>

namespace Themes
{

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
        cursor = "mcmojave-cursors";
    }
    else if ( id == "alg-dark" )
    {
        theme = "Qogir-Dark";
        icon = "Tela-circle-dark";
        cursor = "mcmojave-cursors";
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

}  // namespace Themes
