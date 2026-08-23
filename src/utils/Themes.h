#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QVector>
#include <memory>

namespace Themes
{

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

// Abstract base class for theme managers. Concrete per-DE implementations live under
// src/utils/themes/ (kde.h, gnome.h, xfce.h) - this header only owns the shared interface and
// the factory below, so callers that just need to drive a ThemeManager don't have to pull in
// every DE's implementation details.
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

// Factory function to get appropriate theme manager
std::unique_ptr< ThemeManager > getThemeManager( const QString& desktopEnv );

// Whether the desktop environment's *currently active* preset is a dark one - the single source
// of truth Archer's own UI uses to pick its own colors, so it never disagrees with what the
// Theme page itself shows as selected. Falls back to true (Archer's traditional look) for an
// unsupported/undetectable desktop environment.
bool isSystemDark( const QString& desktopEnv );

}  // namespace Themes

#endif  // THEMES_H
