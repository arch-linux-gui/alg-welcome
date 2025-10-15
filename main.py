#!/usr/bin/env python3
"""
ALG Welcome Application - PySide6 Qt Version
A modern desktop application for Arch Linux GUI (ALG) system management.
"""

import sys
import os
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
    QGroupBox, QPushButton, QLabel, QCheckBox, QMessageBox, QFrame
)
from PySide6.QtCore import Qt, QUrl, Signal
from PySide6.QtGui import QPixmap, QIcon, QDesktopServices, QFont

from utils.system_utils import SystemUtils
from utils.theme_manager import ThemeManager
from utils.autostart_manager import AutostartManager


class WelcomeWindow(QMainWindow):
    """Main application window for ALG Welcome."""
    
    def __init__(self):
        super().__init__()
        self.system_utils = SystemUtils()
        self.theme_manager = ThemeManager()
        self.autostart_manager = AutostartManager()
        
        self.init_ui()
        self.apply_styles()
        self.connect_signals()
        
    def init_ui(self):
        """Initialize the user interface."""
        self.setWindowTitle("Welcome")
        self.setFixedSize(500, 400)
        
        # Set window icon
        icon_path = os.path.join(os.path.dirname(__file__), "assets", "alg-logo.png")
        if os.path.exists(icon_path):
            self.setWindowIcon(QIcon(icon_path))
        
        # Create central widget and main layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setSpacing(15)
        main_layout.setContentsMargins(20, 10, 20, 10)
        
        # Add header section
        self.add_header(main_layout)
        
        # Add Install & Setup section
        self.add_install_setup_section(main_layout)
        
        # Add Basic Utilities section
        self.add_basic_utilities_section(main_layout)
        
        # Add Social Media Links section
        self.add_social_media_section(main_layout)
        
        # Add Get Started section
        self.add_get_started_section(main_layout)
        
        # Add About Us section
        self.add_about_us_section(main_layout)
        
    def add_header(self, layout):
        """Add the header with logo and title."""
        header_frame = QFrame()
        header_layout = QHBoxLayout(header_frame)
        header_layout.setContentsMargins(0, 0, 0, 0)
        
        # Logo
        logo_path = os.path.join(os.path.dirname(__file__), "assets", "alg-logo.png")
        if os.path.exists(logo_path):
            logo_label = QLabel()
            pixmap = QPixmap(logo_path)
            scaled_pixmap = pixmap.scaled(60, 60, Qt.KeepAspectRatio, Qt.SmoothTransformation)
            logo_label.setPixmap(scaled_pixmap)
            header_layout.addWidget(logo_label)
        
        # Title
        title_label = QLabel("Welcome")
        title_label.setObjectName("header")
        title_label.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        header_layout.addWidget(title_label)
        
        layout.addWidget(header_frame)
        
    def add_install_setup_section(self, layout):
        """Add Install & Setup section with buttons."""
        group_box = QGroupBox("Install & Setup")
        group_layout = QVBoxLayout(group_box)
        
        # Create horizontal layout for buttons
        buttons_layout = QHBoxLayout()
        
        # Install ALG button
        install_btn = QPushButton("Install ALG")
        install_btn.setObjectName("actionButton")
        install_btn.clicked.connect(self.on_install_alg_clicked)
        buttons_layout.addWidget(install_btn)
        
        # Screen Resolution button
        resolution_btn = QPushButton("Screen Resolution")
        resolution_btn.setObjectName("actionButton")
        resolution_btn.clicked.connect(self.on_screen_resolution_clicked)
        buttons_layout.addWidget(resolution_btn)
        
        group_layout.addLayout(buttons_layout)
        layout.addWidget(group_box)
        
    def add_basic_utilities_section(self, layout):
        """Add Basic Utilities section with buttons."""
        group_box = QGroupBox("Basic Utilities")
        group_layout = QVBoxLayout(group_box)
        
        # Create horizontal layout for buttons
        buttons_layout = QHBoxLayout()
        
        # Update System button
        update_btn = QPushButton("Update System")
        update_btn.setObjectName("actionButton")
        update_btn.clicked.connect(self.on_update_system_clicked)
        buttons_layout.addWidget(update_btn)
        
        # Update Mirrorlist button
        mirrorlist_btn = QPushButton("Update Mirrorlist")
        mirrorlist_btn.setObjectName("actionButton")
        mirrorlist_btn.clicked.connect(self.on_update_mirrorlist_clicked)
        buttons_layout.addWidget(mirrorlist_btn)
        
        group_layout.addLayout(buttons_layout)
        layout.addWidget(group_box)
        
    def add_social_media_section(self, layout):
        """Add Social Media Links section with buttons."""
        group_box = QGroupBox("Social Media Links")
        group_layout = QVBoxLayout(group_box)
        
        # Create horizontal layout for buttons
        buttons_layout = QHBoxLayout()
        
        # GitHub button with external link icon
        github_btn = QPushButton("GitHub ↗")
        github_btn.setObjectName("socialButton")
        github_btn.clicked.connect(self.on_github_clicked)
        buttons_layout.addWidget(github_btn)
        
        # Discord button with external link icon
        discord_btn = QPushButton("Discord ↗")
        discord_btn.setObjectName("socialButton")
        discord_btn.clicked.connect(self.on_discord_clicked)
        buttons_layout.addWidget(discord_btn)
        
        group_layout.addLayout(buttons_layout)
        layout.addWidget(group_box)
        
    def add_get_started_section(self, layout):
        """Add Get Started section with toggle switches."""
        group_box = QGroupBox("Get Started")
        group_layout = QVBoxLayout(group_box)
        
        # Create horizontal layout for toggles
        toggles_layout = QHBoxLayout()
        
        # AutoStart toggle
        self.autostart_checkbox = QCheckBox("AutoStart :")
        self.autostart_checkbox.setObjectName("toggleCheckbox")
        # Set initial state based on current autostart status
        self.autostart_checkbox.setChecked(self.autostart_manager.is_autostart_enabled())
        self.autostart_checkbox.stateChanged.connect(self.on_autostart_toggled)
        toggles_layout.addWidget(self.autostart_checkbox)
        
        # Dark Theme toggle
        self.dark_theme_checkbox = QCheckBox("Dark Theme :")
        self.dark_theme_checkbox.setObjectName("toggleCheckbox")
        # Set initial state based on current theme
        self.dark_theme_checkbox.setChecked(self.theme_manager.is_dark_theme())
        self.dark_theme_checkbox.stateChanged.connect(self.on_dark_theme_toggled)
        toggles_layout.addWidget(self.dark_theme_checkbox)
        
        group_layout.addLayout(toggles_layout)
        layout.addWidget(group_box)
        
    def add_about_us_section(self, layout):
        """Add About Us section with large button."""
        about_btn = QPushButton("About Us")
        about_btn.setObjectName("aboutButton")
        about_btn.clicked.connect(self.on_about_us_clicked)
        layout.addWidget(about_btn)
        
    def apply_styles(self):
        """Apply modern CSS styling to the application."""
        # Load external QSS stylesheet
        stylesheet_path = os.path.join(os.path.dirname(__file__), "styles.qss")
        try:
            with open(stylesheet_path, 'r') as f:
                style = f.read()
            self.setStyleSheet(style)
        except FileNotFoundError:
            print(f"Stylesheet not found: {stylesheet_path}")
            # Fallback to basic styling
            self.setStyleSheet("""
                QMainWindow {
                    background-color: #2b2b2b;
                    color: white;
                }
                QPushButton {
                    background-color: #404040;
                    color: white;
                    border: none;
                    border-radius: 6px;
                    padding: 8px 16px;
                }
                QPushButton:hover {
                    background-color: #505050;
                }
            """)
        
    def connect_signals(self):
        """Connect all signal handlers."""
        pass  # Signals are connected in individual methods
        
    # Button click handlers
    def on_install_alg_clicked(self):
        """Handle Install ALG button click."""
        print("Install ALG clicked")
        self.system_utils.run_calamares_if_live_iso()
        
    def on_screen_resolution_clicked(self):
        """Handle Screen Resolution button click."""
        print("Screen Resolution clicked")
        self.system_utils.open_screen_resolution_settings()
        
    def on_update_system_clicked(self):
        """Handle Update System button click."""
        print("Update System clicked")
        self.system_utils.update_system()
        
    def on_update_mirrorlist_clicked(self):
        """Handle Update Mirrorlist button click."""
        print("Update Mirrorlist clicked")
        # TODO: Implement mirrorlist update dialog
        QMessageBox.information(self, "Info", "Mirrorlist update functionality will be implemented.")
        
    def on_github_clicked(self):
        """Handle GitHub button click."""
        print("GitHub clicked")
        QDesktopServices.openUrl(QUrl("https://github.com/arch-linux-gui"))
        
    def on_discord_clicked(self):
        """Handle Discord button click."""
        print("Discord clicked")
        QDesktopServices.openUrl(QUrl("https://discord.gg/NgAFEw9Tkf"))
        
    def on_autostart_toggled(self, state):
        """Handle AutoStart toggle state change."""
        is_enabled = state == Qt.Checked
        print(f"AutoStart toggled: {is_enabled}")
        self.autostart_manager.toggle_autostart(is_enabled)
        
    def on_dark_theme_toggled(self, state):
        """Handle Dark Theme toggle state change."""
        is_dark = state == Qt.Checked
        print(f"Dark Theme toggled: {is_dark}")
        self.theme_manager.toggle_theme(is_dark)
        
    def on_about_us_clicked(self):
        """Handle About Us button click."""
        print("About Us clicked")
        self.show_about_dialog()
        
    def show_about_dialog(self):
        """Show the About Us dialog."""
        msg = QMessageBox(self)
        msg.setWindowTitle("About Us")
        msg.setText("""
        <h2>Arka Linux GUI</h2>
        <p><b>Version 1.0.0</b></p>
        <p>Welcome to Arka Linux GUI, formerly Arch Linux GUI. We simplify Arch Linux installation with a fast, offline graphical installer. Our mission is to make Arch accessible to both beginners and power users.</p>
        <p><b>Developers:</b></p>
        <ul>
        <li>DemonKiller (Core Team)</li>
        <li>Akash6222 (Core Team)</li>
        <li>harshau007 (Core Team)</li>
        </ul>
        <p><a href="https://www.arkalinuxgui.org">Visit our website</a></p>
        <p>Distributed under the MIT License.</p>
        """)
        msg.setTextFormat(Qt.RichText)
        msg.exec()


def main():
    """Main application entry point."""
    app = QApplication(sys.argv)
    
    # Set application properties
    app.setApplicationName("ALG Welcome")
    app.setApplicationVersion("1.0.0")
    app.setOrganizationName("Arka Linux GUI")
    
    # Apply Fusion theme for modern look
    app.setStyle('Fusion')
    
    # Create and show main window
    window = WelcomeWindow()
    window.show()
    
    # Start event loop
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
