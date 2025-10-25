"""
Theme management utilities for different desktop environments
Each desktop environment has its own class to handle theme operations
"""

import os
import subprocess
from pathlib import Path
from abc import ABC, abstractmethod


def is_dark_theme(theme):
    """
    Check if the theme is a dark theme
    Args:
        theme: str - Theme name
    Returns: bool - True if dark theme, False otherwise
    """
    theme_lower = theme.lower()
    return any(keyword in theme_lower for keyword in [
        'dark', 'breezedark', 'qogirdark', 'prefer-dark', 'orchis-dark'
    ])


class ThemeManager(ABC):
    """Abstract base class for theme managers"""
    
    @abstractmethod
    def get_current_theme(self):
        """Get the current theme name"""
        pass
    
    @abstractmethod
    def set_theme(self, dark):
        """Set theme to dark or light"""
        pass


class KDETheme(ThemeManager):
    """KDE Plasma theme manager"""
    
    DEFAULT_COLOR_SCHEME = "org.kde.breeze.desktop"
    
    def __init__(self):
        self.home = os.path.expandvars("$HOME")
        self.config_files = [
            f"{self.home}/.config/kdeglobals",
            f"{self.home}/.kde4/share/config/kdeglobals",
            "/etc/kde/kdeglobals",
        ]
    
    def _get_color_scheme_from_file(self, config_file, section_name, key_name):
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
    
    def _format_color_scheme(self, color_scheme):
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
    
    def _get_look_and_feel_package(self):
        """
        Get KDE Look and Feel package
        Returns: str - Color scheme name
        """
        for config_file in self.config_files:
            # Try pure KDE approach
            color_scheme = self._get_color_scheme_from_file(
                config_file, "[KDE]", "LookAndFeelPackage"
            )
            if color_scheme:
                formatted = self._format_color_scheme(color_scheme)
                if formatted not in ["org.kde.breeze.desktop", "org.kde.breezedark.desktop"]:
                    # Try themed approach
                    color_scheme = self._get_color_scheme_from_file(
                        config_file, "[General]", "ColorScheme"
                    )
                    if color_scheme:
                        return self._format_color_scheme(color_scheme)
                else:
                    return formatted
            
            # Try themed KDE approach
            color_scheme = self._get_color_scheme_from_file(
                config_file, "[General]", "ColorScheme"
            )
            if color_scheme:
                return self._format_color_scheme(color_scheme)
        
        return self.DEFAULT_COLOR_SCHEME
    
    def get_current_theme(self):
        """Get current KDE theme"""
        theme = self._get_look_and_feel_package()
        print(f"Current KDE Theme: {theme}")
        return theme
    
    def set_theme(self, dark):
        """
        Set KDE theme to dark or light
        Args:
            dark: bool - True for dark theme, False for light theme
        """
        try:
            current_package = self._get_look_and_feel_package()
            
            if "org.kde.breeze" in current_package:
                # Pure breeze theme
                style = "org.kde.breezedark.desktop" if dark else "org.kde.breeze.desktop"
                subprocess.run(['lookandfeeltool', '--apply', style], check=False)
                print(f"KDE theme changed to {style}")
            else:
                # Themed KDE (Qogir)
                if dark:
                    style = "Qogirdark"
                    win_deco = "__aurorae__svg__Qogir-dark-circle"
                else:
                    style = "Qogirlight"
                    win_deco = "__aurorae__svg__Qogir-light-circle"
                
                cmd = (
                    f"plasma-apply-colorscheme {style} && "
                    f"kwriteconfig6 --file {self.home}/.config/kwinrc "
                    f"--group org.kde.kdecoration2 --key theme {win_deco} && "
                    f"qdbus6 org.kde.KWin /KWin reconfigure"
                )
                subprocess.run(['sh', '-c', cmd], check=False)
                print(f"KDE theme changed to {style}")
                
        except Exception as e:
            print(f"Failed to change KDE theme: {e}")


