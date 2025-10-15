"""
Screen resolution settings utilities
"""

import subprocess


def screen_resolution(desktop_env):
    """
    Open screen resolution settings for the desktop environment
    Args:
        desktop_env: str - Desktop environment name (lowercase)
    """
    try:
        if desktop_env == "xfce":
            subprocess.Popen(['bash', '-c', 'xfce4-display-settings'])
        elif desktop_env == "gnome":
            subprocess.Popen(['gnome-control-center', 'display'])
        elif desktop_env == "kde":
            subprocess.Popen(['bash', '-c', 'kcmshell6 kcm_kscreen'])
        else:
            print(f"Unsupported desktop environment: {desktop_env}")
    except Exception as e:
        print(f"Error occurred: {e}")