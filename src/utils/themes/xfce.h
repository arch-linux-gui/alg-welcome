#ifndef THEMES_XFCE_H
#define THEMES_XFCE_H

#include "../Themes.h"

namespace Themes
{

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

}  // namespace Themes

#endif  // THEMES_XFCE_H