class GNOMETheme(ThemeManager):
    """GNOME theme manager"""
    
    # Theme configurations
    DARK_THEME = {
        'icons': 'Tela-circle-dark',
        'shell': 'Orchis-Red-Dark',
        'gtk': 'Orchis-Red-Dark',
        'color_scheme': 'prefer-dark'
    }
    
    LIGHT_THEME = {
        'icons': 'Tela-circle',
        'shell': 'Orchis-Red-Light',
        'gtk': 'Orchis-Red-Light',
        'color_scheme': 'prefer-light'
    }
    
    def _get_gsetting(self, schema, key):
        """
        Get a gsetting value
        Args:
            schema: str - GSettings schema
            key: str - Setting key
        Returns: str - Setting value
        """
        try:
            result = subprocess.run(
                ['gsettings', 'get', schema, key],
                capture_output=True, text=True, check=False
            )
            return result.stdout.strip().strip("'")
        except Exception as e:
            print(f"Error getting gsetting {schema}.{key}: {e}")
            return ""
    
    def _set_gsetting(self, schema, key, value):
        """
        Set a gsetting value
        Args:
            schema: str - GSettings schema
            key: str - Setting key
            value: str - Setting value
        """
        try:
            subprocess.run(
                ['gsettings', 'set', schema, key, value],
                check=False
            )
        except Exception as e:
            print(f"Error setting gsetting {schema}.{key}: {e}")
    
    def get_current_theme(self):
        """Get current GNOME theme"""
        color_scheme = self._get_gsetting('org.gnome.desktop.interface', 'color-scheme')
        gtk_theme = self._get_gsetting('org.gnome.desktop.interface', 'gtk-theme')
        
        # Determine theme based on color scheme or GTK theme
        theme = color_scheme if color_scheme else gtk_theme
        print(f"Current GNOME Theme: {theme}")
        return theme
    
    def set_theme(self, dark):
        """
        Set GNOME theme to dark or light
        Args:
            dark: bool - True for dark theme, False for light theme
        """
        try:
            theme_config = self.DARK_THEME if dark else self.LIGHT_THEME
            
            # Set icon theme
            self._set_gsetting(
                'org.gnome.desktop.interface',
                'icon-theme',
                theme_config['icons']
            )
            print(f"GNOME icons set to: {theme_config['icons']}")
            
            # Set GTK theme (Legacy Applications)
            self._set_gsetting(
                'org.gnome.desktop.interface',
                'gtk-theme',
                theme_config['gtk']
            )
            print(f"GNOME GTK theme set to: {theme_config['gtk']}")
            
            # Set color scheme
            self._set_gsetting(
                'org.gnome.desktop.interface',
                'color-scheme',
                theme_config['color_scheme']
            )
            print(f"GNOME color scheme set to: {theme_config['color_scheme']}")
            
            # Set shell theme (requires user-theme extension)
            self._set_gsetting(
                'org.gnome.shell.extensions.user-theme',
                'name',
                theme_config['shell']
            )
            print(f"GNOME shell theme set to: {theme_config['shell']}")
            
        except Exception as e:
            print(f"Failed to change GNOME theme: {e}")


class XFCETheme(ThemeManager):
    """XFCE theme manager"""
    
    def _get_xfconf_value(self, channel, property_path):
        """
        Get XFCE configuration value
        Args:
            channel: str - XFCE configuration channel
            property_path: str - Property path
        Returns: str - Property value
        """
        try:
            result = subprocess.run(
                ['xfconf-query', '-c', channel, '-p', property_path],
                capture_output=True, text=True, check=False
            )
            return result.stdout.strip()
        except Exception as e:
            print(f"Error getting xfconf value {channel}{property_path}: {e}")
            return ""
    
    def _set_xfconf_value(self, channel, property_path, value):
        """
        Set XFCE configuration value
        Args:
            channel: str - XFCE configuration channel
            property_path: str - Property path
            value: str - Property value
        """
        try:
            subprocess.run(
                ['xfconf-query', '-c', channel, '-p', property_path, '-s', value],
                check=False
            )
        except Exception as e:
            print(f"Error setting xfconf value {channel}{property_path}: {e}")
    
    def get_current_theme(self):
        """Get current XFCE theme"""
        theme = self._get_xfconf_value('xsettings', '/Net/ThemeName')
        print(f"Current XFCE Theme: {theme}")
        return theme
    
    def set_theme(self, dark):
        """
        Set XFCE theme to dark or light
        Args:
            dark: bool - True for dark theme, False for light theme
        """
        try:
            current_theme = self.get_current_theme()
            
            # Determine theme based on current theme
            if "Qogir" in current_theme:
                style = "Qogir-Dark" if dark else "Qogir-Light"
            else:
                style = "Adwaita-dark" if dark else "Adwaita"
            
            # Set GTK theme
            self._set_xfconf_value('xsettings', '/Net/ThemeName', style)
            print(f"XFCE GTK theme set to: {style}")
            
            # Set window manager theme
            self._set_xfconf_value('xfwm4', '/general/theme', style)
            print(f"XFCE WM theme set to: {style}")
            
        except Exception as e:
            print(f"Failed to change XFCE theme: {e}")


# Factory function to get appropriate theme manager
def get_theme_manager(desktop_env):
    """
    Get the appropriate theme manager for the desktop environment
    Args:
        desktop_env: str - Desktop environment name (lowercase)
    Returns: ThemeManager - Theme manager instance or None
    """
    managers = {
        'kde': KDETheme,
        'gnome': GNOMETheme,
        'xfce': XFCETheme,
    }
    
    manager_class = managers.get(desktop_env)
    if manager_class:
        return manager_class()
    else:
        print(f"Unsupported desktop environment: {desktop_env}")
        return None


# Public API functions (for backward compatibility)
def current_theme(desktop_env):
    """
    Get current theme for the desktop environment
    Args:
        desktop_env: str - Desktop environment name
    Returns: str - Current theme name
    """
    manager = get_theme_manager(desktop_env)
    if manager:
        return manager.get_current_theme()
    return ""


def toggle_theme(dark, desktop_env):
    """
    Toggle between light and dark theme
    Args:
        dark: bool - True for dark theme, False for light theme
        desktop_env: str - Desktop environment name
    """
    manager = get_theme_manager(desktop_env)
    if manager:
        manager.set_theme(dark)
    else:
        print(f"Cannot toggle theme for unsupported desktop environment: {desktop_env}")