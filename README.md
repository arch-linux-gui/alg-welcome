# ALG Welcome

Welcome to **ALG Welcome** – a fast, lightweight welcome application for ALG built with **Qt6 and modern C++**. This application simplifies your ALG experience by offering an intuitive graphical interface with features designed to help you get started and maintain your system with confidence.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Authors](#authors)
- [Contact](#contact)

## Features

- **Install ALG:** Launch the Calamares installer for system installation (Live ISO only).
- **System Updates:** Keep your system current with the built-in Pacman updater.
- **Mirrorlist Updater:** Refresh your Arch Linux mirrorlist using reflector to ensure optimal package download speeds.
- **Screen Resolution:** Adjust and manage your screen resolution settings directly from the application.
- **Social Media Links:** Quick access to GitHub and Discord communities.
- **AutoStart Management:** Enable or disable automatic startup of the welcome application on login.
- **Theme Management:** Toggle between light and dark themes for KDE Plasma, GNOME, and Xfce desktop environments.
- **Modern UI:** Enjoy a fast, native interface built with Qt6 and optimized C++17 with LTO (Link Time Optimization).

## Installation

### Prerequisites

- **C++ Compiler:** GCC 7+ or Clang 5+ with C++17 support
- **CMake:** Version 3.16 or later
- **Qt6:** Qt6 Core, Widgets, and Gui modules
- **Build Tools:** Make or Ninja build system
- **Operating System:** Arch Linux or Arch-based distributions (ALG, Manjaro, EndeavourOS, etc.)

### Dependencies

On Arch Linux, install the required dependencies:

```bash
sudo pacman -S qt6-base cmake make gcc
```

### Building from Source

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/arch-linux-gui/alg-welcome.git
   cd alg-welcome
   ```

2. **Build the Application:**
   ```bash
   ./build.sh
   ```

3. **Run the Application:**
   ```bash
   ./alg-welcome
   ```

4. **Install System-Wide (Optional):**
   ```bash
   sudo make install
   ```

   This installs:
   - Binary to `/usr/local/bin/alg-welcome`
   - Desktop file to `/usr/local/share/applications/welcome.desktop`
   - Icon to `/usr/local/share/pixmaps/welcome.png`
   - Stylesheet to `/usr/local/share/alg-welcome/styles.qss`

## Usage

When you launch **ALG Welcome**, you'll be greeted by a modern Qt interface that provides:

- **Install & Setup:** Install the ALG system (Live ISO only) or adjust screen resolution settings.
- **Basic Utilities:** Update your system packages with Pacman or refresh the mirrorlist using reflector for faster downloads.
- **Social Media Links:** Quick access to the GitHub repository and Discord community.
- **Get Started:** Toggle autostart functionality and switch between light/dark themes for your desktop environment.
- **About Us:** Learn more about the Arka Linux GUI project and its developers.

### Command-Line Options

```bash
alg-welcome [options]

Options:
  --version    Display version information
  --debug      Enable debug logging output
  -h, --help   Show help information
```

## License

This project is distributed under the MIT License. See the [LICENSE](LICENSE) file for complete details.

## Authors

- **DemonKiller** – Core Team Developer
- **Akash6222** – Core Team Developer
- **harshau007** – Core Team Developer

## Contact

For support or inquiries, please visit our [website](https://www.arkalinuxgui.org) or open an issue on our GitHub repository.

---

*Arka Linux GUI: Simplifying the Arch Linux Experience, One Click at a Time.*