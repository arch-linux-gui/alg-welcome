"""
Theme management utilities for ALG Welcome application.
Handles theme switching for different desktop environments.
"""

import os
import subprocess
import re
from typing import Optional


class ThemeManager:
    """Utility class for theme management."""
    
    def __init__(self):
        self.desktop_env = self._get_desktop_environment()
        
    def _get_desktop_environment(self) -> str:
        """Get the current desktop environment."""
        return os.environ.get('XDG_CURRENT_DESKTOP', '').lower()
        
    def is_dark_theme(self) -> bool:
        """Check if the current theme is dark."""
        current_theme = self.get_current_theme()
        return self._is_dark_theme(current_theme)
        
    def _is_dark_theme(self, theme: str) -> bool:
        """Check if a theme name indicates dark theme."""
        if not theme:
            return False
            
        theme_lower = theme.lower()
        return ('dark' in theme_lower or 
                'breezedark' in theme_lower or 
                'qogirdark' in theme_lower or 
                'prefer-dark' in theme_lower)
        
    def get_current_theme(self) -> str:
        """Get the current theme based on desktop environment."""
        if self.desktop_env == 'kde':
            return self._get_kde_theme()
        elif self.desktop_env == 'gnome':
            return self._get_gnome_theme()
        elif self.desktop_env == 'xfce':
            return self._get_xfce_theme()
        else:
            return ""
            
    def _get_kde_theme(self) -> str:
        """Get KDE theme."""
        config_files = [
            os.path.expanduser('~/.config/kdeglobals'),
            os.path.expanduser('~/.kde4/share/config/kdeglobals'),
            '/etc/kde/kdeglobals'
        ]
        
        for config_file in config_files:
            if os.path.exists(config_file):
                theme = self._get_color_scheme_from_file(config_file, '[KDE]', 'LookAndFeelPackage')
                if theme:
                    return self._format_color_scheme(theme)
                    
                theme = self._get_color_scheme_from_file(config_file, '[General]', 'ColorScheme')
                if theme:
                    return self._format_color_scheme(theme)
                    
        return "org.kde.breeze.desktop"
        
    def _get_gnome_theme(self) -> str:
        """Get GNOME theme."""
        try:
            result = subprocess.run(['gsettings', 'get', 'org.gnome.desktop.interface', 'color-scheme'], 
                                  capture_output=True, text=True, check=True)
            theme = result.stdout.strip().strip("'")
            return theme
        except (subprocess.CalledProcessError, FileNotFoundError):
            return ""
            
    def _get_xfce_theme(self) -> str:
        """Get XFCE theme."""
        try:
            result = subprocess.run(['xfconf-query', '-c', 'xsettings', '-p', '/Net/ThemeName', '-v'], 
                                  capture_output=True, text=True, check=True)
            return result.stdout.strip()
        except (subprocess.CalledProcessError, FileNotFoundError):
            return ""
            
    def _get_color_scheme_from_file(self, config_file: str, section_name: str, key_name: str) -> Optional[str]:
        """Get color scheme from config file."""
        try:
            with open(config_file, 'r') as f:
                in_target_section = False
                for line in f:
                    line = line.strip()
                    
                    if not line or line.startswith('#'):
                        continue
                        
                    if line == section_name:
                        in_target_section = True
                        continue
                        
                    if in_target_section and line.startswith(key_name + '='):
                        return line.split('=', 1)[1]
                        
                    if in_target_section and line.startswith('['):
                        break
                        
        except (OSError, IOError):
            pass
            
        return None
        
    def _format_color_scheme(self, color_scheme: str) -> str:
        """Format color scheme name."""
        if color_scheme in ['org.kde.breeze.desktop', 'org.kde.breezedark.desktop']:
            return color_scheme
        elif color_scheme == 'breeze':
            return 'org.kde.breeze.desktop'
        elif color_scheme == 'breezedark':
            return 'org.kde.breezedark.desktop'
        elif color_scheme.endswith('.colors'):
            return color_scheme[:-7]  # Remove .colors extension
        else:
            return color_scheme
            
    def toggle_theme(self, dark: bool):
        """Toggle between light and dark theme."""
        if self.desktop_env == 'kde':
            self._toggle_kde_theme(dark)
        elif self.desktop_env == 'gnome':
            self._toggle_gnome_theme(dark)
        elif self.desktop_env == 'xfce':
            self._toggle_xfce_theme(dark)
        else:
            print(f"Unsupported desktop environment: {self.desktop_env}")
            
    def _toggle_kde_theme(self, dark: bool):
        """Toggle KDE theme."""
        current_theme = self._get_kde_theme()
        
        if 'org.kde.breeze' in current_theme:
            # Pure KDE approach
            style = 'org.kde.breezedark.desktop' if dark else 'org.kde.breeze.desktop'
            try:
                subprocess.run(['lookandfeeltool', '--apply', style], check=True)
                print(f"KDE theme changed to {style}")
            except subprocess.CalledProcessError as e:
                print(f"Failed to change KDE theme: {e}")
        else:
            # Themed KDE approach
            if dark:
                style = 'Qogirdark'
                win_deco = '__aurorae__svg__Qogir-dark-circle'
            else:
                style = 'Qogirlight'
                win_deco = '__aurorae__svg__Qogir-light-circle'
                
            home_dir = os.path.expanduser('~')
            cmd = (f'plasma-apply-colorscheme {style} && '
                   f'kwriteconfig6 --file {home_dir}/.config/kwinrc '
                   f'--group org.kde.kdecoration2 --key theme {win_deco} && '
                   f'qdbus6 org.kde.KWin /KWin reconfigure')
            
            try:
                subprocess.run(['sh', '-c', cmd], check=True)
                print(f"KDE theme changed to {style}")
            except subprocess.CalledProcessError as e:
                print(f"Failed to change KDE theme: {e}")
                
    def _toggle_gnome_theme(self, dark: bool):
        """Toggle GNOME theme."""
        try:
            # Get current shell theme
            result = subprocess.run(['gsettings', 'get', 'org.gnome.shell.extensions.user-theme', 'name'], 
                                  capture_output=True, text=True)
            shell_theme = result.stdout.strip().strip("'")
            
            if 'Orchis' in shell_theme:
                # Orchis theme
                style = 'prefer-dark' if dark else 'prefer-light'
                shell = 'Orchis-Red-Dark' if dark else 'Orchis-Light'
                
                cmd = (f'gsettings set org.gnome.desktop.interface color-scheme {style} && '
                       f'gsettings set org.gnome.shell.extensions.user-theme name {shell}')
                subprocess.run(['sh', '-c', cmd], check=True)
            else:
                # Default GNOME
                style = 'prefer-dark' if dark else 'prefer-light'
                subprocess.run(['gsettings', 'set', 'org.gnome.desktop.interface', 'color-scheme', style], check=True)
                
            print(f"GNOME theme changed to {style}")
        except subprocess.CalledProcessError as e:
            print(f"Failed to change GNOME theme: {e}")
            
    def _toggle_xfce_theme(self, dark: bool):
        """Toggle XFCE theme."""
        try:
            current_theme = self._get_xfce_theme()
            
            if 'Qogir' in current_theme:
                style = 'Qogir-Dark' if dark else 'Qogir-Light'
            else:
                style = 'Adwaita-dark' if dark else 'Adwaita'
                
            cmd = (f'xfconf-query -c xsettings -p /Net/ThemeName -s {style} && '
                   f'xfconf-query -c xfwm4 -p /general/theme -s {style}')
            subprocess.run(['sh', '-c', cmd], check=True)
            print(f"XFCE theme changed to {style}")
        except subprocess.CalledProcessError as e:
            print(f"Failed to change XFCE theme: {e}")