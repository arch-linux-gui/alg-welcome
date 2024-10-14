package utils

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func IsDarkTheme(theme string) bool {
	lowercaseTheme := strings.ToLower(theme)
	return strings.Contains(lowercaseTheme, "dark") ||
		strings.Contains(lowercaseTheme, "breezedark") ||
		strings.Contains(lowercaseTheme, "qogirdark") ||
		strings.Contains(lowercaseTheme, "prefer-dark")
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
			if formattedScheme != "org.kde.breeze.desktop" && formattedScheme != "org.kde.breezedark.desktop" {
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

func CurrentTheme(desktopEnv string) string {
	var currThemeName string
	switch desktopEnv {
	case "kde":
		currThemeName = getLookAndFeelPackageKDE()
		log.Printf("Current Theme %s on %s DE", currThemeName, desktopEnv)
	case "xfce":
		currThemeName = getThemeNameXFCE()
		log.Printf("Current Theme %s on %s DE", currThemeName, desktopEnv)
	case "gnome":
		cmd := exec.Command("gsettings", "get", "org.gnome.desktop.interface", "color-scheme")
		log.Printf("Current Theme %s on %s DE", currThemeName, desktopEnv)
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

func ToggleTheme(dark bool, desktopEnv string) {
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
				log.Printf("failed to change %s theme: %s", desktopEnv, err)
			}
			log.Printf("On %s theme changed to %s", desktopEnv, style)
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
				log.Printf("failed to change %s theme: %s", desktopEnv, err)
			}
			log.Printf("On %s theme changed to %s", desktopEnv, style)
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
				log.Printf("failed to change %s theme: %s", desktopEnv, err)
			}
			log.Printf("On %s theme changed to %s", desktopEnv, style)
		} else {
			if dark {
				style = "prefer-dark"
			} else {
				style = "prefer-light"
			}
			cmd := exec.Command("gsettings", "set", "org.gnome.desktop.interface", "color-scheme", style)
			_, err := cmd.Output()
			if err != nil {
				log.Printf("failed to change %s theme: %s", desktopEnv, err)
			}
			log.Printf("On %s theme changed to %s", desktopEnv, style)
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
				log.Printf("failed to change %s theme: %s", desktopEnv, err)
			}
			log.Printf("On %s theme changed to %s", desktopEnv, style)
		} else {
			if dark {
				style = "Adwaita-dark"
			} else {
				style = "Adwaita"
			}
			cmd := exec.Command("sh", "-c", fmt.Sprintf("xfconf-query -c xsettings -p /Net/ThemeName -s %s && xfconf-query -c xfwm4 -p /general/theme -s %s", style, style))
			_, err := cmd.Output()
			if err != nil {
				log.Printf("failed to change %s theme: %s", desktopEnv, err)
			}
			log.Printf("On %s theme changed to %s", desktopEnv, style)
		}
	default:
		fmt.Println("unsupported desktop environment:", desktopEnv)
	}
}
