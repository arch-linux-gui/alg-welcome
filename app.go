package main

import (
	"bufio"
	"context"
	"fmt"
	"io"
	"os"
	"os/exec"
	"os/user"
	"path/filepath"
	"strings"

	"github.com/wailsapp/wails/v2/pkg/runtime"
)

var desktopEnv string
var isLiveISO bool

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
	desktopEnv = getDesktopEnvironment()
	isLiveISO = checkIfLiveISO()
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
	isLiveISO = checkIfLiveISO()
}

func getDesktopEnvironment() string {
	return strings.ToLower(os.Getenv("XDG_CURRENT_DESKTOP"))
}

const defaultColorScheme = "org.kde.breeze.desktop"

func getLookAndFeelPackageKDE() string {
	configFiles := []string{
		os.ExpandEnv("$HOME/.config/kdeglobals"),
		os.ExpandEnv("$HOME/.kde4/share/config/kdeglobals"),
		"/etc/kde/kdeglobals",
	}

	for _, configFile := range configFiles {
		// Pure KDE approach
		colorScheme, err := getColorSchemeFromFile(configFile, "[KDE]", "LookAndFeelPackage")
		if err == nil {
			formattedScheme := formatColorScheme(colorScheme)
			if formattedScheme != "breeze" && formattedScheme != "breezedark" {
				// If not breeze or breezedark, continue to themed approach
				colorScheme, err = getColorSchemeFromFile(configFile, "[General]", "ColorScheme")
				if err == nil {
					return formatColorScheme(colorScheme)
				}
			} else {
				return formattedScheme
			}
		}

		// If not found, then themed KDE approach
		colorScheme, err = getColorSchemeFromFile(configFile, "[General]", "ColorScheme")
		if err == nil {
			return formatColorScheme(colorScheme)
		}
	}

	return defaultColorScheme
}

func getColorSchemeFromFile(configFile, sectionName, keyName string) (string, error) {
	file, err := os.Open(configFile)
	if err != nil {
		return "", fmt.Errorf("error opening file %s: %w", configFile, err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	inTargetSection := false
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())

		// Skip empty lines and comments
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}

		// Check for target section
		if line == sectionName {
			inTargetSection = true
			continue
		}

		// If we're in the target section and find the key, return its value
		if inTargetSection && strings.HasPrefix(line, keyName+"=") {
			return strings.TrimPrefix(line, keyName+"="), nil
		}

		// If we've moved past the target section, stop searching
		if inTargetSection && strings.HasPrefix(line, "[") {
			break
		}
	}

	if err := scanner.Err(); err != nil {
		return "", fmt.Errorf("error reading file %s: %w", configFile, err)
	}

	return "", fmt.Errorf("%s not found in %s section of %s", keyName, sectionName, configFile)
}

func formatColorScheme(colorScheme string) string {
	// Handle pure KDE themes
	switch colorScheme {
	case "org.kde.breeze.desktop":
		return colorScheme
	case "org.kde.breezedark.desktop":
		return colorScheme
	case "breeze":
		return "org.kde.breeze.desktop"
	case "breezedark":
		return "org.kde.breezedark.desktop"
	}

	// Handle themed KDE
	if filepath.Ext(colorScheme) == ".colors" {
		colorScheme = strings.TrimSuffix(filepath.Base(colorScheme), ".colors")
	}

	return colorScheme
}

func getShellTheme() string {
	cmd := exec.Command("gsettings", "get", "org.gnome.shell.extensions.user-theme", "name")
	output, err := cmd.Output()
	if err != nil {
		fmt.Println("Shell Theme error", err)
	}
	return strings.TrimSpace(string(output))
}

func getThemeNameXFCE() string {
	cmd := exec.Command("xfconf-query", "-c", "xsettings", "-p", "/Net/ThemeName", "-v")
	output, err := cmd.Output()
	if err != nil {
		fmt.Println("Curr theme Error:", err)
	}

	return strings.TrimSpace(string(output))
}

