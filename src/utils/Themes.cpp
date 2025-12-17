#include "Themes.h"
#include "Logger.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>

namespace Themes {

// ============================================================================
// Helper Functions
// ============================================================================

bool isDarkTheme(const QString &theme) {
    const auto themeLower = theme.toLower();
    const QStringList darkKeywords = {
        "dark", "breezedark", "qogirdark", "prefer-dark", "orchis-dark"
    };
    
    for (const auto &keyword : darkKeywords) {
        if (themeLower.contains(keyword)) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// KDETheme Implementation
// ============================================================================

KDETheme::KDETheme() {
    homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    configFiles = {
        homeDir + "/.config/kdeglobals",
        homeDir + "/.kde4/share/config/kdeglobals",
        "/etc/kde/kdeglobals"
    };
}

QString KDETheme::getColorSchemeFromFile(const QString &configFile,
                                         const QString &sectionName,
                                         const QString &keyName) {
    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    
    QTextStream in(&file);
    QString currentSection;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        
        // Check for section headers
        if (line.startsWith('[') && line.endsWith(']')) {
            currentSection = line;
            continue;
        }
        
        // Check if we're in the target section and found the key
        if (currentSection == sectionName && line.startsWith(keyName + "=")) {
            const auto value = line.section('=', 1);
            file.close();
            return value;
        }
    }
    
    file.close();
    return QString();
}

QString KDETheme::formatColorScheme(const QString &colorScheme) {
    static const QMap<QString, QString> schemeMap = {
        {"org.kde.breeze.desktop", "org.kde.breeze.desktop"},
        {"org.kde.breezedark.desktop", "org.kde.breezedark.desktop"},
        {"breeze", "org.kde.breeze.desktop"},
        {"breezedark", "org.kde.breezedark.desktop"}
    };
    
    if (schemeMap.contains(colorScheme)) {
        return schemeMap[colorScheme];
    }
    
    // Handle themed KDE
    if (colorScheme.endsWith(".colors")) {
        return QFileInfo(colorScheme).baseName();
    }
    
    return colorScheme;
}

QString KDETheme::getLookAndFeelPackage() {
    for (const auto &configFile : configFiles) {
        // Try pure KDE approach
        auto colorScheme = getColorSchemeFromFile(configFile, "[KDE]", "LookAndFeelPackage");
        if (!colorScheme.isEmpty()) {
            const auto formatted = formatColorScheme(colorScheme);
            if (formatted != "org.kde.breeze.desktop" && formatted != "org.kde.breezedark.desktop") {
                // Try themed approach
                colorScheme = getColorSchemeFromFile(configFile, "[General]", "ColorScheme");
                if (!colorScheme.isEmpty()) {
                    return formatColorScheme(colorScheme);
                }
            } else {
                return formatted;
            }
        }
        
        // Try themed KDE approach
        colorScheme = getColorSchemeFromFile(configFile, "[General]", "ColorScheme");
        if (!colorScheme.isEmpty()) {
            return formatColorScheme(colorScheme);
        }
    }
    
    return DEFAULT_COLOR_SCHEME;
}

QString KDETheme::getCurrentTheme() {
    const auto theme = getLookAndFeelPackage();
    Logger::info("Current KDE Theme: " + theme);
    return theme;
}

void KDETheme::setTheme(bool dark) {
    const auto currentPackage = getLookAndFeelPackage();
    
    if (currentPackage.contains("org.kde.breeze")) {
        // Pure breeze theme
        const QString style = dark ? "org.kde.breezedark.desktop" : "org.kde.breeze.desktop";
        QProcess::execute("lookandfeeltool", QStringList() << "--apply" << style);
        Logger::info("KDE theme changed to " + style);
    } else {
        // Themed KDE (Qogir)
        QString style, winDeco;
        if (dark) {
            style = "Qogirdark";
            winDeco = "__aurorae__svg__Qogir-dark-circle";
        } else {
            style = "Qogirlight";
            winDeco = "__aurorae__svg__Qogir-light-circle";
        }
        
        const QString cmd = QString(
            "plasma-apply-colorscheme %1 && "
            "kwriteconfig6 --file %2/.config/kwinrc "
            "--group org.kde.kdecoration2 --key theme %3 && "
            "qdbus6 org.kde.KWin /KWin reconfigure"
        ).arg(style, homeDir, winDeco);
        
        QProcess::execute("sh", QStringList() << "-c" << cmd);
        Logger::info("KDE theme changed to " + style);
    }
}

// ============================================================================
// GNOMETheme Implementation
// ============================================================================

const GNOMETheme::ThemeConfig GNOMETheme::DARK_THEME = {
    "Tela-circle-dark",
    "Orchis-Red-Dark",
    "Orchis-Red-Dark",
    "prefer-dark"
};

const GNOMETheme::ThemeConfig GNOMETheme::LIGHT_THEME = {
    "Tela-circle",
    "Orchis-Red-Light",
    "Orchis-Red-Light",
    "prefer-light"
};

GNOMETheme::GNOMETheme() {
    // Constructor
}

QString GNOMETheme::getGSetting(const QString &schema, const QString &key) {
    QProcess process;
    process.start("gsettings", QStringList() << "get" << schema << key);
    process.waitForFinished();
    
    auto output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    // Remove quotes
    output.remove('\'');
    output.remove('\"');
    return output;
}

void GNOMETheme::setGSetting(const QString &schema, const QString &key, const QString &value) {
    QProcess::execute("gsettings", QStringList() << "set" << schema << key << value);
}

QString GNOMETheme::getCurrentTheme() {
    const auto colorScheme = getGSetting("org.gnome.desktop.interface", "color-scheme");
    const auto gtkTheme = getGSetting("org.gnome.desktop.interface", "gtk-theme");
    
    // Determine theme based on color scheme or GTK theme
    const auto theme = !colorScheme.isEmpty() ? colorScheme : gtkTheme;
    Logger::info("Current GNOME Theme: " + theme);
    return theme;
}

void GNOMETheme::setTheme(bool dark) {
    const auto &themeConfig = dark ? DARK_THEME : LIGHT_THEME;
    
    // Set icon theme
    setGSetting("org.gnome.desktop.interface", "icon-theme", themeConfig.icons);
    Logger::info("GNOME icons set to: " + themeConfig.icons);
    
    // Set GTK theme (Legacy Applications)
    setGSetting("org.gnome.desktop.interface", "gtk-theme", themeConfig.gtk);
    Logger::info("GNOME GTK theme set to: " + themeConfig.gtk);
    
    // Set color scheme
    setGSetting("org.gnome.desktop.interface", "color-scheme", themeConfig.colorScheme);
    Logger::info("GNOME color scheme set to: " + themeConfig.colorScheme);
    
    // Set shell theme (requires user-theme extension)
    setGSetting("org.gnome.shell.extensions.user-theme", "name", themeConfig.shell);
    Logger::info("GNOME shell theme set to: " + themeConfig.shell);
}

// ============================================================================
// XFCETheme Implementation
// ============================================================================

XFCETheme::XFCETheme() {
    // Constructor
}

QString XFCETheme::getXfconfValue(const QString &channel, const QString &propertyPath) {
    QProcess process;
    process.start("xfconf-query", QStringList() << "-c" << channel << "-p" << propertyPath);
    process.waitForFinished();
    
    return QString::fromUtf8(process.readAllStandardOutput()).trimmed();
}

void XFCETheme::setXfconfValue(const QString &channel, const QString &propertyPath, const QString &value) {
    QProcess::execute("xfconf-query", QStringList() << "-c" << channel << "-p" << propertyPath << "-s" << value);
}

QString XFCETheme::getCurrentTheme() {
    const auto theme = getXfconfValue("xsettings", "/Net/ThemeName");
    Logger::info("Current XFCE Theme: " + theme);
    return theme;
}

void XFCETheme::setTheme(bool dark) {
    const auto currentTheme = getCurrentTheme();
    
    // Determine theme based on current theme
    QString style;
    if (currentTheme.contains("Qogir")) {
        style = dark ? "Qogir-Dark" : "Qogir-Light";
    } else {
        style = dark ? "Adwaita-dark" : "Adwaita";
    }
    
    // Set GTK theme
    setXfconfValue("xsettings", "/Net/ThemeName", style);
    Logger::info("XFCE GTK theme set to: " + style);
    
    // Set window manager theme
    setXfconfValue("xfwm4", "/general/theme", style);
    Logger::info("XFCE WM theme set to: " + style);
}

// ============================================================================
// Factory Function
// ============================================================================

std::unique_ptr<ThemeManager> getThemeManager(const QString &desktopEnv) {
    if (desktopEnv == "kde") {
        return std::make_unique<KDETheme>();
    } else if (desktopEnv == "gnome") {
        return std::make_unique<GNOMETheme>();
    } else if (desktopEnv == "xfce") {
        return std::make_unique<XFCETheme>();
    } else {
        Logger::warning("Unsupported desktop environment: " + desktopEnv);
        return nullptr;
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

QString getCurrentTheme(const QString &desktopEnv) {
    auto manager = getThemeManager(desktopEnv);
    if (manager) {
        return manager->getCurrentTheme();
    }
    return QString();
}

void toggleTheme(bool dark, const QString &desktopEnv) {
    auto manager = getThemeManager(desktopEnv);
    if (manager) {
        manager->setTheme(dark);
    } else {
        Logger::warning("Cannot toggle theme for unsupported desktop environment: " + desktopEnv);
    }
}

} // namespace Themes
