package main

import (
	"log"
	"strings"

	"github.com/gotk3/gotk3/gdk"
	"github.com/gotk3/gotk3/gtk"

	utils "github.com/harshau007/welcome/utils"
)

const (
	windowTitle  = "Welcome to ALG"
	windowWidth  = 500
	windowHeight = 400
	cssFilePath  = "styles.css"
	logoFilePath = "/usr/share/pixmaps/welcome.png"
	logoSize     = 60
	iconSize     = 20
)

var desktopEnv string
var isLiveISO bool
var win *gtk.Window

func init() {
	desktopEnv = utils.GetDesktopEnvironment()
	isLiveISO = utils.CheckIfLiveISO()
}

func main() {
	gtk.Init(nil)

	win, err := setupWindow()
	if err != nil {
		log.Fatal("Unable to create window:", err)
	}

	vbox, err := setupMainLayout()
	if err != nil {
		log.Fatal("Unable to create main layout:", err)
	}

	addHeader(vbox)
	addInstallSetupSection(vbox)
	addSocialMediaSection(vbox)
	addGetStartedSection(vbox)
	addAboutUsSection(vbox)

	win.Add(vbox)
	win.ShowAll()

	win.Connect("delete-event", func() bool {
		gtk.MainQuit()
		return false
	})
	gtk.Main()
}

func setupWindow() (*gtk.Window, error) {
	win, err := gtk.WindowNew(gtk.WINDOW_TOPLEVEL)
	if err != nil {
		return nil, err
	}
	win.SetTitle(windowTitle)
	win.SetSizeRequest(windowWidth, windowHeight)
	win.SetResizable(true)
	win.SetPosition(gtk.WIN_POS_CENTER)

	applyCSS()

	return win, nil
}

func setupMainLayout() (*gtk.Box, error) {
	vbox, err := gtk.BoxNew(gtk.ORIENTATION_VERTICAL, 10)
	if err != nil {
		return nil, err
	}
	vbox.SetHomogeneous(false)
	vbox.SetMarginStart(20)
	vbox.SetMarginEnd(20)
	vbox.SetMarginTop(10)
	vbox.SetMarginBottom(10)
	return vbox, nil
}

func applyCSS() {
	provider, err := gtk.CssProviderNew()
	if err != nil {
		log.Println("Unable to create CSS provider:", err)
		return
	}

	err = provider.LoadFromPath(cssFilePath)
	if err != nil {
		log.Println("Unable to load CSS file:", err)
		return
	}

	screen, err := gdk.ScreenGetDefault()
	if err != nil {
		log.Println("Unable to get default screen:", err)
		return
	}

	gtk.AddProviderForScreen(screen, provider, gtk.STYLE_PROVIDER_PRIORITY_USER)
}

func createButtonWithIcon(label, iconName string, fromFile bool) (*gtk.Button, error) {
	button, err := gtk.ButtonNew()
	if err != nil {
		return nil, err
	}

	box, err := gtk.BoxNew(gtk.ORIENTATION_HORIZONTAL, 5)
	if err != nil {
		return nil, err
	}

	labelWidget, err := gtk.LabelNew(label)
	if err != nil {
		return nil, err
	}

	var icon *gtk.Image
	if fromFile {
		pixbuf, err := gdk.PixbufNewFromFileAtScale(iconName, iconSize, iconSize, true)
		if err != nil {
			log.Println("Unable to load logo:", err)
		}
		icon, err = gtk.ImageNewFromPixbuf(pixbuf)
		if err != nil {
			return nil, err
		}
	} else {
		icon, err = gtk.ImageNewFromIconName(iconName, gtk.ICON_SIZE_BUTTON)
		if err != nil {
			return nil, err
		}
		icon.SetPixelSize(iconSize)
	}

	box.PackStart(labelWidget, true, true, 0)
	box.PackEnd(icon, false, false, 0)
	button.Add(box)
	button.SetCanFocus(false)
	button.Connect("clicked", func() {
		if strings.Contains(label, "Screen Resolution") {
			utils.ScreenResolution(desktopEnv)
		}

		if strings.Contains(label, "Update System") {
			utils.UpdateSystem(desktopEnv)
		}

		if strings.Contains(label, "Tutorials") {
			utils.URL("https://arkalinuxgui.org/tutorials")
		}

		if strings.Contains(label, "GitHub") {
			utils.URL("https://github.com/arch-linux-gui")
		}

		if strings.Contains(label, "Discord") {
			utils.URL("https://discord.gg/NgAFEw9Tkf")
		}

		if strings.Contains(label, "Update Mirrorlist") {
			utils.MirrorList(win)
		}

		if strings.Contains(label, "Install ALG") {
			utils.RunCalamaresIfLiveISO(isLiveISO)
		}
	})
	return button, nil
}

