"""
About Us dialog
"""

from PySide6.QtWidgets import (
    QDialog, QVBoxLayout, QLabel, QPushButton
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QDesktopServices, QFont
from PySide6.QtCore import QUrl


class AboutUsDialog(QDialog):
    """About Us dialog showing application information"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setup_ui()
        
    def setup_ui(self):
        """Setup the About Us dialog UI"""
        self.setWindowTitle("About Us")
        self.setFixedSize(400, 350)
        self.setModal(True)
        
        layout = QVBoxLayout(self)
        layout.setSpacing(10)
        layout.setContentsMargins(20, 20, 20, 20)
        
        # App name
        app_name = QLabel("Arka Linux GUI")
        font = QFont()
        font.setPointSize(18)
        font.setBold(True)
        app_name.setFont(font)
        app_name.setAlignment(Qt.AlignCenter)
        layout.addWidget(app_name)
        
        # Version
        version = QLabel("Version 2.1.5")
        version.setAlignment(Qt.AlignCenter)
        layout.addWidget(version)
        
        # Description
        description = QLabel(
            "ALG Welcome was made to help you onboard quickly"
        )
        description.setWordWrap(True)
        description.setAlignment(Qt.AlignJustify)
        layout.addWidget(description)
        
        # Developers label
        dev_label = QLabel("<b>Developers:</b>")
        dev_label.setAlignment(Qt.AlignLeft)
        layout.addWidget(dev_label)
        
        # Developer names
        dev_names = QLabel(
            "• DemonKiller (Core Team)\n"
            "• Akash6222 (Core Team)\n"
            "• harshau007 (Core Team)"
        )
        dev_names.setAlignment(Qt.AlignLeft)
        layout.addWidget(dev_names)
        
        # Website button
        website_button = QPushButton("Visit our website")
        website_button.setFocusPolicy(Qt.NoFocus)
        website_button.clicked.connect(
            lambda: QDesktopServices.openUrl(QUrl("https://www.arkalinuxgui.org"))
        )
        layout.addWidget(website_button)
        
        # License
        license_text = QLabel("Distributed under the MIT License.")
        license_text.setWordWrap(True)
        license_text.setAlignment(Qt.AlignCenter)
        layout.addWidget(license_text)
        
        layout.addStretch()