"""
Theme management utilities for different desktop environments
"""

import os
import subprocess
from pathlib import Path


def is_dark_theme(theme):
    """
    Check if the theme is a dark theme
    Args:
        theme: str - Theme name
    Returns: bool - True if dark theme, False otherwise
    """
    theme_lower = theme.lower()
    return any(keyword in theme_lower for keyword in [
        'dark', 'breezedark', 'qogirdark', 'prefer-dark'
    ])


DEFAULT_COLOR_SCHEME = "org.kde.breeze.desktop"


def get_color_scheme_from_file(config_file, section_name, key_name):
    """
    Read color scheme from KDE config file
    Args:
        config_file: str - Path to config file
        section_name: str - Section name in config file
        key_name: str - Key name to read
    Returns: str or None - Value if found, None otherwise
    """
    try:
        with open(config_file, 'r') as f:
            in_target_section = False
            for line in f:
                line = line.strip()
                
                # Skip empty lines and comments
                if not line or line.startswith('#'):
                    continue
                
                # Check for target section
                if line == section_name:
                    in_target_section = True
                    continue
                
                # If in target section and find the key, return value
                if in_target_section and line.startswith(f"{key_name}="):
                    return line.split('=', 1)[1]
                
                # If moved past target section, stop
                if in_target_section and line.startswith('['):
                    break
    except Exception as e:
        print(f"Error reading {config_file}: {e}")
    
    return None


def format_color_scheme(color_scheme):
    """
    Format KDE color scheme name
    Args:
        color_scheme: str - Raw color scheme name
    Returns: str - Formatted color scheme name
    """
    scheme_map = {
        "org.kde.breeze.desktop": "org.kde.breeze.desktop",
        "org.kde.breezedark.desktop": "org.kde.breezedark.desktop",
        "breeze": "org.kde.breeze.desktop",
        "breezedark": "org.kde.breezedark.desktop",
    }
    
    if color_scheme in scheme_map:
        return scheme_map[color_scheme]
    
    # Handle themed KDE
    if color_scheme.endswith('.colors'):
        return Path(color_scheme).stem
    
    return color_scheme


def get_look_and_feel_package_kde():
    """
    Get KDE Look and Feel package
    Returns: str - Color scheme name
    """
    home = os.path.expandvars("$HOME")
    config_files = [
        f"{home}/.config/kdeglobals",
        f"{home}/.kde4/share/config/kdeglobals",
        "/etc/kde/kdeglobals",
    ]
    
    for config_file in config_files:
        # Try pure KDE approach
        color_scheme = get_color_scheme_from_file(config_file, "[KDE]", "LookAndFeelPackage")
        if color_scheme:
            formatted = format_color_scheme(color_scheme)
            if formatted not in ["org.kde.breeze.desktop", "org.kde.breezedark.desktop"]:
                # Try themed approach
                color_scheme = get_color_scheme_from_file(config_file, "[General]", "ColorScheme")
                if color_scheme:
                    return format_color_scheme(color_scheme)
            else:
                return formatted
        
        # Try themed KDE approach
        color_scheme = get_color_scheme_from_file(config_file, "[General]", "ColorScheme")
        if color_scheme:
            return format_color_scheme(color_scheme)
    
    return DEFAULT_COLOR_SCHEME


def get_shell_theme():
    """
    Get GNOME shell theme
    Returns: str - Shell theme name
    """
    try:
        result = subprocess.run(
            ['gsettings', 'get', 'org.gnome.shell.extensions.user-theme', 'name'],
            capture_output=True, text=True, check=False
        )
        return result.stdout.strip()
    except Exception as e:
        print(f"Shell Theme error: {e}")
        return ""


def get_theme_name_xfce():
    """
    Get XFCE theme name
    Returns: str - Theme name
    """
    try:
        result = subprocess.run(
            ['xfconf-query', '-c', 'xsettings', '-p', '/Net/ThemeName', '-v'],
            capture_output=True, text=True, check=False
        )
        return result.stdout.strip()
    except Exception as e:
        print(f"Current theme error: {e}")
        return ""