func (a *App) CurrentTheme() string {
	var currThemeName string
	switch desktopEnv {
	case "kde":
		currThemeName = getLookAndFeelPackageKDE()
		runtime.LogPrintf(a.ctx, "Current Theme %s on %s DE", currThemeName, desktopEnv)
	case "xfce":
		currThemeName = getThemeNameXFCE()
		runtime.LogPrintf(a.ctx, "Current Theme %s on %s DE", currThemeName, desktopEnv)
	case "gnome":
		cmd := exec.Command("gsettings", "get", "org.gnome.desktop.interface", "color-scheme")
		runtime.LogPrintf(a.ctx, "Current Theme %s on %s DE", currThemeName, desktopEnv)
		output, err := cmd.Output()
		if err != nil {
			fmt.Println("Curr theme Error:", err)
		}

		currThemeName = strings.Trim(strings.TrimSpace(string(output)), "'")
	}

	return currThemeName
}

/*
kwriteconfig6 --file ~/.config/kwinrc --group org.kde.kdecoration2 --key theme "__aurorae__svg__Qogir-Light"

kwriteconfig6 --file ~/.config/kwinrc --group org.kde.kdecoration2 --key theme "__aurorae__svg__Qogir-circle-Dark"

qdbus6 org.kde.KWin /KWin reconfigure
*/

func (a *App) ToggleTheme(dark bool) {
	var style string
	var winDeco string
	switch desktopEnv {
	case "kde":
		isPure := getLookAndFeelPackageKDE()
		if strings.Contains(isPure, "org.kde.breeze") {
			if dark {
				style = "org.kde.breezedark.desktop"
			} else {
				style = "org.kde.breeze.desktop"
			}
			cmd := exec.Command("lookandfeeltool", "--apply", style)
			_, err := cmd.Output()
			if err != nil {
				runtime.LogErrorf(a.ctx, "failed to change %s theme: %s", desktopEnv, err)
			}
			runtime.LogPrintf(a.ctx, "On %s theme changed to %s", desktopEnv, style)
		} else {
			if dark {
				style = "Qogirdark"
				winDeco = "__aurorae__svg__Qogir-dark-circle"
			} else {
				style = "Qogirlight"
				winDeco = "__aurorae__svg__Qogir-light-circle"
			}
			cmd := exec.Command("sh", "-c", fmt.Sprintf("plasma-apply-colorscheme %s && kwriteconfig6 --file %s/.config/kwinrc --group org.kde.kdecoration2 --key theme %s && qdbus6 org.kde.KWin /KWin reconfigure", style, os.Getenv("HOME"), winDeco))
			_, err := cmd.Output()
			if err != nil {
				runtime.LogErrorf(a.ctx, "failed to change %s theme: %s", desktopEnv, err)
			}
			runtime.LogPrintf(a.ctx, "On %s theme changed to %s", desktopEnv, style)
		}
	case "gnome":
		shellTheme := getShellTheme()
		var shell string
		if strings.Contains(shellTheme, "Orchis") {
			if dark {
				style = "prefer-dark"
				shell = "Orchis-Red-Dark"
			} else {
				style = "prefer-light"
				shell = "Orchis-Light"
			}
			cmd := exec.Command("sh", "-c", fmt.Sprintf("gsettings set org.gnome.desktop.interface color-scheme %s && gsettings set org.gnome.shell.extensions.user-theme name %s", style, shell))
			_, err := cmd.Output()
			if err != nil {
				runtime.LogErrorf(a.ctx, "failed to change %s theme: %s", desktopEnv, err)
			}
			runtime.LogPrintf(a.ctx, "On %s theme changed to %s", desktopEnv, style)
		} else {
			if dark {
				style = "prefer-dark"
			} else {
				style = "prefer-light"
			}
			cmd := exec.Command("gsettings", "set", "org.gnome.desktop.interface", "color-scheme", style)
			_, err := cmd.Output()
			if err != nil {
				runtime.LogErrorf(a.ctx, "failed to change %s theme: %s", desktopEnv, err)
			}
			runtime.LogPrintf(a.ctx, "On %s theme changed to %s", desktopEnv, style)
		}
	case "xfce":
		xfceThemeName := getThemeNameXFCE()
		if strings.Contains(xfceThemeName, "Qogir") {
			if dark {
				style = "Qogir-Dark"
			} else {
				style = "Qogir-Light"
			}
			cmd := exec.Command("sh", "-c", fmt.Sprintf("xfconf-query -c xsettings -p /Net/ThemeName -s %s && xfconf-query -c xfwm4 -p /general/theme -s %s", style, style))
			_, err := cmd.Output()
			if err != nil {
				runtime.LogErrorf(a.ctx, "failed to change %s theme: %s", desktopEnv, err)
			}
			runtime.LogPrintf(a.ctx, "On %s theme changed to %s", desktopEnv, style)
		} else {
			if dark {
				style = "Adwaita-dark"
			} else {
				style = "Adwaita"
			}
			cmd := exec.Command("sh", "-c", fmt.Sprintf("xfconf-query -c xsettings -p /Net/ThemeName -s %s && xfconf-query -c xfwm4 -p /general/theme -s %s", style, style))
			_, err := cmd.Output()
			if err != nil {
				runtime.LogErrorf(a.ctx, "failed to change %s theme: %s", desktopEnv, err)
			}
			runtime.LogPrintf(a.ctx, "On %s theme changed to %s", desktopEnv, style)
		}
	default:
		fmt.Println("unsupported desktop environment:", desktopEnv)
	}
}

