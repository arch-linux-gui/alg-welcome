#ifndef THEMES_GNOME_H
#define THEMES_GNOME_H

#include "../Themes.h"

namespace Themes
{

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

}  // namespace Themes

#endif  // THEMES_GNOME_H