def current_theme(desktop_env):
    """
    Get current theme for the desktop environment
    Args:
        desktop_env: str - Desktop environment name
    Returns: str - Current theme name
    """
    curr_theme_name = ""
    
    if desktop_env == "kde":
        curr_theme_name = get_look_and_feel_package_kde()
        print(f"Current Theme {curr_theme_name} on {desktop_env} DE")
    elif desktop_env == "xfce":
        curr_theme_name = get_theme_name_xfce()
        print(f"Current Theme {curr_theme_name} on {desktop_env} DE")
    elif desktop_env == "gnome":
        try:
            result = subprocess.run(
                ['gsettings', 'get', 'org.gnome.desktop.interface', 'color-scheme'],
                capture_output=True, text=True, check=False
            )
            curr_theme_name = result.stdout.strip().strip("'")
            print(f"Current Theme {curr_theme_name} on {desktop_env} DE")
        except Exception as e:
            print(f"Current theme error: {e}")
    
    return curr_theme_name


def toggle_theme(dark, desktop_env):
    """
    Toggle between light and dark theme
    Args:
        dark: bool - True for dark theme, False for light theme
        desktop_env: str - Desktop environment name
    """
    try:
        if desktop_env == "kde":
            is_pure = get_look_and_feel_package_kde()
            
            if "org.kde.breeze" in is_pure:
                # Pure breeze theme
                style = "org.kde.breezedark.desktop" if dark else "org.kde.breeze.desktop"
                subprocess.run(['lookandfeeltool', '--apply', style], check=False)
                print(f"On {desktop_env} theme changed to {style}")
            else:
                # Themed KDE (Qogir)
                if dark:
                    style = "Qogirdark"
                    win_deco = "__aurorae__svg__Qogir-dark-circle"
                else:
                    style = "Qogirlight"
                    win_deco = "__aurorae__svg__Qogir-light-circle"
                
                home = os.environ.get('HOME', '')
                cmd = (
                    f"plasma-apply-colorscheme {style} && "
                    f"kwriteconfig6 --file {home}/.config/kwinrc "
                    f"--group org.kde.kdecoration2 --key theme {win_deco} && "
                    f"qdbus6 org.kde.KWin /KWin reconfigure"
                )
                subprocess.run(['sh', '-c', cmd], check=False)
                print(f"On {desktop_env} theme changed to {style}")
                
        elif desktop_env == "gnome":
            shell_theme = get_shell_theme()
            
            if "Orchis" in shell_theme:
                if dark:
                    style = "prefer-dark"
                    shell = "Orchis-Red-Dark"
                else:
                    style = "prefer-light"
                    shell = "Orchis-Light"
                
                cmd = (
                    f"gsettings set org.gnome.desktop.interface color-scheme {style} && "
                    f"gsettings set org.gnome.shell.extensions.user-theme name {shell}"
                )
                subprocess.run(['sh', '-c', cmd], check=False)
                print(f"On {desktop_env} theme changed to {style}")
            else:
                style = "prefer-dark" if dark else "prefer-light"
                subprocess.run([
                    'gsettings', 'set', 'org.gnome.desktop.interface', 'color-scheme', style
                ], check=False)
                print(f"On {desktop_env} theme changed to {style}")
                
        elif desktop_env == "xfce":
            xfce_theme_name = get_theme_name_xfce()
            
            if "Qogir" in xfce_theme_name:
                style = "Qogir-Dark" if dark else "Qogir-Light"
            else:
                style = "Adwaita-dark" if dark else "Adwaita"
            
            cmd = (
                f"xfconf-query -c xsettings -p /Net/ThemeName -s {style} && "
                f"xfconf-query -c xfwm4 -p /general/theme -s {style}"
            )
            subprocess.run(['sh', '-c', cmd], check=False)
            print(f"On {desktop_env} theme changed to {style}")
        else:
            print(f"Unsupported desktop environment: {desktop_env}")
            
    except Exception as e:
        print(f"Failed to change {desktop_env} theme: {e}")