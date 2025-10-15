"""
Utils package for Welcome to ALG application
"""

from .extras import get_desktop_environment, open_url, check_if_live_iso, run_calamares_if_live_iso
from .autostart import toggle_autostart, check_file_exists
from .themes import toggle_theme, current_theme, is_dark_theme
from .resolution import screen_resolution
from .updates import update_system
from .mirrorlist import MirrorListDialog
from .aboutus import AboutUsDialog

__all__ = [
    'get_desktop_environment',
    'open_url',
    'check_if_live_iso',
    'run_calamares_if_live_iso',
    'toggle_autostart',
    'check_file_exists',
    'toggle_theme',
    'current_theme',
    'is_dark_theme',
    'screen_resolution',
    'update_system',
    'MirrorListDialog',
    'AboutUsDialog',
]