#!/usr/bin/env python3
"""
Test script to verify Calamares monitoring functionality
"""

import sys
import time
from pathlib import Path

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

from utils.extras import is_calamares_running, _calamares_running, _calamares_lock
import threading

def test_calamares_status():
    """Test the Calamares status checking functionality"""
    print("Testing Calamares monitoring functionality...")
    
    # Test initial state
    print(f"Initial Calamares status: {is_calamares_running()}")
    assert not is_calamares_running(), "Calamares should not be running initially"
    
    # Simulate Calamares starting
    print("Simulating Calamares start...")
    with _calamares_lock:
        global _calamares_running
        _calamares_running = True
    
    print(f"Calamares status after start: {is_calamares_running()}")
    assert is_calamares_running(), "Calamares should be running after start"
    
    # Simulate Calamares stopping
    print("Simulating Calamares stop...")
    with _calamares_lock:
        _calamares_running = False
    
    print(f"Calamares status after stop: {is_calamares_running()}")
    assert not is_calamares_running(), "Calamares should not be running after stop"
    
    print("All tests passed! Calamares monitoring functionality works correctly.")

if __name__ == "__main__":
    test_calamares_status()