#include "utils/Updates.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Updates::commandFor selects the right tool per desktop environment", "[updates][dispatch]" )
{
    CHECK( Updates::commandFor( "xfce" ).program == "xfce4-terminal" );
    CHECK( Updates::commandFor( "xfce" ).arguments == QStringList { "-x", "pkexec", "pacman", "--noconfirm", "-Syu" } );

    CHECK( Updates::commandFor( "gnome" ).program == "kgx" );
    CHECK( Updates::commandFor( "gnome" ).arguments == QStringList { "--", "sudo", "pacman", "--noconfirm", "-Syu" } );

    CHECK( Updates::commandFor( "kde" ).program == "konsole" );
    CHECK( Updates::commandFor( "kde" ).arguments == QStringList { "-e", "sudo", "pacman", "--noconfirm", "-Syu" } );

    CHECK( Updates::commandFor( "unknown-de" ).program.isEmpty() );
}
