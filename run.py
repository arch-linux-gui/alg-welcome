#!/usr/bin/env python3
"""
ALG Welcome Application Launcher
Simple launcher script for the ALG Welcome application.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import and run the main application
if __name__ == "__main__":
    try:
        from main import main
        main()
    except ImportError as e:
        print(f"Error importing application: {e}")
        print("Make sure PySide6 is installed: pip install PySide6")
        sys.exit(1)
    except Exception as e:
        print(f"Error running application: {e}")
        sys.exit(1)