func (a *App) LogMessage(command string) {
	cmd := exec.Command("bash", "-c", command)
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		a.LogMessage(fmt.Sprintf("Error creating StdoutPipe: %s", err.Error()))
		return
	}

	if err := cmd.Start(); err != nil {
		a.LogMessage(fmt.Sprintf("Error starting command: %s", err.Error()))
		return
	}

	scanner := bufio.NewScanner(stdout)
	for scanner.Scan() {
		logMessage := scanner.Text()
		fmt.Println("Logging:", logMessage)
		runtime.EventsEmit(a.ctx, "log", logMessage)
	}

	if err := scanner.Err(); err != nil {
		a.LogMessage(fmt.Sprintf("Error reading from scanner: %s", err.Error()))
	}

	if err := cmd.Wait(); err != nil {
		a.LogMessage(fmt.Sprintf("Error waiting for command: %s", err.Error()))
	}

	runtime.EventsEmit(a.ctx, "log", "Logging completed.")
}

// func (a *App) MirrorList(command string) {
// 	var pkexecCmd *exec.Cmd

// 	switch desktopEnv {
// 	case "xfce":
// 		pkexecCmd = exec.Command("xfce4-terminal", "-x", "bash", "-c", command)
// 		if err := pkexecCmd.Run(); err != nil {
// 			fmt.Printf("Error executing command: %v\n", err)
// 		}
// 	case "gnome":
// 		pkexecCmd = exec.Command("gnome-terminal", "--", "bash", "-c", command)
// 		if err := pkexecCmd.Run(); err != nil {
// 			fmt.Printf("Error executing command: %v\n", err)
// 		}
// 	case "kde":
// 		pkexecCmd = exec.Command("konsole", "-e", "bash", "-c", command)
// 		if err := pkexecCmd.Run(); err != nil {
// 			fmt.Printf("Error executing command: %v\n", err)
// 		}
// 	default:
// 		fmt.Printf("unsupported desktop environment: %s", desktopEnv)
// 		if err := pkexecCmd.Run(); err != nil {
// 			fmt.Printf("Error executing command: %v\n", err)
// 		}
// 	}
// }

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

func checkIfLiveISO() bool {
	_, err := os.Stat("/run/archiso")
	return err == nil
}

func (a *App) RunCalamaresIfLiveISO() {
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

func (a *App) IsLiveISO() bool {
	return isLiveISO
}

func (a *App) URL(url string) {
	cmd := exec.Command("xdg-open", url)
	err := cmd.Run()
	if err != nil {
		fmt.Println("Discord Error: " + err.Error())
	}
}

func (a *App) ToggleAutostart(enable bool) {
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

func (a *App) CheckFileExists() bool {
	usr, err := user.Current()
	if err != nil {
		return false
	}

	homeDir := usr.HomeDir
	filePath := filepath.Join(homeDir, ".config", "autostart", "welcome.desktop")

	_, err = os.Stat(filePath)
	return !os.IsNotExist(err)
}

func (a *App) MirrorList(command string) error {
	fmt.Println(command)
	return a.StartLogging(command)
}

func (a *App) StartLogging(command string) error {
	cmd := exec.Command("sh", "-c", command)
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return err
	}

	stderr, err := cmd.StderrPipe()
	if err != nil {
		return err
	}

	if err := cmd.Start(); err != nil {
		return err
	}

	go a.streamLogs(stdout)
	go a.streamLogs(stderr)

	return cmd.Wait()
}

func (a *App) streamLogs(pipe io.ReadCloser) {
	scanner := bufio.NewScanner(pipe)
	for scanner.Scan() {
		line := scanner.Text()
		fmt.Println(line)
		runtime.EventsEmit(a.ctx, "log", line)
	}
}
