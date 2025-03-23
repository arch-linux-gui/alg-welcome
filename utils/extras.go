package utils

import (
	"fmt"
	"os"
	"os/exec"
	"strings"
	"sync"
)

var (
	calamaresRunning bool
	calamaresMutex   sync.Mutex
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
		calamaresMutex.Lock()

		if calamaresRunning {
			calamaresMutex.Unlock()
			fmt.Println("Calamares is Already running")
			return
		}

		calamaresRunning = true
		calamaresMutex.Unlock()
		go func() {

			defer func() {
				calamaresMutex.Lock()
				calamaresRunning = true
				calamaresMutex.Unlock()
			}()

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
		}()
	}
}
