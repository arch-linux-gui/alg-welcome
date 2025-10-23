#!/usr/bin/env python3
"""
Welcome to ALG - PySide6 Application
Main application entry point
"""

import sys
from pathlib import Path
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QPushButton, QGridLayout, QFrame, QSizePolicy
)
from PySide6.QtCore import Qt, QSize
from PySide6.QtGui import QPixmap, QIcon

from utils.extras import get_desktop_environment, check_if_live_iso, open_url
from utils.autostart import toggle_autostart, check_file_exists
from utils.themes import toggle_theme, current_theme, is_dark_theme
from utils.resolution import screen_resolution
from utils.updates import update_system
from utils.mirrorlist import MirrorListDialog
from utils.aboutus import AboutUsDialog


class WelcomeWindow(QMainWindow):
    """Main Welcome Application Window"""
    
    WINDOW_TITLE = "ALG Welcome"
    WINDOW_WIDTH = 480
    WINDOW_HEIGHT = 400
    LOGO_SIZE = 60
    ICON_SIZE = 20
    
    def __init__(self):
        super().__init__()

        # Setup window logo
        self.setWindowTitle(self.WINDOW_TITLE)
        self.resize(self.WINDOW_WIDTH, self.WINDOW_HEIGHT)
        base_path = Path(__file__).resolve().parent
        icon_path = base_path / "assets" / "welcome.png"
        self.setWindowIcon(QIcon(str(icon_path)))
        
        # Get system information
        self.desktop_env = get_desktop_environment()
        self.is_live_iso = check_if_live_iso()
        
        # Setup UI
        self.setup_window()
        self.apply_stylesheet()
        self.setup_ui()
        
    def setup_window(self):
        """Configure main window properties"""
        self.setWindowTitle(self.WINDOW_TITLE)
        self.setFixedSize(self.WINDOW_WIDTH, self.WINDOW_HEIGHT)
        
        # Center window on screen
        screen = QApplication.primaryScreen().geometry()
        x = (screen.width() - self.WINDOW_WIDTH) // 2
        y = (screen.height() - self.WINDOW_HEIGHT) // 2
        self.move(x, y)
        
    def apply_stylesheet(self):
        """Load and apply Qt stylesheet"""
        qss_path = Path(__file__).parent / "styles.qss"
        if qss_path.exists():
            with open(qss_path, 'r') as f:
                self.setStyleSheet(f.read())
                
    def setup_ui(self):
        """Setup the main UI layout"""
        # Central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Main vertical layout
        main_layout = QVBoxLayout(central_widget)
        main_layout.setSpacing(10)
        main_layout.setContentsMargins(20, 10, 20, 10)
        
        # Add sections
        self.add_header(main_layout)
        self.add_install_setup_section(main_layout)
        self.add_social_media_section(main_layout)
        self.add_more_options_section(main_layout)
        self.add_about_us_section(main_layout)
        
    def add_header(self, layout):
        """Add header with logo and welcome text"""
        header_layout = QHBoxLayout()
        header_layout.setSpacing(10)
        
        # Logo
        base_path = Path(__file__).resolve().parent
        logo_path = base_path / "assets" / "welcome.png"
        if Path(logo_path).exists():
            logo_label = QLabel()
            pixmap = QPixmap(logo_path)
            scaled_pixmap = pixmap.scaled(
                self.LOGO_SIZE, self.LOGO_SIZE,
                Qt.KeepAspectRatio, Qt.SmoothTransformation
            )
            logo_label.setPixmap(scaled_pixmap)
            header_layout.addWidget(logo_label)
        
        # Welcome text
        welcome_label = QLabel("Welcome to ALG!")
        welcome_label.setObjectName("header")
        welcome_label.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        header_layout.addWidget(welcome_label, 1)
        
        layout.addLayout(header_layout)
        
    def add_install_setup_section(self, layout):
        """Add Install & Setup section with buttons"""
        section_label = QLabel("Install & Setup")
        section_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(section_label)
        
        # Create grid for buttons
        grid = QGridLayout()
        grid.setSpacing(10)
        
        # Define buttons based on whether it's Live ISO
        if self.is_live_iso:
            buttons = [
                ("Install ALG ", "system-software-install", self.on_install_alg),
                ("Screen Resolution ", "video-display", self.on_screen_resolution),
                ("Update System ", "system-software-update", self.on_update_system),
                ("Update Mirrorlist ", "view-refresh", self.on_update_mirrorlist),
            ]
        else:
            buttons = [
                ("Tutorials ", "help-contents", self.on_tutorials),
                ("Screen Resolution ", "video-display", self.on_screen_resolution),
                ("Update System ", "system-software-update", self.on_update_system),
                ("Update Mirrorlist ", "view-refresh", self.on_update_mirrorlist),
            ]
        
        # Add buttons to grid (2 columns)
        for i, (label, icon, callback) in enumerate(buttons):
            button = self.create_button_with_icon(label, icon, False)
            button.clicked.connect(callback)
            grid.addWidget(button, i // 2, i % 2)
        
        layout.addLayout(grid)
        layout.addStretch()
        
    def add_social_media_section(self, layout):
        """Add Social Media Links section"""
        section_label = QLabel("Social Media Links")
        section_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(section_label)
        
        # Create grid for social buttons
        grid = QGridLayout()
        grid.setSpacing(10)
        
        buttons = [
            ("GitHub  ", "assets/github.svg", lambda: open_url("https://github.com/arch-linux-gui")),
            ("Discord ", "assets/discord.svg", lambda: open_url("https://discord.gg/NgAFEw9Tkf")),
        ]
        
        for i, (label, icon_path, callback) in enumerate(buttons):
            button = self.create_button_with_icon(label, icon_path, True)
            button.clicked.connect(callback)
            grid.addWidget(button, 0, i)
        
        layout.addLayout(grid)
        
    def add_more_options_section(self, layout):
        """Add More Options section with toggles"""
        section_label = QLabel("More Options")
        section_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(section_label)
        
        hbox = QHBoxLayout()
        hbox.setSpacing(10)
        
        # Autostart toggle
        autostart_widget, self.autostart_switch = self.create_switch_with_label(
            "AutoStart:", self.on_autostart_toggled
        )
        self.autostart_switch.setChecked(check_file_exists())
        hbox.addWidget(autostart_widget)
        
        # Dark theme toggle
        darktheme_widget, self.theme_switch = self.create_switch_with_label(
            "Dark Theme:", self.on_theme_toggled
        )
        curr_theme = current_theme(self.desktop_env)
        self.theme_switch.setChecked(is_dark_theme(curr_theme))
        hbox.addWidget(darktheme_widget)
        
        layout.addLayout(hbox)
        
    def add_about_us_section(self, layout):
        """Add About Us button"""
        about_button = QPushButton("About Us")
        about_button.setFocusPolicy(Qt.NoFocus)
        about_button.clicked.connect(self.on_about_us)
        layout.addWidget(about_button)
        
    def create_button_with_icon(self, label, icon_name, from_file):
        """Create a button with icon and label"""
        button = QPushButton(label)
        button.setFocusPolicy(Qt.NoFocus)
        
        # Set icon
        if from_file:
            icon_path = Path(__file__).parent / icon_name
            if icon_path.exists():
                button.setIcon(QIcon(str(icon_path)))
                button.setIconSize(QSize(self.ICON_SIZE, self.ICON_SIZE))
        else:
            # Use system theme icon
            button.setIcon(QIcon.fromTheme(icon_name))
            button.setIconSize(QSize(self.ICON_SIZE, self.ICON_SIZE))
        
        button.setLayoutDirection(Qt.RightToLeft)  # Icon on right
        return button
        
    def create_switch_with_label(self, label_text, callback):
        """Create a switch (checkbox) with label"""
        from PySide6.QtWidgets import QCheckBox
        
        widget = QWidget()
        layout = QHBoxLayout(widget)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(5)
        
        label = QLabel(label_text)
        switch = QCheckBox()
        switch.setFocusPolicy(Qt.NoFocus)
        switch.toggled.connect(callback)
        
        layout.addWidget(label)
        layout.addStretch()
        layout.addWidget(switch)
        
        return widget, switch
        
    # Callback methods
    def on_install_alg(self):
        """Handle Install ALG button click"""
        from utils.extras import run_calamares_if_live_iso
        run_calamares_if_live_iso(self.is_live_iso)
        
    def on_screen_resolution(self):
        """Handle Screen Resolution button click"""
        screen_resolution(self.desktop_env)
        
    def on_update_system(self):
        """Handle Update System button click"""
        update_system(self.desktop_env)
        
    def on_update_mirrorlist(self):
        """Handle Update Mirrorlist button click"""
        dialog = MirrorListDialog(self)
        dialog.exec()
        
    def on_tutorials(self):
        """Handle Tutorials button click"""
        open_url("https://arkalinuxgui.org/tutorials")
        
    def on_autostart_toggled(self, checked):
        """Handle autostart toggle"""
        toggle_autostart(checked)
        
    def on_theme_toggled(self, checked):
        """Handle theme toggle"""
        toggle_theme(checked, self.desktop_env)
        
    def on_about_us(self):
        """Handle About Us button click"""
        dialog = AboutUsDialog(self)
        dialog.exec()


def main():
    """Main application entry point"""
    app = QApplication(sys.argv)
    app.setApplicationName("Welcome to ALG")
    
    window = WelcomeWindow()
    window.show()
    
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
