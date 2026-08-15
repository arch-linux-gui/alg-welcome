#include "utils/Themes.h"

#include <catch2/catch_test_macros.hpp>

#include <QTemporaryFile>
#include <QTextStream>

TEST_CASE("isDarkTheme recognizes dark theme name keywords", "[themes]") {
    CHECK(Themes::isDarkTheme("BreezeDark"));
    CHECK(Themes::isDarkTheme("prefer-dark"));
    CHECK(Themes::isDarkTheme("Orchis-Dark"));
    CHECK_FALSE(Themes::isDarkTheme("Breeze"));
    CHECK_FALSE(Themes::isDarkTheme("Adwaita"));
}

TEST_CASE("KDETheme::formatColorScheme maps known aliases", "[themes][kde]") {
    CHECK(Themes::KDETheme::formatColorScheme("breeze") == "org.kde.breeze.desktop");
    CHECK(Themes::KDETheme::formatColorScheme("breezedark") == "org.kde.breezedark.desktop");
    CHECK(Themes::KDETheme::formatColorScheme("org.kde.breeze.desktop") == "org.kde.breeze.desktop");
}

TEST_CASE("KDETheme::formatColorScheme strips a .colors suffix down to its basename", "[themes][kde]") {
    CHECK(Themes::KDETheme::formatColorScheme("/home/user/.local/share/color-schemes/OrchisDark.colors")
          == "OrchisDark");
}

TEST_CASE("KDETheme::formatColorScheme passes through unknown values unchanged", "[themes][kde]") {
    CHECK(Themes::KDETheme::formatColorScheme("SomeCustomScheme") == "SomeCustomScheme");
}

TEST_CASE("KDETheme::getColorSchemeFromFile reads a matching key from an INI-style file", "[themes][kde]") {
    QTemporaryFile file;
    REQUIRE(file.open());
    {
        QTextStream out(&file);
        out << "[General]\n";
        out << "ColorScheme=BreezeDark\n";
        out << "[KDE]\n";
        out << "LookAndFeelPackage=org.kde.breezedark.desktop\n";
    }
    file.flush();

    CHECK(Themes::KDETheme::getColorSchemeFromFile(file.fileName(), "[General]", "ColorScheme") == "BreezeDark");
    CHECK(Themes::KDETheme::getColorSchemeFromFile(file.fileName(), "[KDE]", "LookAndFeelPackage")
          == "org.kde.breezedark.desktop");
}

TEST_CASE("KDETheme::getColorSchemeFromFile returns empty for a missing key", "[themes][kde]") {
    QTemporaryFile file;
    REQUIRE(file.open());
    {
        QTextStream out(&file);
        out << "[General]\nSomeOtherKey=x\n";
    }
    file.flush();

    CHECK(Themes::KDETheme::getColorSchemeFromFile(file.fileName(), "[General]", "ColorScheme").isEmpty());
}

TEST_CASE("KDETheme::getColorSchemeFromFile returns empty for a nonexistent file", "[themes][kde]") {
    CHECK(Themes::KDETheme::getColorSchemeFromFile("/nonexistent/path/kdeglobals", "[General]", "ColorScheme")
          .isEmpty());
}

TEST_CASE("getThemeManager dispatches to the right implementation per desktop environment", "[themes][dispatch]") {
    CHECK(dynamic_cast<Themes::KDETheme *>(Themes::getThemeManager("kde").get()) != nullptr);
    CHECK(dynamic_cast<Themes::GNOMETheme *>(Themes::getThemeManager("gnome").get()) != nullptr);
    CHECK(dynamic_cast<Themes::XFCETheme *>(Themes::getThemeManager("xfce").get()) != nullptr);
    CHECK(Themes::getThemeManager("unknown-de") == nullptr);
}
