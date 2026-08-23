#include "utils/Themes.h"
#include "utils/themes/gnome.h"
#include "utils/themes/kde.h"
#include "utils/themes/xfce.h"

#include <catch2/catch_test_macros.hpp>

#include <QTemporaryFile>
#include <QTextStream>

TEST_CASE( "isDarkTheme recognizes dark theme name keywords", "[themes]" )
{
    CHECK( Themes::isDarkTheme( "BreezeDark" ) );
    CHECK( Themes::isDarkTheme( "prefer-dark" ) );
    CHECK( Themes::isDarkTheme( "Orchis-Dark" ) );
    CHECK_FALSE( Themes::isDarkTheme( "Breeze" ) );
    CHECK_FALSE( Themes::isDarkTheme( "Adwaita" ) );
}

TEST_CASE( "KDETheme::formatColorScheme maps known aliases", "[themes][kde]" )
{
    CHECK( Themes::KDETheme::formatColorScheme( "breeze" ) == "org.kde.breeze.desktop" );
    CHECK( Themes::KDETheme::formatColorScheme( "breezedark" ) == "org.kde.breezedark.desktop" );
    CHECK( Themes::KDETheme::formatColorScheme( "org.kde.breeze.desktop" ) == "org.kde.breeze.desktop" );
}

TEST_CASE( "KDETheme::formatColorScheme strips a .colors suffix down to its basename", "[themes][kde]" )
{
    CHECK( Themes::KDETheme::formatColorScheme( "/home/user/.local/share/color-schemes/OrchisDark.colors" )
           == "OrchisDark" );
}

TEST_CASE( "KDETheme::formatColorScheme passes through unknown values unchanged", "[themes][kde]" )
{
    CHECK( Themes::KDETheme::formatColorScheme( "SomeCustomScheme" ) == "SomeCustomScheme" );
}

TEST_CASE( "KDETheme::getColorSchemeFromFile reads a matching key from an INI-style file", "[themes][kde]" )
{
    QTemporaryFile file;
    REQUIRE( file.open() );
    {
        QTextStream out( &file );
        out << "[General]\n";
        out << "ColorScheme=BreezeDark\n";
        out << "[KDE]\n";
        out << "LookAndFeelPackage=org.kde.breezedark.desktop\n";
    }
    file.flush();

    CHECK( Themes::KDETheme::getColorSchemeFromFile( file.fileName(), "[General]", "ColorScheme" ) == "BreezeDark" );
    CHECK( Themes::KDETheme::getColorSchemeFromFile( file.fileName(), "[KDE]", "LookAndFeelPackage" )
           == "org.kde.breezedark.desktop" );
}

TEST_CASE( "KDETheme::getColorSchemeFromFile returns empty for a missing key", "[themes][kde]" )
{
    QTemporaryFile file;
    REQUIRE( file.open() );
    {
        QTextStream out( &file );
        out << "[General]\nSomeOtherKey=x\n";
    }
    file.flush();

    CHECK( Themes::KDETheme::getColorSchemeFromFile( file.fileName(), "[General]", "ColorScheme" ).isEmpty() );
}

TEST_CASE( "KDETheme::getColorSchemeFromFile returns empty for a nonexistent file", "[themes][kde]" )
{
    CHECK( Themes::KDETheme::getColorSchemeFromFile( "/nonexistent/path/kdeglobals", "[General]", "ColorScheme" )
               .isEmpty() );
}

TEST_CASE( "getThemeManager dispatches to the right implementation per desktop environment", "[themes][dispatch]" )
{
    CHECK( dynamic_cast< Themes::KDETheme* >( Themes::getThemeManager( "kde" ).get() ) != nullptr );
    CHECK( dynamic_cast< Themes::GNOMETheme* >( Themes::getThemeManager( "gnome" ).get() ) != nullptr );
    CHECK( dynamic_cast< Themes::XFCETheme* >( Themes::getThemeManager( "xfce" ).get() ) != nullptr );
    CHECK( Themes::getThemeManager( "unknown-de" ) == nullptr );
}

namespace
{

void
checkFourStandardPresets( const QVector< Themes::ThemePreset >& presets, const QString& defaultFamily )
{
    REQUIRE( presets.size() == 4 );

    CHECK( presets[ 0 ].id == "default-light" );
    CHECK( presets[ 0 ].family == defaultFamily );
    CHECK_FALSE( presets[ 0 ].isDark );

    CHECK( presets[ 1 ].id == "default-dark" );
    CHECK( presets[ 1 ].family == defaultFamily );
    CHECK( presets[ 1 ].isDark );

    CHECK( presets[ 2 ].id == "alg-light" );
    CHECK( presets[ 2 ].family == "ALG Themes" );
    CHECK_FALSE( presets[ 2 ].isDark );

    CHECK( presets[ 3 ].id == "alg-dark" );
    CHECK( presets[ 3 ].family == "ALG Themes" );
    CHECK( presets[ 3 ].isDark );
}

}  // namespace

TEST_CASE( "KDETheme::availablePresets offers Breeze and Qogir, light and dark", "[themes][kde]" )
{
    Themes::KDETheme theme;
    const auto presets = theme.availablePresets();
    checkFourStandardPresets( presets, "KDE Defaults" );

    CHECK( presets[ 0 ].displayName == "Breeze Light" );
    CHECK( presets[ 1 ].displayName == "Breeze Dark" );
    CHECK( presets[ 2 ].displayName == "Qogir Light" );
    CHECK( presets[ 3 ].displayName == "Qogir Dark" );
}

TEST_CASE( "GNOMETheme::availablePresets offers Adwaita and Orchis Red, light and dark", "[themes][gnome]" )
{
    Themes::GNOMETheme theme;
    const auto presets = theme.availablePresets();
    checkFourStandardPresets( presets, "GNOME Defaults" );

    CHECK( presets[ 0 ].displayName == "Adwaita Light" );
    CHECK( presets[ 1 ].displayName == "Adwaita Dark" );
    CHECK( presets[ 2 ].displayName == "Orchis Red Light" );
    CHECK( presets[ 3 ].displayName == "Orchis Red Dark" );
}

TEST_CASE( "XFCETheme::availablePresets offers Adwaita and Qogir, light and dark", "[themes][xfce]" )
{
    Themes::XFCETheme theme;
    const auto presets = theme.availablePresets();
    checkFourStandardPresets( presets, "Xfce Defaults" );

    CHECK( presets[ 0 ].displayName == "Adwaita Light" );
    CHECK( presets[ 1 ].displayName == "Adwaita Dark" );
    CHECK( presets[ 2 ].displayName == "Qogir Light" );
    CHECK( presets[ 3 ].displayName == "Qogir Dark" );
}