func createSwitchWithLabel(labelText string, fn func(*gtk.Switch, bool)) (*gtk.Box, *gtk.Switch, error) {
	hbox, err := gtk.BoxNew(gtk.ORIENTATION_HORIZONTAL, 5)
	if err != nil {
		return nil, nil, err
	}
	label, err := gtk.LabelNew(labelText)
	if err != nil {
		return nil, nil, err
	}
	switchWidget, err := gtk.SwitchNew()
	if err != nil {
		return nil, nil, err
	}
	switchWidget.SetCanFocus(false)
	// switchWidget.SetSizeRequest(5, 5)
	switchWidget.Connect("state-set", fn)
	hbox.PackStart(label, false, false, 0)
	hbox.PackEnd(switchWidget, false, false, 5)
	return hbox, switchWidget, nil
}

func addHeader(vbox *gtk.Box) {
	headerBox, _ := gtk.BoxNew(gtk.ORIENTATION_HORIZONTAL, 10)

	pixbuf, err := gdk.PixbufNewFromFileAtScale(logoFilePath, logoSize, logoSize, true)
	if err != nil {
		log.Println("Unable to load ALG logo:", err)
		return
	}
	algImage, _ := gtk.ImageNewFromPixbuf(pixbuf)

	welcomeLabel, _ := gtk.LabelNew("Welcome")
	welcomeLabel.SetName("header")
	welcomeLabel.SetHAlign(gtk.ALIGN_START)

	headerBox.PackStart(algImage, false, false, 0)
	headerBox.PackStart(welcomeLabel, true, true, 0)

	vbox.PackStart(headerBox, false, false, 0)
}

func addInstallSetupSection(vbox *gtk.Box) {
	sectionLabel, _ := gtk.LabelNew("Install & Setup")
	sectionLabel.SetHAlign(gtk.ALIGN_START)
	vbox.PackStart(sectionLabel, false, false, 0)

	grid, _ := gtk.GridNew()
	grid.SetColumnSpacing(10)
	grid.SetRowSpacing(10)
	grid.SetColumnHomogeneous(true)

	var buttons []struct {
		label string
		icon  string
	}

	if isLiveISO {
		buttons = []struct {
			label string
			icon  string
		}{
			{"Install ALG", "system-software-install"},
			{"Screen Resolution", "video-display"},
			{"Update System", "system-software-update"},
			{"Update Mirrorlist", "view-refresh"},
		}
	} else {
		buttons = []struct {
			label string
			icon  string
		}{
			{"Tutorials", "help-contents"},
			{"Screen Resolution", "video-display"},
			{"Update System", "system-software-update"},
			{"Update Mirrorlist", "view-refresh"},
		}
	}

	for i, btn := range buttons {
		button, _ := createButtonWithIcon(btn.label, btn.icon, false)
		grid.Attach(button, i%2, i/2, 1, 1)
	}

	vbox.PackStart(grid, true, true, 0)
}

func addSocialMediaSection(vbox *gtk.Box) {
	sectionLabel, _ := gtk.LabelNew("Social Media Links")
	sectionLabel.SetHAlign(gtk.ALIGN_START)
	vbox.PackStart(sectionLabel, false, false, 0)

	grid, _ := gtk.GridNew()
	grid.SetColumnSpacing(10)
	grid.SetColumnHomogeneous(true)

	buttons := []struct {
		label string
		icon  string
	}{
		{"GitHub", "assets/github.svg"},
		{"Discord", "assets/discord.svg"},
	}

	for i, btn := range buttons {
		button, _ := createButtonWithIcon(btn.label, btn.icon, true)
		grid.Attach(button, i, 0, 1, 1)
	}

	vbox.PackStart(grid, false, false, 0)
}

func addGetStartedSection(vbox *gtk.Box) {
	sectionLabel, _ := gtk.LabelNew("Get Started")
	sectionLabel.SetHAlign(gtk.ALIGN_START)
	vbox.PackStart(sectionLabel, false, false, 0)

	hbox, _ := gtk.BoxNew(gtk.ORIENTATION_HORIZONTAL, 10)

	// Autostart Toggle
	autostartBox, autostartSwitch, _ := createSwitchWithLabel("AutoStart:", func(s *gtk.Switch, state bool) {
		utils.ToggleAutostart(state)
	})

	// Set initial state of autostart switch
	autostartSwitch.SetActive(utils.CheckFileExists())

	// Theme Toggle
	darkthemeBox, themeSwitch, _ := createSwitchWithLabel("Dark Theme:", func(s *gtk.Switch, state bool) {
		utils.ToggleTheme(state, desktopEnv)
	})

	// Set initial state of theme switch
	currentTheme := utils.CurrentTheme(desktopEnv)
	isDarkTheme := utils.IsDarkTheme(currentTheme)
	themeSwitch.SetActive(isDarkTheme)

	hbox.PackStart(autostartBox, true, true, 0)
	hbox.PackStart(darkthemeBox, true, true, 0)

	vbox.PackStart(hbox, false, false, 0)
}

func addAboutUsSection(vbox *gtk.Box) {
	button, _ := gtk.ButtonNewWithLabel("About Us")
	button.SetCanFocus(false)
	button.SetMarginTop(5)
	button.SetMarginBottom(5)
	button.Connect("clicked", func() {
		utils.AboutUs(win)
	})
	vbox.Add(button)
}
