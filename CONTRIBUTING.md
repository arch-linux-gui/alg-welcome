# Contributing to Archer

Contributions are welcome and appreciated! To contribute:

1. **Fork the Repository.**
2. **Create a New Branch** for your feature or bug fix:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make Your Changes:**
   - Follow modern C++17 best practices
   - Use Qt6 APIs and conventions
   - Ensure code compiles without warnings
   - Test on your desktop environment (KDE, GNOME, or Xfce)
4. **Commit Your Changes** and push your branch:
   ```bash
   git commit -m "Add new feature or fix bug"
   git push -u origin feature/your-feature-name
   ```
5. **Open a Pull Request** describing your changes.

## Project Structure

```
archer/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── WelcomeWindow.cpp/h   # Main window implementation
│   ├── dialogs/              # Dialog implementations
│   │   ├── AboutUsDialog.cpp/h
│   │   └── MirrorListDialog.cpp/h
│   └── utils/                # Utility modules
│       ├── Autostart.cpp/h   # Autostart management
│       ├── Extras.cpp/h      # System utilities
│       ├── Logger.h          # Logging utilities
│       ├── Resolution.cpp/h  # Screen resolution
│       ├── Themes.cpp/h      # Theme management
│       └── Updates.cpp/h     # System updates
├── assets/                   # Application assets
├── build/                    # Build output directory
├── CMakeLists.txt           # CMake build configuration
└── styles.qss               # Qt stylesheet
```

## Technical Details

- **Language:** Modern C++17
- **GUI Framework:** Qt6 (Core, Widgets, Gui)
- **Build System:** CMake 3.16+
- **Threading:** std::thread with Qt signals/slots for thread-safe UI updates
- **Optimization:** Link Time Optimization (LTO) enabled for Release builds
- **Binary Size:** ~188 KB (unstripped), ~133 KB (stripped) with LTO
- **Desktop Environments:** Full support for KDE Plasma, GNOME, and Xfce theme switching