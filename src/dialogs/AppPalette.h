#ifndef APPPALETTE_H
#define APPPALETTE_H

#include <QString>

// The handful of color roles that vary between Archer's dark and light look. Everything else
// (layout, spacing, fonts, the "Detected: X" chip) is unaffected by system theme - only these
// tokens change based on which one (see Themes::isSystemDark()) is currently active.
namespace AppPalette
{

struct Palette
{
    bool dark = true;

    QString primaryText;  // headings, primary body text
    QString mutedText;  // secondary/help text, muted labels
    QString accentText;  // section labels and links set directly against the page background

    QString chromeBg;  // sub-page header bar / theme confirm bar / toast background
    QString chromeText;
    QString chromeAccent;  // sub-page header bar's "‹ Back" link color

    QString pageBg;  // ThemePage / AboutPage content background
    QString cardText;  // ThemePage preset card label text (unchecked)
    QString dividerColor;  // Home page's two horizontal separator lines
};

inline const Palette&
darkPalette()
{
    static const Palette palette {
        true, "#eef0f2", "#9aa0a8", "#a7adf0", "#24262b", "#eef0f2", "#8fb1ff", "#2b2e33", "#c7cbd1", "#37393e",
    };
    return palette;
}

inline const Palette&
lightPalette()
{
    static const Palette palette {
        false, "#2b2e33", "#6b7078", "#3d6fe0", "#e4e7eb", "#2b2e33", "#2f5fd0", "#f4f5f7", "#5a5d63", "#d7dade",
    };
    return palette;
}

inline const Palette&
forSystem( bool dark )
{
    return dark ? darkPalette() : lightPalette();
}

}  // namespace AppPalette

#endif  // APPPALETTE_H
