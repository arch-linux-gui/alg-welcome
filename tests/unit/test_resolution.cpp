#include "utils/Resolution.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Resolution::commandFor selects the right tool per desktop environment", "[resolution][dispatch]" )
{
    CHECK( Resolution::commandFor( "xfce" ).program == "bash" );
    CHECK( Resolution::commandFor( "xfce" ).arguments == QStringList { "-c", "xfce4-display-settings" } );

    CHECK( Resolution::commandFor( "gnome" ).program == "gnome-control-center" );
    CHECK( Resolution::commandFor( "gnome" ).arguments == QStringList { "display" } );

    CHECK( Resolution::commandFor( "kde" ).program == "kcmshell6" );
    CHECK( Resolution::commandFor( "kde" ).arguments == QStringList { "kcm_kscreen" } );

    CHECK( Resolution::commandFor( "unknown-de" ).program.isEmpty() );
}
