package utils

import (
	"fmt"
	"os/exec"
)

func ScreenResolution(desktopEnv string) {
	var pkexecCmd *exec.Cmd

	switch desktopEnv {
	case "xfce":
		pkexecCmd = exec.Command("bash", "-c", "xfce4-display-settings")
	case "gnome":
		pkexecCmd = exec.Command("gnome-control-center", "display")
	case "kde":
		kcm_kscreen := "`kcmshell6 kcm_kscreen`"
		pkexecCmd = exec.Command("bash", "-c", kcm_kscreen)
	default:
		fmt.Printf("unsupported desktop environment: %s\n", desktopEnv)
	}

	err := pkexecCmd.Run()
	if err != nil {
		fmt.Printf("Error Occured: %s", err)
	}
}
