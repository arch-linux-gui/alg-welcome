#include "utils/Updates.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Updates::updateArgs runs a full system upgrade via pacman", "[updates]" )
{
    CHECK( Updates::updateArgs() == QStringList { "pacman", "--noconfirm", "-Syu" } );
}

TEST_CASE( "Updates::syncArgs runs a database-only sync via pacman", "[updates]" )
{
    CHECK( Updates::syncArgs() == QStringList { "pacman", "--noconfirm", "-Syy" } );
}
