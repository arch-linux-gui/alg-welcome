# ALG Welcome

Welcome to **ALG Welcome** – a fast, intuitive welcome application for Arch Linux built using PySide6 (Qt for Python). Our application simplifies your Arch Linux experience by offering an easy-to-use graphical interface with features designed to help you get started and maintain your system with confidence.

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
- **System Updates:** Keep your system current using our built-in updater.
- **Mirrorlist Updater:** Refresh your Arch Linux mirrorlist quickly to ensure efficient package downloads.
- **Screen Resolution:** Adjust and manage your screen resolution settings directly from the application.
- **Social Media Links:** Quick access to GitHub and Discord communities.
- **AutoStart Management:** Enable or disable automatic startup of the welcome application.
- **Theme Management:** Toggle between light and dark themes for supported desktop environments.
- **Modern UI:** Enjoy an intuitive and responsive interface built with PySide6 and Qt.

## Installation

### Prerequisites

- **Python:** Version 3.8 or later is recommended.
- **PySide6:** Qt for Python bindings. On Arch Linux, install via pip:
  ```bash
  sudo pacman -S pyside6
  ```
  On non-Qt DEs like GNOME, you may need to install the Qt6 package.

- **Operating System:** Made for ALG. Should work on Arch Linux, and it's derivative distributions.

### Installation

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/arch-linux-gui/alg-welcome.git
   cd alg-welcome
   ```

2. **Install Dependencies:**
   ```bash
   sudo pacman -S pyside6
   ```

3. **Run the Application:**
   ```bash
   python main.py
   ```

## Usage

When you launch **ALG Welcome**, you'll be greeted by a modern interface that provides:

- **Install & Setup:** Install ALG system or adjust screen resolution settings.
- **Basic Utilities:** Update your system packages or refresh the mirrorlist for faster downloads.
- **Social Media Links:** Quick access to GitHub repository and Discord community.
- **Get Started:** Toggle autostart functionality and switch between light/dark themes.
- **About Us:** Learn more about the Arka Linux GUI project and its developers.

## Contributing

Contributions are welcome and appreciated! To contribute:

1. **Fork the Repository.**
2. **Create a New Branch** for your feature or bug fix:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Commit Your Changes** and push your branch:
   ```bash
   git commit -m "Add new feature or fix bug"
   git push -u origin feature/your-feature-name
   ```
4. **Open a Pull Request** describing your changes.

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