package utils

import (
	"fmt"
	"os"
	"os/exec"
	"os/user"
	"path/filepath"
)

func ToggleAutostart(enable bool) {
	autoDir := filepath.Join(os.Getenv("HOME"), ".config", "autostart")
	autoFile := filepath.Join(autoDir, "welcome.desktop")

	if _, err := os.Stat(autoDir); os.IsNotExist(err) {
		err := os.MkdirAll(autoDir, 0755)
		if err != nil {
			fmt.Println("Error creating directory:", err)
			return
		}
	}

	if enable {
		if _, err := os.Stat(autoFile); err == nil {
			fmt.Println("Autostart is already enabled")
			return
		}

		fmt.Println("Enabling autostart...")
		err := exec.Command("cp", "/usr/share/applications/welcome.desktop", autoFile).Run()
		if err != nil {
			fmt.Println("Error enabling autostart:", err)
			return
		}
		fmt.Println("Autostart enabled")
	} else {
		if _, err := os.Stat(autoFile); err != nil {
			fmt.Println("Autostart is already disabled")
			return
		}

		fmt.Println("Disabling autostart...")
		err := exec.Command("pkexec", "rm", autoFile).Run()
		if err != nil {
			fmt.Println("Error disabling autostart:", err)
			return
		}
		fmt.Println("Autostart disabled")
	}
}

func CheckFileExists() bool {
	usr, err := user.Current()
	if err != nil {
		return false
	}

	homeDir := usr.HomeDir
	filePath := filepath.Join(homeDir, ".config", "autostart", "welcome.desktop")

	_, err = os.Stat(filePath)
	return !os.IsNotExist(err)
}
