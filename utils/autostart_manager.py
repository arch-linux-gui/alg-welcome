"""
Autostart management utilities for ALG Welcome application.
Handles enabling/disabling autostart functionality.
"""

import os
import subprocess
import shutil
from pathlib import Path


class AutostartManager:
    """Utility class for managing autostart functionality."""
    
    def __init__(self):
        self.autostart_dir = Path.home() / '.config' / 'autostart'
        self.autostart_file = self.autostart_dir / 'welcome.desktop'
        self.system_desktop_file = '/usr/share/applications/welcome.desktop'
        
    def is_autostart_enabled(self) -> bool:
        """Check if autostart is currently enabled."""
        return self.autostart_file.exists()
        
    def toggle_autostart(self, enable: bool):
        """Enable or disable autostart."""
        if enable:
            self._enable_autostart()
        else:
            self._disable_autostart()
            
    def _enable_autostart(self):
        """Enable autostart by copying desktop file to autostart directory."""
        if self.is_autostart_enabled():
            print("Autostart is already enabled")
            return
            
        try:
            # Create autostart directory if it doesn't exist
            self.autostart_dir.mkdir(parents=True, exist_ok=True)
            
            # Copy system desktop file to autostart directory
            if os.path.exists(self.system_desktop_file):
                shutil.copy2(self.system_desktop_file, self.autostart_file)
                print("Autostart enabled")
            else:
                print(f"System desktop file not found: {self.system_desktop_file}")
                
        except (OSError, IOError) as e:
            print(f"Error enabling autostart: {e}")
            
    def _disable_autostart(self):
        """Disable autostart by removing desktop file from autostart directory."""
        if not self.is_autostart_enabled():
            print("Autostart is already disabled")
            return
            
        try:
            # Remove autostart file
            self.autostart_file.unlink()
            print("Autostart disabled")
        except (OSError, IOError) as e:
            print(f"Error disabling autostart: {e}")
            
    def get_autostart_status(self) -> str:
        """Get current autostart status as string."""
        return "Enabled" if self.is_autostart_enabled() else "Disabled"