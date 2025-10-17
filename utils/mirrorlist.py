"""
Mirror list update dialog with reflector integration
"""

import subprocess
import re
import threading
from PySide6.QtWidgets import (
    QDialog, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QCheckBox, QComboBox, QSpinBox, QGroupBox, QGridLayout,
    QTreeWidget, QTreeWidgetItem, QHeaderView, QScrollArea, QWidget
)
from PySide6.QtCore import Qt, Signal, QObject


class MirrorListSignals(QObject):
    """Signals for thread-safe UI updates"""
    log_appended = Signal(str, str, str)
    update_finished = Signal()


class MirrorListDialog(QDialog):
    """Dialog for updating Arch Linux mirror list using reflector"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.selected_countries = []
        self.is_updating = False
        self.line_counter = 0
        self.log_dialog = None
        
        # Signals for thread communication
        self.signals = MirrorListSignals()
        self.signals.log_appended.connect(self._append_log_to_ui)
        self.signals.update_finished.connect(self._on_update_finished)
        
        self.setup_ui()
        
    def setup_ui(self):
        """Setup the mirror list dialog UI"""
        self.setWindowTitle("Update MirrorList")
        self.setMinimumSize(500, 450)
        self.setModal(True)
        
        layout = QVBoxLayout(self)
        layout.setSpacing(10)
        layout.setContentsMargins(10, 10, 10, 10)
        
        # Countries selection
        self.setup_countries_section(layout)
        
        # Protocol selection
        self.setup_protocol_section(layout)
        
        # Sort by selection
        self.setup_sort_section(layout)
        
        # Settings (max mirrors and timeout)
        self.setup_settings_section(layout)
        
        # Update button
        self.update_button = QPushButton("Update")
        self.update_button.setEnabled(False)
        self.update_button.clicked.connect(self.on_update_clicked)
        layout.addWidget(self.update_button)
        
    def setup_countries_section(self, layout):
        """Setup countries selection section"""
        group = QGroupBox("Countries")
        grid = QGridLayout()
        grid.setSpacing(10)
        
        # countries = [
        #     "United States", "United Kingdom", "Brazil", "Japan", "Sweden", "France",
        #     "Canada", "India", "Australia", "China", "Germany", "Netherlands", "Russia",
        #     "Russia"
        # ]

        countries = [
            "Australia", "Brazil", "Canada", "China", "France", "Germany", "India",
            "Japan", "Netherlands", "Russia", "Sweden", "United Kingdom", "United States", "Worldwide"
        ]
        
        self.country_checkboxes = {}
        for i, country in enumerate(countries):
            checkbox = QCheckBox(country)
            checkbox.setFocusPolicy(Qt.NoFocus)
            checkbox.toggled.connect(self.on_country_toggled)
            self.country_checkboxes[country] = checkbox
            grid.addWidget(checkbox, i // 2, i % 2)
        
        group.setLayout(grid)
        layout.addWidget(group)
        
    def setup_protocol_section(self, layout):
        """Setup protocol selection section"""
        group = QGroupBox("Protocols")
        hbox = QHBoxLayout()
        
        self.https_check = QCheckBox("HTTPS")
        self.https_check.setChecked(True)
        self.https_check.setFocusPolicy(Qt.NoFocus)
        
        self.http_check = QCheckBox("HTTP")
        self.http_check.setFocusPolicy(Qt.NoFocus)
        
        hbox.addWidget(self.https_check)
        hbox.addWidget(self.http_check)
        hbox.addStretch()
        
        group.setLayout(hbox)
        layout.addWidget(group)
        
    def setup_sort_section(self, layout):
        """Setup sort by section"""
        group = QGroupBox("Sort By")
        hbox = QHBoxLayout()
        
        self.sort_combo = QComboBox()
        self.sort_combo.addItems(["Rate", "Age", "Score", "Delay", "Country"])
        self.sort_combo.setCurrentIndex(0)
        
        hbox.addWidget(self.sort_combo)
        group.setLayout(hbox)
        layout.addWidget(group)
        
    def setup_settings_section(self, layout):
        """Setup settings section (max mirrors and timeout)"""
        hbox = QHBoxLayout()
        
        # Max fresh mirrors
        mirror_label = QLabel("Max Fresh Mirrors:")
        self.mirror_spin = QSpinBox()
        self.mirror_spin.setRange(1, 20)
        self.mirror_spin.setValue(5)
        
        hbox.addWidget(mirror_label)
        hbox.addWidget(self.mirror_spin)
        
        # Timeout
        timeout_label = QLabel("Timeout (s):")
        self.timeout_spin = QSpinBox()
        self.timeout_spin.setRange(5, 60)
        self.timeout_spin.setValue(10)
        
        hbox.addWidget(timeout_label)
        hbox.addWidget(self.timeout_spin)
        hbox.addStretch()
        
        layout.addLayout(hbox)
        
    def on_country_toggled(self, checked):
        """Handle country checkbox toggle"""
        sender = self.sender()
        if sender:
            country = sender.text()
            if checked:
                if country not in self.selected_countries:
                    self.selected_countries.append(country)
            else:
                if country in self.selected_countries:
                    self.selected_countries.remove(country)
        
        # Enable update button only if at least one country is selected
        self.update_button.setEnabled(len(self.selected_countries) > 0)
        
    def on_update_clicked(self):
        """Handle update button click"""
        # Gather settings
        protocols = ["https"]
        if self.http_check.isChecked():
            protocols.append("http")
        
        max_mirrors = self.mirror_spin.value()
        timeout = self.timeout_spin.value()
        sort_by = self.sort_combo.currentText().lower()
        
        # Build reflector command
        command = (
            f'pkexec reflector --country "{",".join(self.selected_countries)}" '
            f'--protocol {",".join(protocols)} '
            f'--latest {max_mirrors} '
            f'--sort {sort_by} '
            f'--download-timeout {timeout} '
            f'--save /etc/pacman.d/mirrorlist --verbose'
        )
        
        # Show log dialog and start update
        self.show_log_dialog()
        self.start_mirror_list_update(command)
        
    def show_log_dialog(self):
        """Show progress log dialog"""
        if self.log_dialog:
            self.log_dialog.show()
            return
        
        self.line_counter = 0
        
        self.log_dialog = QDialog(self)
        self.log_dialog.setWindowTitle("Update Progress")
        self.log_dialog.setMinimumSize(600, 400)
        self.log_dialog.setModal(False)
        self.log_dialog.setWindowFlags(
            Qt.Dialog | Qt.WindowTitleHint | Qt.CustomizeWindowHint
        )
        
        layout = QVBoxLayout(self.log_dialog)
        layout.setSpacing(10)
        layout.setContentsMargins(10, 10, 10, 10)
        
        # Create tree widget for logs
        self.log_tree = QTreeWidget()
        self.log_tree.setHeaderLabels(["Server", "Rate", "Time"])
        self.log_tree.header().setSectionResizeMode(0, QHeaderView.Stretch)
        self.log_tree.setAlternatingRowColors(True)
        
        layout.addWidget(self.log_tree)
        
        # Position next to parent
        parent_geo = self.geometry()
        self.log_dialog.move(
            parent_geo.x() + parent_geo.width() + 10,
            parent_geo.y()
        )
        
        self.log_dialog.show()
        
    def start_mirror_list_update(self, command):
        """Start mirror list update in background thread"""
        self.is_updating = True
        self.update_button.setEnabled(False)
        
        def run_update():
            try:
                process = subprocess.Popen(
                    ['sh', '-c', command],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    bufsize=1
                )
                
                self.line_counter = 0
                
                # Read stdout
                for line in iter(process.stdout.readline, ''):
                    if line:
                        self.process_log_line(line.strip())
                
                # Read stderr
                for line in iter(process.stderr.readline, ''):
                    if line:
                        self.process_log_line(line.strip())
                
                process.wait()
                
                self.signals.log_appended.emit("Update completed.", "", "")
                
            except Exception as e:
                print(f"Error during update: {e}")
                self.signals.log_appended.emit(f"Error: {e}", "", "")
            finally:
                self.signals.update_finished.emit()
        
        # Start update thread
        thread = threading.Thread(target=run_update, daemon=True)
        thread.start()
        
    def process_log_line(self, log_line):
        """Process a log line from reflector"""
        self.line_counter += 1
        if self.line_counter <= 2:
            return
        
        # Regular expression to match the log line format
        pattern = r'^\[.*?\]\s+(INFO|WARNING):\s+(.+)$'
        matches = re.match(pattern, log_line)
        
        if matches:
            log_type = matches.group(1)
            content = matches.group(2)
            
            if log_type == "INFO":
                # Try to parse server info
                server_pattern = r'^(https?://\S+)\s+(\S+\s+\S+/s)\s+(\S+\s+s)$'
                server_matches = re.match(server_pattern, content)
                
                if server_matches:
                    server = server_matches.group(1)
                    rate = server_matches.group(2)
                    time = server_matches.group(3)
                    self.signals.log_appended.emit(server, rate, time)
                else:
                    self.signals.log_appended.emit(content, "", "")
            elif log_type == "WARNING":
                self.signals.log_appended.emit(content, "WARNING", "N/A")
        
    def _append_log_to_ui(self, server, rate, time):
        """Append log to UI (called from main thread)"""
        if self.log_tree:
            item = QTreeWidgetItem([server, rate, time])
            self.log_tree.addTopLevelItem(item)
            self.log_tree.scrollToItem(item)
            
    def _on_update_finished(self):
        """Handle update finished (called from main thread)"""
        self.is_updating = False
        self.update_button.setEnabled(True)
        
        if self.log_dialog:
            self.log_dialog.close()
            self.log_dialog = None