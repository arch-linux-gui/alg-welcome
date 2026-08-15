# Archer (ALG Welcome App)

**Archer** is a fast, lightweight welcome/onboarding application for ALG, built with Qt6 and
modern C++. It launches the Calamares installer on the live ISO, launches the ALG app store post
install, and provides a few onboarding utilities (mirrorlist refresh, screen resolution, system
updates, autostart, light/dark theme toggling).

## Prerequisites

- **C++ Compiler:** GCC 7+ or Clang 5+ with C++17 support
- **CMake:** 3.16+
- **Qt6:** Core, Widgets, Gui modules
- **spdlog** and **fmt:** logging
- **Catch2** (v3): test suite only, needed with the default `-DBUILD_TESTS=ON` (falls back to
  CMake `FetchContent` if not installed locally)

On Arch Linux:

```bash
sudo pacman -S qt6-base spdlog fmt catch2 cmake make gcc
```

## Building

```bash
git clone https://github.com/arch-linux-gui/alg-welcome.git
cd alg-welcome
./build.sh              # or: cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j$(nproc)
./build/archer
```

Install system-wide:

```bash
sudo cmake --install build
```

This installs the binary to `bin/archer`, the desktop file to `share/applications/archer.desktop`,
the icon to `share/pixmaps/archer.png`, and the stylesheet to `share/archer/styles.qss`, relative
to the install prefix (`/usr/local` by default).

## Testing

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Pass `-DBUILD_TESTS=OFF` to `cmake -B build` to skip building the test suite entirely.

## Command-Line Options

```bash
archer [options]

Options:
  --version             Display version information
  -h, --help            Show help information
  -n, --no-autostart    Don't check autostart status on launch
  -d, --debug           Enable debug-level logging (equivalent to -v)
  -v, --verbose         Increase log verbosity (-v for debug, -vv for trace)
  --log-level <level>   Set the log level explicitly (trace, debug, info,
                         warn, error, critical, off)
```

Logs are written to stderr and to a log file — `/var/log/archer/archer.log` if writable, falling
back to a temp location (e.g. `/tmp/archer.log`) otherwise. The chosen path is printed at debug
level on startup.

## License

MIT — see [LICENSE](LICENSE).

## Authors

DemonKiller, Akash6222, harshau007 — ALG Core Team. Website: https://www.arkalinuxgui.org
