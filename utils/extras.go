package utils

import (
	"fmt"
	"os"
	"os/exec"
	"strings"
)

func GetDesktopEnvironment() string {
	return strings.ToLower(os.Getenv("XDG_CURRENT_DESKTOP"))
}

func URL(url string) {
	cmd := exec.Command("xdg-open", url)
	err := cmd.Run()
	if err != nil {
		fmt.Println("URL Error: " + err.Error())
	}
}

func CheckIfLiveISO() bool {
	_, err := os.Stat("/run/archiso")
	return err == nil
}

func RunCalamaresIfLiveISO(isLiveISO bool) {
	if isLiveISO {
		calamaresCmd := "`sudo calamares -D 8`"
		pkexecCmd := exec.Command("bash", "-c", calamaresCmd)
		output, err := pkexecCmd.CombinedOutput()
		if err != nil {
			if exitError, ok := err.(*exec.ExitError); ok {
				fmt.Println(exitError.ExitCode())
			}
			fmt.Printf("runCalamaresIfLiveISO Error: %s", err)
		}
		fmt.Println(string(output))
	}
}
