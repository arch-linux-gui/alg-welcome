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
- `PKGBUILD` for a proper pacman-managed install (`makepkg -si`). `depends` is derived from
  auditing every external binary the app actually shells out to (`grep`-ing every
  `QProcess::start`/`startDetached`/`execute` call site and the strings passed to `bash -c`/
  `sh -c`): `qt6-base`, `spdlog`, `fmt` (this app's own libraries), `polkit` (`pkexec`),
  `xdg-utils` (`xdg-open`), `reflector`, and `sudo`. Desktop-environment-specific tools
  (`kcmshell6`/`qdbus6`/`lookandfeeltool`/`plasma-apply-colorscheme`/`kwriteconfig6`/`konsole` for
  KDE; `gnome-control-center`/`kgx`/`gsettings` for GNOME; `xfce4-display-settings`/
  `xfconf-query`/`xfce4-terminal` for Xfce) are `optdepends`, not `depends` — a given ALG edition
  only needs its own DE's tools, and every edition already ships them as part of the desktop
  install itself, so hard-depending on all three would bloat every install regardless of DE.
  `pkgver()` derives from the repo's own `VERSION` file rather than a git-describe heuristic, since
  `VERSION` is already this project's single source of truth (M1). `build.yml` gained a separate
  `format` job (`clang-format --dry-run --Werror`, parallel to `build` so a formatting nit doesn't
  wait behind a full build to be reported) now that the whole tree has been brought into
  `.clang-format` compliance (see below). `release.yml` gained a `package` job (`needs: [build,
  release]`, since the git tag the PKGBUILD's source pins to only exists once the `release` job
  creates it) that builds the `.pkg.tar.zst` in a clean chroot via `devtools`' `extra-x86_64-build`
  (not a bare `makepkg` in the already-populated CI container, so the `depends` list is verified
  against a minimal base) and attaches it to the GitHub Release alongside the existing raw
  `tar.gz`. Marked `continue-on-error: true` and gated on the release job having already
  succeeded, so a package-build failure is reported but never blocks the plain-binary release.

- `sanitize` job in `build.yml`: a Debug build with `-fsanitize=address,undefined
  -fno-omit-frame-pointer` (and the matching linker flag), running the full `ctest` suite under
  ASan/UBSan on every PR. Parallel to `format`/`build`, not gating them, so a sanitizer regression
  is reported without slowing down the plain build.

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
- Root `CMakeLists.txt` split by directory: it now only declares the project/version, runs
  `find_package()`, creates the `archer` target from `src/main.cpp`, links libraries, and
  `add_subdirectory(src)`/`add_subdirectory(tests)`s. `src/CMakeLists.txt`,
  `src/dialogs/CMakeLists.txt`, and `src/utils/CMakeLists.txt` each own a
  `target_sources(archer PRIVATE ...)` block for their own files. Pure build-system
  reorganization — no source, target, or install-rule behavior changed; verified via a clean
  rebuild, a full `ctest` pass, and an equivalent `compile_commands.json`.
- Reformatted every `.cpp`/`.h` under `src/` and `tests/` with `clang-format` per the existing
  (previously unenforced) `.clang-format` config, so the new `format` CI job (see above) starts
  from a clean baseline instead of being red from its first run. Whitespace/brace-style only — no
  behavior change; verified via a clean rebuild and a full `ctest` pass before and after.
- `CMAKE_CXX_STANDARD` bumped from 17 to 20, unlocking `std::jthread` for the RAII fix below. Built
  clean with zero new warnings; nothing else in the codebase or its Qt6 dependency needed to
  change.
- `MirrorListDialog`'s two rule-of-zero violations (the only hand-written destructor in the
  codebase) removed: `MirrorListSignals* workerSignals` (heap-allocated, manually `delete`d) is now
  a plain value member — no allocation at all, since its lifetime is exactly the dialog's — and
  `std::unique_ptr<std::thread> updateThread` (manually `join()`ed) is now a `std::jthread`, which
  auto-joins on destruction. `~MirrorListDialog()` is gone entirely; the implicit destructor is
  correct. Verified via a full rebuild and `ctest` pass under `-fsanitize=address,undefined` (see
  the new `sanitize` CI job above).
- `WelcomeWindow`'s three near-identical "try the installed path, fall back to the current
  directory" loops (window icon, header logo, stylesheet) consolidated into one
  `WelcomeWindow::resolveExistingPath()` helper.
- `-n`/`--no-autostart` (registered on the CLI parser since M1 but never actually checked) is now
  wired up: `main()` skips the boot-time `Autostart::toggleAutostart(true)` call when the flag is
  set, matching the option's own `--help` description.
- Remaining "Arch Linux GUI" branding text replaced with "Arka Linux GUI" in `main.cpp`
  (`QApplication::setOrganizationName()`, `setApplicationDescription()`) and `PKGBUILD`'s
  `pkgdesc`. Left unchanged: mentions of the Arch Linux *distribution* itself (README's "On Arch
  Linux:", the `archlinux:latest` CI container images, the reflector mirror path in the test
  fixture) and the `arch-linux-gui` GitHub org slug in repo URLs, which is confirmed live via
  `git remote -v` and not a branding string to rewrite.

### Removed

- Dead code: `WelcomeWindow::onTutorials()` (defined, never connected to any widget), the
  commented-out `app.setApplicationName("Welcome to ALG")` line in `main.cpp`, and the
  explicitly-marked "backward-compat overload (unused)" `MirrorListDialog::startMirrorListUpdate(const
  QString&)`.
