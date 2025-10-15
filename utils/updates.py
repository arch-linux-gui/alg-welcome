"""
System update utilities
"""

import subprocess


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
            subprocess.Popen([
                'konsole', '-e', 'sudo', 'pacman', '--noconfirm', '-Syu'
            ])
        else:
            print(f"Unsupported desktop environment: {desktop_env}")
    except Exception as e:
        print(f"Error executing command: {e}")