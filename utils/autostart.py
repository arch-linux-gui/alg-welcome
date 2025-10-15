"""
Autostart management utilities
"""

import os
import subprocess
import shutil
from pathlib import Path


def toggle_autostart(enable):
    """
    Toggle autostart for the welcome application
    Args:
        enable: bool - True to enable autostart, False to disable
    """
    home = Path.home()
    autostart_dir = home / ".config" / "autostart"
    autostart_file = autostart_dir / "welcome.desktop"
    source_file = Path("/usr/share/applications/welcome.desktop")
    
    # Create autostart directory if it doesn't exist
    if not autostart_dir.exists():
        try:
            autostart_dir.mkdir(parents=True, mode=0o755)
        except Exception as e:
            print(f"Error creating directory: {e}")
            return
    
    if enable:
        # Enable autostart
        if autostart_file.exists():
            print("Autostart is already enabled")
            return
        
        print("Enabling autostart...")
        try:
            if source_file.exists():
                shutil.copy2(source_file, autostart_file)
                print("Autostart enabled")
            else:
                print(f"Source file {source_file} not found")
        except Exception as e:
            print(f"Error enabling autostart: {e}")
    else:
        # Disable autostart
        if not autostart_file.exists():
            print("Autostart is already disabled")
            return
        
        print("Disabling autostart...")
        try:
            # Use pkexec to remove file (in case permissions needed)
            subprocess.run(['pkexec', 'rm', str(autostart_file)], check=False)
            print("Autostart disabled")
        except Exception as e:
            print(f"Error disabling autostart: {e}")


def check_file_exists():
    """
    Check if autostart file exists
    Returns: bool - True if autostart is enabled, False otherwise
    """
    home = Path.home()
    autostart_file = home / ".config" / "autostart" / "welcome.desktop"
    return autostart_file.exists()