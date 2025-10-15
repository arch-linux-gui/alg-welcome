"""
System utilities for ALG Welcome application.
Handles system operations like Calamares, screen resolution, and system updates.
"""

import os
import subprocess
import platform
from typing import Optional


class SystemUtils:
    """Utility class for system operations."""
    
    def __init__(self):
        self.desktop_env = self._get_desktop_environment()
        self.is_live_iso = self._check_if_live_iso()
        
    def _get_desktop_environment(self) -> str:
        """Get the current desktop environment."""
        desktop_env = os.environ.get('XDG_CURRENT_DESKTOP', '').lower()
        return desktop_env
        
    def _check_if_live_iso(self) -> bool:
        """Check if running on a live ISO."""
        return os.path.exists('/run/archiso')
        
    def run_calamares_if_live_iso(self):
        """Run Calamares installer if on live ISO."""
        if self.is_live_iso:
            try:
                # Run Calamares installer
                subprocess.run(['bash', '-c', '/etc/calamares/launch.sh'], check=True)
                print("Calamares installer started")
            except subprocess.CalledProcessError as e:
                print(f"Error running Calamares: {e}")
            except FileNotFoundError:
                print("Calamares not found")
        else:
            print("Not running on live ISO")
            
    def open_screen_resolution_settings(self):
        """Open screen resolution settings based on desktop environment."""
        try:
            if self.desktop_env == 'xfce':
                subprocess.run(['xfce4-display-settings'], check=True)
            elif self.desktop_env == 'gnome':
                subprocess.run(['gnome-control-center', 'display'], check=True)
            elif self.desktop_env == 'kde':
                subprocess.run(['bash', '-c', 'kcmshell6 kcm_kscreen'], check=True)
            else:
                print(f"Unsupported desktop environment: {self.desktop_env}")
        except subprocess.CalledProcessError as e:
            print(f"Error opening display settings: {e}")
        except FileNotFoundError:
            print("Display settings application not found")
            
    def update_system(self):
        """Update the system using appropriate terminal based on desktop environment."""
        try:
            if self.desktop_env == 'xfce':
                subprocess.run(['xfce4-terminal', '-x', 'pkexec', 'pacman', '--noconfirm', '-Syu'], check=True)
            elif self.desktop_env == 'gnome':
                subprocess.run(['gnome-terminal', '--', 'sudo', 'pacman', '--noconfirm', '-Syu'], check=True)
            elif self.desktop_env == 'kde':
                subprocess.run(['konsole', '-e', 'sudo', 'pacman', '--noconfirm', '-Syu'], check=True)
            else:
                print(f"Unsupported desktop environment: {self.desktop_env}")
        except subprocess.CalledProcessError as e:
            print(f"Error updating system: {e}")
        except FileNotFoundError:
            print("Terminal application not found")
            
    def open_url(self, url: str):
        """Open a URL in the default browser."""
        try:
            subprocess.run(['xdg-open', url], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error opening URL {url}: {e}")
        except FileNotFoundError:
            print("xdg-open not found")
            
    def get_desktop_environment(self) -> str:
        """Get the current desktop environment."""
        return self.desktop_env
        
    def is_live_iso(self) -> bool:
        """Check if running on live ISO."""
        return self.is_live_iso