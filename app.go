package main

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"strings"
)

var desktopEnv string

// App struct
type App struct {
	ctx context.Context
}

// NewApp creates a new App application struct
func NewApp() *App {
	return &App{}
}

// startup is called at application startup
func (a *App) startup(ctx context.Context) {
	// Perform your setup here
	a.ctx = ctx
}

// domReady is called after front-end resources have been loaded
func (a App) domReady(ctx context.Context) {
	// Add your action here
}

// beforeClose is called when the application is about to quit,
// either by clicking the window close button or calling runtime.Quit.
// Returning true will cause the application to continue, false will continue shutdown as normal.
func (a *App) beforeClose(ctx context.Context) (prevent bool) {
	return false
}

// shutdown is called at application termination
func (a *App) shutdown(ctx context.Context) {
	// Perform your teardown here
}

// Greet returns a greeting for the given name
func (a *App) Greet(name string) string {
	return fmt.Sprintf("Hello %s, It's show time!", name)
}

func init() {
	desktopEnv = getDesktopEnvironment()
}

func getDesktopEnvironment() string {
	return strings.ToLower(os.Getenv("XDG_CURRENT_DESKTOP"))
}

func (a *App) CurrentTheme() string {
	var currThemeName string
	switch desktopEnv {
	case "kde":
		cmd := exec.Command("kreadconfig5", "--file", "kdeglobals", "--group", "General", "--key", "widgetStyle")
		output, err := cmd.Output()
		if err != nil {
			fmt.Println("Curr theme Error:", err)
		}

		currThemeName = strings.TrimSpace(string(output))
	case "xfce":
		cmd := exec.Command("xfconf-query", "-c", "xsettings", "-p", "/Net/ThemeName", "-v")
		output, err := cmd.Output()
		if err != nil {
			fmt.Println("Curr theme Error:", err)
		}

		currThemeName = strings.TrimSpace(string(output))
	case "gnome":
		cmd := exec.Command("gsettings", "get", "org.gnome.desktop.interface", "color-scheme")
		output, err := cmd.Output()
		if err != nil {
			fmt.Println("Curr theme Error:", err)
		}

		currThemeName = strings.Trim(strings.TrimSpace(string(output)), "'")
	}

	return currThemeName
}

func (a *App) ToggleTheme(dark bool) {
	var style string
	switch desktopEnv {
	case "kde":
		if dark {
			style = "kde dark theme"
		} else {
			style = "kde light theme"
		}
		cmd := exec.Command("kwriteconfig5", "--file", "kdeglobals", "--group", "KDE", "--key", "LookAndFeelPackage", style)
		_, err := cmd.Output()
		if err != nil {
			fmt.Println("failed to change KDE theme:", err)
		}
	case "gnome":
		if dark {
			style = "prefer-dark"
		} else {
			style = "prefer-light"
		}
		cmd := exec.Command("gsettings", "set", "org.gnome.desktop.interface", "color-scheme", style)
		_, err := cmd.Output()
		if err != nil {
			fmt.Println("failed to change GNOME theme:", err)
		}
	case "xfce":
		if dark {
			style = "Adwaita-dark"
		} else {
			style = "Adwaita"
		}
		cmd := exec.Command("xfconf-query", "-c", "xsettings", "-p", "/Net/ThemeName", "-s", style)
		_, err := cmd.Output()
		if err != nil {
			fmt.Println("failed to change XFCE theme:", err)
		}
	default:
		fmt.Println("unsupported desktop environment:", desktopEnv)
	}
}

func (a *App) MirrorList(command string) {
	var pkexecCmd *exec.Cmd

	switch desktopEnv {
	case "xfce":
		pkexecCmd = exec.Command("xfce4-terminal", "-x", "bash", "-c", command)
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	case "gnome":
		pkexecCmd = exec.Command("gnome-terminal", "--", "bash", "-c", command)
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	case "kde":
		pkexecCmd = exec.Command("konsole", "-e", "bash", "-c", command)
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	default:
		fmt.Printf("unsupported desktop environment: %s", desktopEnv)
		if err := pkexecCmd.Run(); err != nil {
			fmt.Printf("Error executing command: %v\n", err)
		}
	}
}

func (a *App) UpdateSystem() {
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

func (a *App) ScreenResolution() {
	var pkexecCmd *exec.Cmd

	switch desktopEnv {
	case "xfce":
		pkexecCmd = exec.Command("bash", "-c", "xfce4-display-settings")
	case "gnome":
		pkexecCmd = exec.Command("bash", "-c", "gnome-control-center", "display")
	case "kde":
		pkexecCmd = exec.Command("bash", "-e", "kde screen resolution")
	default:
		fmt.Printf("unsupported desktop environment: %s\n", desktopEnv)
	}

	err := pkexecCmd.Run()
	if err != nil {
		fmt.Printf("Error Occured: %s", err)
	}
}

func (a *App) IsLiveISO() bool {
	var pkexecCmd *exec.Cmd
	_, err := os.Stat("/run/archiso")
	if err != nil {
		return false
	}
	pkexecCmd = exec.Command("bash", "-c", "`", "sudo", "-E", "calamares", "-D", "8", "`")
	if err = pkexecCmd.Run(); err != nil {
		if exitError, ok := err.(*exec.ExitError); ok {
			fmt.Println(exitError.ExitCode())
		}
		fmt.Printf("isLiveISO Error: %s", err)
	}
	return true
}
