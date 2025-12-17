#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QMap>
#include <memory>

namespace Themes {

// Forward declarations
class ThemeManager;

// Check if a theme name indicates a dark theme
bool isDarkTheme(const QString &theme);

// Get current theme for the desktop environment
QString getCurrentTheme(const QString &desktopEnv);

// Toggle between light and dark theme
void toggleTheme(bool dark, const QString &desktopEnv);

// Abstract base class for theme managers
class ThemeManager {
public:
    virtual ~ThemeManager() = default;
    
    virtual QString getCurrentTheme() = 0;
    virtual void setTheme(bool dark) = 0;
};

// KDE Plasma theme manager
class KDETheme : public ThemeManager {
public:
    KDETheme();
    QString getCurrentTheme() override;
    void setTheme(bool dark) override;
    
private:
    static constexpr const char* DEFAULT_COLOR_SCHEME = "org.kde.breeze.desktop";
    
    QString getColorSchemeFromFile(const QString &configFile, 
                                   const QString &sectionName, 
                                   const QString &keyName);
    QString formatColorScheme(const QString &colorScheme);
    QString getLookAndFeelPackage();
    
    QString homeDir;
    QStringList configFiles;
};

// GNOME theme manager
class GNOMETheme : public ThemeManager {
public:
    GNOMETheme();
    QString getCurrentTheme() override;
    void setTheme(bool dark) override;
    
private:
    struct ThemeConfig {
        QString icons;
        QString shell;
        QString gtk;
        QString colorScheme;
    };
    
    static const ThemeConfig DARK_THEME;
    static const ThemeConfig LIGHT_THEME;
    
    QString getGSetting(const QString &schema, const QString &key);
    void setGSetting(const QString &schema, const QString &key, const QString &value);
};

// XFCE theme manager
class XFCETheme : public ThemeManager {
public:
    XFCETheme();
    QString getCurrentTheme() override;
    void setTheme(bool dark) override;
    
private:
    QString getXfconfValue(const QString &channel, const QString &propertyPath);
    void setXfconfValue(const QString &channel, const QString &propertyPath, const QString &value);
};

// Factory function to get appropriate theme manager
std::unique_ptr<ThemeManager> getThemeManager(const QString &desktopEnv);

} // namespace Themes

#endif // THEMES_H
