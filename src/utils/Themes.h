#ifndef THEMES_H
#define THEMES_H

#include <QMap>
#include <QString>
#include <QVector>
#include <memory>

namespace Themes
{

// Forward declarations
class ThemeManager;

// Check if a theme name indicates a dark theme
bool isDarkTheme( const QString& theme );

// A single selectable theme option. Every desktop environment offers exactly four: the DE's own
// stock look ("Default") and the ALG look ("ALG Theme"), each in a light and a dark variant.
struct ThemePreset
{
    QString id;  // "default-light" | "default-dark" | "alg-light" | "alg-dark"
    QString displayName;  // e.g. "Breeze Dark", "Qogir Light" - what the picker shows
    QString family;  // "Default" or "ALG Theme" - for grouping in the picker
    bool isDark = false;
};

// Abstract base class for theme managers
class ThemeManager
{
public:
    virtual ~ThemeManager() = default;

    // Pure: the four presets this DE supports, in display order.
    virtual QVector< ThemePreset > availablePresets() const = 0;
    // Reads live desktop state to determine which preset (if any) is currently active.
    virtual QString currentPresetId() = 0;
    virtual void applyPreset( const QString& id ) = 0;
};

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

    QString getLookAndFeelPackage();

    QString homeDir;
    QStringList configFiles;
};

// GNOME theme manager
class GNOMETheme : public ThemeManager
{
public:
    GNOMETheme();
    QVector< ThemePreset > availablePresets() const override;
    QString currentPresetId() override;
    void applyPreset( const QString& id ) override;

private:
    struct ThemeConfig
    {
        QString icons;
        QString shell;
        QString gtk;
        QString colorScheme;
        QString cursor;
    };

    static const ThemeConfig DEFAULT_LIGHT_THEME;
    static const ThemeConfig DEFAULT_DARK_THEME;
    static const ThemeConfig ALG_LIGHT_THEME;
    static const ThemeConfig ALG_DARK_THEME;

    static const ThemeConfig* configFor( const QString& id );

    QString getGSetting( const QString& schema, const QString& key );
    void setGSetting( const QString& schema, const QString& key, const QString& value );
};

// XFCE theme manager
class XFCETheme : public ThemeManager
{
public:
    XFCETheme();
    QVector< ThemePreset > availablePresets() const override;
    QString currentPresetId() override;
    void applyPreset( const QString& id ) override;

private:
    QString getXsettingsThemeName();
    QString getXfconfValue( const QString& channel, const QString& propertyPath );
    void setXfconfValue( const QString& channel, const QString& propertyPath, const QString& value );
};

// Factory function to get appropriate theme manager
std::unique_ptr< ThemeManager > getThemeManager( const QString& desktopEnv );

}  // namespace Themes

#endif  // THEMES_H
