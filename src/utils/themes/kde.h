#ifndef THEMES_KDE_H
#define THEMES_KDE_H

#include "../Themes.h"

#include <QStringList>

namespace Themes
{

// KDE Plasma theme manager
class KDETheme : public ThemeManager
{
public:
    KDETheme();
    QVector< ThemePreset > availablePresets() const override;
    QString currentPresetId() override;
    void applyPreset( const QString& id ) override;

    // Public and static: neither touches instance state, and exposing them lets unit tests
    // exercise the parsing logic directly without needing a KDETheme instance or a live KDE.
    static QString
    getColorSchemeFromFile( const QString& configFile, const QString& sectionName, const QString& keyName );
    static QString formatColorScheme( const QString& colorScheme );

private:
    static constexpr const char* DEFAULT_COLOR_SCHEME = "org.kde.breeze.desktop";

    // Every setting System Settings' Appearance page exposes, applied explicitly per preset
    // rather than relying solely on lookandfeeltool's bundle - which, e.g., doesn't reset the
    // cursor theme the way this app wants (see PresetConfig::cursorTheme).
    struct PresetConfig
    {
        QString lookAndFeel;  // lookandfeeltool package id; empty to skip (ALG presets set
        // each piece directly instead of applying a bundle)
        QString colorScheme;  // plasma-apply-colorscheme argument
        QString widgetStyle;  // kdeglobals [KDE] widgetStyle
        QString plasmaTheme;  // plasmarc [Theme] name
        QString decorationLibrary;  // kwinrc [org.kde.kdecoration2] library
        QString decorationTheme;  // kwinrc [org.kde.kdecoration2] theme (aurorae svg id; empty
        // for Breeze's own built-in decoration)
        QString iconTheme;  // kdeglobals [Icons] Theme
        QString cursorTheme;  // plasma-apply-cursortheme argument
    };

    static const PresetConfig DEFAULT_LIGHT_CONFIG;
    static const PresetConfig DEFAULT_DARK_CONFIG;
    static const PresetConfig ALG_LIGHT_CONFIG;
    static const PresetConfig ALG_DARK_CONFIG;

    static const PresetConfig* configFor( const QString& id );

    QString getLookAndFeelPackage();

    QString homeDir;
    QStringList configFiles;
};

}  // namespace Themes

#endif  // THEMES_KDE_H
