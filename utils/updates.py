"""
System update utilities
"""

import subprocess
import os

def update_system(desktop_env):
    """
    Update the system using pacman in a terminal appropriate for the desktop environment
    Args:
        desktop_env: str - Desktop environment name (lowercase)
    """
    try:
        if desktop_env == "xfce":
            subprocess.Popen([
                'xfce4-terminal', '-x', 'pkexec', 'pacman', '--noconfirm', '-Syu'
            ])
        elif desktop_env == "gnome":
            subprocess.Popen([
                'gnome-terminal', '--', 'sudo', 'pacman', '--noconfirm', '-Syu'
            ])
        elif desktop_env == "kde":
            env = os.environ.copy()
            
            env.pop('LD_LIBRARY_PATH', None)
            env.pop('QT_PLUGIN_PATH', None)
            env.pop('QT_QPA_PLATFORM_THEME', None)
            subprocess.Popen([
                'konsole', '-e', 'sudo', 'pacman', '--noconfirm', '-Syu'
            ], env=env)
        else:
            print(f"Unsupported desktop environment: {desktop_env}")
    except Exception as e:
        print(f"Error executing command: {e}")