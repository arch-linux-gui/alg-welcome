"""
Screen resolution settings utilities
"""

import subprocess
import os


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
            # subprocess.Popen(['bash', '-c', "`kcmshell6 kcm_kscreen`"])
            new_env = os.environ.copy()
            
            new_env.pop('LD_LIBRARY_PATH', None)
            subprocess.Popen(['kcmshell6', 'kcm_kscreen'], env=new_env)
        else:
            print(f"Unsupported desktop environment: {desktop_env}")
    except Exception as e:
        print(f"Error occurred: {e}")