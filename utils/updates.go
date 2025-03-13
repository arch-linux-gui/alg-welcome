package utils

import (
	"fmt"
	"os/exec"
)

func UpdateSystem(desktopEnv string) {
	var pkexecCmd *exec.Cmd

	switch desktopEnv {
	case "xfce":
		pkexecCmd = exec.Command("xfce4-terminal", "-x", "pkexec", "pacman", "--noconfirm", "-Syu")
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	case "gnome":
		pkexecCmd = exec.Command("gnome-terminal", "--", "sudo", "pacman", "--noconfirm", "-Syu")
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	case "kde":
		pkexecCmd = exec.Command("konsole", "-e", "sudo", "pacman", "--noconfirm", "-Syu")
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	default:
		fmt.Printf("unsupported desktop environment: %s\n", desktopEnv)
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	}
}
