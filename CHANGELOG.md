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
