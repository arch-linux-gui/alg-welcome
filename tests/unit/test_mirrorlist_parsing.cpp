#include "dialogs/MirrorListParsing.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MirrorListParsing parses a matched server line", "[mirrorlist][parsing]") {
    const auto parsed = MirrorListParsing::parseLine(
        "[2024-01-15 10:30:45] INFO: https://mirror.example.com/archlinux 5.2 MiB/s 0.5 s");

    CHECK(parsed.server == "https://mirror.example.com/archlinux");
    CHECK(parsed.rate == "5.2 MiB/s");
    CHECK(parsed.time == "0.5 s");
    CHECK(parsed.logType.isEmpty());
}

TEST_CASE("MirrorListParsing parses an INFO line without a server pattern", "[mirrorlist][parsing]") {
    const auto parsed = MirrorListParsing::parseLine("[2024-01-15 10:30:45] INFO: Retrieving mirrorlist");

    CHECK(parsed.server == "Retrieving mirrorlist");
    CHECK(parsed.rate.isEmpty());
    CHECK(parsed.time.isEmpty());
    CHECK(parsed.logType.isEmpty());
}

TEST_CASE("MirrorListParsing parses a WARNING line", "[mirrorlist][parsing]") {
    const auto parsed = MirrorListParsing::parseLine("[2024-01-15 10:30:45] WARNING: Could not connect to mirror");

    CHECK(parsed.server == "Could not connect to mirror");
    CHECK(parsed.rate == "WARNING");
    CHECK(parsed.time == "N/A");
    CHECK(parsed.logType == "WARNING");
}

TEST_CASE("MirrorListParsing parses an ERROR line", "[mirrorlist][parsing]") {
    const auto parsed = MirrorListParsing::parseLine("[2024-01-15 10:30:45] ERROR: reflector failed");

    CHECK(parsed.server == "reflector failed");
    CHECK(parsed.rate == "ERROR");
    CHECK(parsed.time == "N/A");
    CHECK(parsed.logType == "ERROR");
}

TEST_CASE("MirrorListParsing passes an unmatched line through raw", "[mirrorlist][parsing]") {
    const auto parsed = MirrorListParsing::parseLine("some unstructured reflector output");

    CHECK(parsed.server == "some unstructured reflector output");
    CHECK(parsed.rate.isEmpty());
    CHECK(parsed.time.isEmpty());
    CHECK(parsed.logType.isEmpty());
}

TEST_CASE("MirrorListParsing handles an empty line", "[mirrorlist][parsing]") {
    const auto parsed = MirrorListParsing::parseLine("");

    CHECK(parsed.server.isEmpty());
    CHECK(parsed.logType.isEmpty());
}
