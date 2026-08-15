# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added

- `VERSION` file as the single source of truth for the project version.
- Pre-commit hook (`scripts/bump-version.sh`, installed via `scripts/install-hooks.sh`) that
  auto-bumps the patch component of `VERSION` on every commit. Minor/major bumps stay a
  deliberate manual edit.
- This `CHANGELOG.md`.
- `spdlog`/`fmt`-based logging: a small `src/utils/Logging.{h,cpp}` component configures the
  process-wide default logger (colored stderr sink + file sink, preferring
  `/var/log/archer/archer.log` and falling back to a writable temp location) once at the very top
  of `main()`, before `QApplication` is constructed.
- CLI verbosity flags: `-d`/`--debug` (debug level), repeatable `-v`/`-vv`/`--verbose` (debug/trace,
  ssh-style), and an explicit `--log-level <name>` for scripting. Resolved from raw `argv` before
  `QApplication` exists so logging is live from the first line of `main()`.
- `--version` is now a manually-registered long-only option (`QCommandLineParser::addVersionOption()`
  was dropped) so `-v` is free to mean "verbose" instead of "version".
- Catch2-via-CTest test suite (`tests/CMakeLists.txt`, `find_package(Catch2 3 QUIET)` with a
  `FetchContent` fallback), replacing the old hand-rolled `calamares_status_test` binary. One
  `archer_tests` binary, `catch_discover_tests`-registered so `ctest` reports per-`TEST_CASE`
  pass/fail. Covers: the Calamares test hook (as before) plus a real assertion against the new
  `/proc`-based `isProcessRunning()` probe (spawns and kills a real child process — no dependency
  on Calamares being installed); the actual-Calamares-detection case uses Catch2's `SKIP()` with an
  explanation when `/usr/bin/calamares` isn't present, which is always true in CI; `KDETheme`'s
  color-scheme parsing; `getThemeManager`'s DE dispatch; `Resolution`/`Updates`'s new `commandFor()`
  DE dispatch (see below); and `MirrorListParsing::parseLine`'s regex parsing (see below).
  `ctest --output-on-failure` wired into both `build.yml` and `release.yml`, and `catch2` added to
  both workflows' `pacman -S` lists and the README prerequisites/install line.

### Changed

- `CMakeLists.txt` now reads `PROJECT_VERSION` from `VERSION` via `file(STRINGS ...)` instead of
  a version hardcoded in the `project()` call.
- `release.yml` now extracts the release version from `VERSION` instead of grepping
  `CMakeLists.txt`, and the release-trigger check and path filter both watch `VERSION` instead of
  `CMakeLists.txt`.
- Renamed the project/binary/package identity from `alg-welcome` to `archer` (part of the
  ALG-wide push toward recognizable, HPC-cluster-inspired codenames, mirroring
  `alg-installer` → *Challenger*). Affects: CMake project/target name, the
  `ARCHER_VERSION` compile definition (was `ALG_WELCOME_VERSION`), the desktop file
  (`alg-welcome.desktop` → `archer.desktop`), the app icon (`welcome.png` → `archer.png`), install
  destinations (`share/alg-welcome` → `share/archer`), the autostart source/target desktop-file
  paths, `app.setDesktopFileName()`, and all CI/release artifact names and install instructions.
  This is a full rebrand, not just an internal codename: the window title, the About Us dialog's
  app-name label, and the desktop entry `Name=` are all now `Archer` (desktop `Comment=ALG Welcome
  and onboarding utility`). The in-window header text ("Welcome to ALG!") is unchanged.
- Replaced the ad-hoc `src/utils/Logger.h` wrapper (a thin `qDebug()`/`qWarning()`/`qCritical()`
  wrapper) and ~70 raw `qDebug()`/`Logger::` call sites across `main.cpp`, `WelcomeWindow.cpp`,
  `Autostart.cpp`, `Themes.cpp`, `Resolution.cpp`, `Updates.cpp`, `Extras.cpp`, and
  `MirrorListDialog.cpp` with direct `spdlog::` calls at appropriate levels. `MirrorListDialog.cpp`
  in particular had its firehose of tracing releveled: routine UI plumbing and per-line parsing
  detail moved to `trace`, operation parameters to `debug`, the update start/finish bookends to
  `info`, and `reflector`'s own `WARNING`/`ERROR` output now surfaces at the matching `spdlog`
  level. A few redundant lines (derivable from adjacent ones) were deleted outright rather than
  releveled.
- `find_package(spdlog REQUIRED)` / `find_package(fmt REQUIRED)` added to `CMakeLists.txt`, linked
  into both the `archer` and `calamares_status_test` targets; `spdlog fmt` added to both CI
  workflows' `pacman -S` dependency lists and to the README prerequisites/install line.
- `Extras::isCalamaresRunning()` no longer only trusts this app's own in-process
  `std::atomic<bool>` bookkeeping (set only around this app's own launch of Calamares, so it
  never noticed a Calamares instance started any other way, or one still running across an app
  restart). It now also probes `/proc` for a real running `calamares` process via a new
  `Extras::isProcessRunning(name)` helper, and returns true if either signal says so. The
  `ENABLE_TEST_HOOKS`/`setCalamaresRunningForTest()` seam is unchanged.
- `Resolution::screenResolution()` and `Updates::updateSystem()` each split into a pure
  `commandFor(desktopEnv)` (which DE maps to which program/arguments — now unit-testable without
  launching anything) and the existing execution logic, which is otherwise unchanged (including
  the KDE branches' environment-variable stripping).
- `MirrorListDialog::processLogLine()`'s reflector-output regex parsing extracted into a new pure
  `MirrorListParsing::parseLine()` (`src/dialogs/MirrorListParsing.{h,cpp}`, no `QWidget`
  dependency), now unit-tested directly with sample `reflector --verbose` lines. `processLogLine()`
  itself is now a thin wrapper that also logs `reflector`'s own `WARNING`/`ERROR` lines at the
  matching `spdlog` level.
- `KDETheme::getColorSchemeFromFile()` and `KDETheme::formatColorScheme()` made `public static`
  (neither touched instance state) so they're unit-testable without a live KDE session.
