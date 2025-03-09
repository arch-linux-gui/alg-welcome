# ALG Welcome

Welcome to **ALG Welcome** – a fast, intuitive welcome application for Arch Linux built using GTK 3.0 and GoTK. Our application simplifies your Arch Linux experience by offering an easy-to-use graphical interface with features designed to help you get started and maintain your system with confidence.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Authors](#authors)
- [Contact](#contact)

## Features

- **Tutorials:** Access step-by-step guides to help you set up and navigate the Arch Linux ecosystem.
- **System Updates:** Keep your system current using our built-in updater.
- **Mirrorlist Updater:** Refresh your Arch Linux mirrorlist quickly to ensure efficient package downloads.
- **Screen Resolution:** Adjust and manage your screen resolution settings directly from the application.
- **User-Friendly GUI:** Enjoy an intuitive and responsive interface built with GTK 3.0.
- **Offline Functionality:** Designed to work even in offline scenarios, allowing you to perform essential tasks without an internet connection.

## Installation

### Prerequisites

- **Operating System:** Arch Linux or an Arch-based distribution.
- **GTK 3.0:** Ensure GTK 3.0 is installed. You can install it via:
  ```bash
  sudo pacman -S gtk3
  ```
- **Go:** Version 1.23 or later is recommended. You can download Go from [golang.org](https://golang.org/dl/).
- **Additional Dependencies:** Other build tools and libraries might be required. Check your package manager's documentation if needed.

### Building from Source

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/arch-linux-gui/alg-welcome.git
   cd alg-welcome
   ```

2. **Install Dependencies:**
   For Arch Linux, you can install GTK 3.0 using:
   ```bash
   sudo pacman -S gtk3
   ```

3. **Build the Application:**
   ```bash
   go build -o welcome .
   ```

4. **Run the Application:**
   ```bash
   ./welcome
   ```

## Usage

When you launch **ALG Welcome**, you'll be greeted by a welcoming interface that provides:

- **Tutorials:** Learn how to install, configure, and use Arch Linux with our comprehensive guides.
- **System Updater:** Easily update your system to the latest packages and improvements.
- **Mirrorlist Management:** Update your mirrorlist to optimize package download speeds.
- **Screen Resolution Control:** Adjust display settings to suit your hardware and preferences.
- **Additional Tools:** Explore other features and options designed to enhance your overall experience.

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