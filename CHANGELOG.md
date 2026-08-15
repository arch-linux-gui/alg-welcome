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

### Changed

- `CMakeLists.txt` now reads `PROJECT_VERSION` from `VERSION` via `file(STRINGS ...)` instead of
  a version hardcoded in the `project()` call.
- `release.yml` now extracts the release version from `VERSION` instead of grepping
  `CMakeLists.txt`, and the release-trigger check and path filter both watch `VERSION` instead of
  `CMakeLists.txt`.
- Renamed the project/binary/package identity from `alg-welcome` to `archer` (internal codename,
  mirrors `alg-installer` → *Challenger*). Affects: CMake project/target name, the
  `ARCHER_VERSION` compile definition (was `ALG_WELCOME_VERSION`), the desktop file
  (`alg-welcome.desktop` → `archer.desktop`), the app icon (`welcome.png` → `archer.png`), install
  destinations (`share/alg-welcome` → `share/archer`), the autostart source/target desktop-file
  paths, `app.setDesktopFileName()`, and all CI/release artifact names and install instructions.
  This is a full rebrand, not just an internal codename: the window title, the About Us dialog's
  app-name label, and the desktop entry `Name=` are all now `Archer` (desktop `Comment=ALG Welcome
  App`). The in-window header text ("Welcome to ALG!") is unchanged.
