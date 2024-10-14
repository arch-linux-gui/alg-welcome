package utils

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os/exec"
	"regexp"
	"strings"
	"sync"

	"github.com/gotk3/gotk3/gdk"
	"github.com/gotk3/gotk3/glib"
	"github.com/gotk3/gotk3/gtk"
	"github.com/gotk3/gotk3/pango"
)

var (
	mirrorListDialog  *gtk.Dialog
	logDialog         *gtk.Dialog
	logTreeView       *gtk.TreeView
	updateButton      *gtk.Button
	selectedCountries []string
	isUpdating        bool
	logMutex          sync.Mutex

	// Variables to store initial state
	initialProtocol   string
	initialSortBy     string
	initialMaxMirrors int
	initialTimeout    int
)

func MirrorList(parentWindow *gtk.Window) {
	if mirrorListDialog != nil {
		log.Println("MirrorList window is already open!")
		return
	}

	// Main dialog
	dialog, err := gtk.DialogNew()
	if err != nil {
		log.Println("Error creating dialog:", err)
		return
	}
	mirrorListDialog = dialog
	dialog.SetTitle("Update MirrorList")
	dialog.SetDefaultSize(500, 400)
	dialog.SetTransientFor(parentWindow)
	dialog.SetModal(true)

	content, _ := dialog.GetContentArea()
	content.SetSpacing(10)
	content.SetMarginTop(10)
	content.SetMarginBottom(10)
	content.SetMarginStart(10)
	content.SetMarginEnd(10)

	// Countries selection (using checkboxes in a 2x5 grid)
	countryFrame, _ := gtk.FrameNew("Countries")
	content.Add(countryFrame)

	countryGrid, _ := gtk.GridNew()
	countryGrid.SetColumnSpacing(10)
	countryGrid.SetRowSpacing(5)
	countryFrame.Add(countryGrid)

	countries := []string{"United States", "Brazil", "Japan", "Sweden", "France", "Norway", "India", "Australia", "China", "United Kingdom"}
	for i, country := range countries {
		checkButton, _ := gtk.CheckButtonNewWithLabel(country)
		checkButton.SetMarginBottom(3)
		checkButton.SetMarginStart(3)
		checkButton.SetCanFocus(false)
		checkButton.Connect("toggled", func(btn *gtk.CheckButton) {
			onCountryToggled(btn)
		})
		countryGrid.Attach(checkButton, i%2, i/2, 1, 1)
	}

	// Protocol selection (using radio buttons)
	protocolFrame, _ := gtk.FrameNew("Protocols")
	content.Add(protocolFrame)

	protocolBox, _ := gtk.BoxNew(gtk.ORIENTATION_HORIZONTAL, 10)
	protocolFrame.Add(protocolBox)

	httpsCheck, _ := gtk.CheckButtonNewWithLabel("HTTPS")
	httpsCheck.SetMarginBottom(5)
	httpsCheck.SetMarginStart(5)
	httpsCheck.SetActive(true)
	httpsCheck.SetCanFocus(false)

	httpCheck, _ := gtk.CheckButtonNewWithLabel("HTTP")
	httpCheck.SetMarginBottom(5)
	httpCheck.SetMarginStart(5)
	httpCheck.SetCanFocus(false)

	protocolBox.PackStart(httpsCheck, true, true, 0)
	protocolBox.PackStart(httpCheck, true, true, 0)

	// Sorting options
	sortByFrame, _ := gtk.FrameNew("Sort By")
	content.Add(sortByFrame)

	sortByCombo, _ := gtk.ComboBoxTextNew()
	sortOptions := []string{"Rate", "Age", "Score", "Delay", "Country"}
	for _, option := range sortOptions {
		sortByCombo.AppendText(option)
		sortByCombo.SetMarginBottom(3)
		sortByCombo.SetMarginStart(3)
	}
	sortByCombo.SetActive(0)
	sortByFrame.Add(sortByCombo)

	// Spin buttons for Max Number of Fresh Mirrors and Download Timeout
	settingsBox, _ := gtk.BoxNew(gtk.ORIENTATION_HORIZONTAL, 10)
	content.Add(settingsBox)

	mirrorLabel, _ := gtk.LabelNew("Max Fresh Mirrors:")
	settingsBox.PackStart(mirrorLabel, false, false, 0)
	mirrorSpin, _ := gtk.SpinButtonNewWithRange(1, 20, 1)
	mirrorSpin.SetValue(5)
	settingsBox.PackStart(mirrorSpin, false, false, 0)

	timeoutLabel, _ := gtk.LabelNew("Timeout (s):")
	settingsBox.PackStart(timeoutLabel, false, false, 0)
	timeoutSpin, _ := gtk.SpinButtonNewWithRange(5, 60, 1)
	timeoutSpin.SetValue(10)
	settingsBox.PackStart(timeoutSpin, false, false, 0)

	// Update Button
	updateButton, _ = gtk.ButtonNewWithLabel("Update")
	updateButton.SetSensitive(false)
	content.Add(updateButton)

	// Store initial state
	initialProtocol = "https"
	initialSortBy = sortByCombo.GetActiveText()
	initialMaxMirrors = mirrorSpin.GetValueAsInt()
	initialTimeout = timeoutSpin.GetValueAsInt()

	updateButton.Connect("clicked", func() {
		protocols := []string{"https"}
		if httpCheck.GetActive() {
			protocols = append(protocols, "http")
		}
		maxMirrors := mirrorSpin.GetValueAsInt()
		timeout := timeoutSpin.GetValueAsInt()
		sortBy := strings.ToLower(sortByCombo.GetActiveText())

		command := fmt.Sprintf(`pkexec reflector --country "%s" --protocol %s --latest %d --sort %s --download-timeout %d --save /etc/pacman.d/mirrorlist --verbose`,
			strings.Join(selectedCountries, ","), strings.Join(protocols, ","), maxMirrors, sortBy, timeout)

		showLogDialog(dialog)
		go startMirrorListUpdate(command)
	})

	dialog.Connect("response", func(dialog *gtk.Dialog, responseId gtk.ResponseType) {
		if responseId == gtk.RESPONSE_DELETE_EVENT {
			if isUpdating {
				dialog.Hide()
			} else {
				dialog.Destroy()
				mirrorListDialog = nil
			}
		}
	})

	dialog.ShowAll()
}

var lineCounter int = 0

func showLogDialog(parent *gtk.Dialog) {
	if logDialog != nil {
		logDialog.ShowAll()
		return
	}

	lineCounter = 0

	logDialog, _ = gtk.DialogNew()
	logDialog.SetTitle("Update Progress")
	logDialog.SetDefaultSize(600, 400)
	logDialog.SetTransientFor(parent)
	logDialog.SetModal(false)

	// Remove minimize, maximize, and close buttons
	logDialog.SetTypeHint(gdk.WINDOW_TYPE_HINT_DIALOG)
	logDialog.SetDeletable(false)

	content, _ := logDialog.GetContentArea()
	content.SetSpacing(10)
	content.SetMarginTop(10)
	content.SetMarginBottom(10)
	content.SetMarginStart(10)
	content.SetMarginEnd(10)

	scrolledWindow, _ := gtk.ScrolledWindowNew(nil, nil)
	scrolledWindow.SetMinContentHeight(400)
	content.Add(scrolledWindow)

	// Create TreeView
	logTreeView, _ = gtk.TreeViewNew()
	scrolledWindow.Add(logTreeView)

	// Create columns
	renderer, _ := gtk.CellRendererTextNew()
	renderer.SetProperty("wrap-width", 300)
	renderer.SetProperty("wrap-mode", pango.WRAP_WORD_CHAR)

	serverColumn, _ := gtk.TreeViewColumnNewWithAttribute("Server", renderer, "text", 0)
	serverColumn.SetExpand(true)
	logTreeView.AppendColumn(serverColumn)

	rateColumn, _ := gtk.TreeViewColumnNewWithAttribute("Rate", renderer, "text", 1)
	logTreeView.AppendColumn(rateColumn)

	timeColumn, _ := gtk.TreeViewColumnNewWithAttribute("Time", renderer, "text", 2)
	logTreeView.AppendColumn(timeColumn)

	// Create ListStore
	listStore, _ := gtk.ListStoreNew(glib.TYPE_STRING, glib.TYPE_STRING, glib.TYPE_STRING)
	logTreeView.SetModel(listStore)

	parentX, parentY := parent.GetPosition()
	parentWidth, _ := parent.GetSize()
	logDialog.Move(parentX+parentWidth+10, parentY)

	logDialog.ShowAll()
}

func startMirrorListUpdate(command string) {
	isUpdating = true
	updateButton.SetSensitive(false)
	cmd := exec.Command("sh", "-c", command)

	stdout, _ := cmd.StdoutPipe()
	stderr, _ := cmd.StderrPipe()

	cmd.Start()

	lineCounter = 0

	go streamLogs(stdout)
	go streamLogs(stderr)

	cmd.Wait()

	isUpdating = false
	updateButton.SetSensitive(true)
	appendLog("Update completed.", "", "")

	// Reset all changes
	glib.IdleAdd(func() {
		// resetChanges()
		if logDialog != nil {
			logDialog.Destroy()
			logDialog = nil
		}
	})
}

func streamLogs(pipe io.ReadCloser) {
	scanner := bufio.NewScanner(pipe)
	for scanner.Scan() {
		logLine := scanner.Text()
		fmt.Println("Received log line:", logLine)
		processLogLine(logLine)
	}
	if err := scanner.Err(); err != nil {
		fmt.Println("Error reading pipe:", err)
	}
}

func processLogLine(logLine string) {
	lineCounter++
	if lineCounter <= 2 {
		return
	}

	// Regular expression to match the log line format
	re := regexp.MustCompile(`^\[.*?\]\s+(INFO|WARNING):\s+(.+)$`)
	matches := re.FindStringSubmatch(logLine)

	if len(matches) == 3 {
		logType := matches[1]
		content := matches[2]

		if logType == "INFO" {
			serverRe := regexp.MustCompile(`^(https?://\S+)\s+(\S+\s+\S+/s)\s+(\S+\s+s)$`)
			serverMatches := serverRe.FindStringSubmatch(content)

			if len(serverMatches) == 4 {
				server := serverMatches[1]
				rate := serverMatches[2]
				time := serverMatches[3]
				appendLog(server, rate, time)
			} else {
				appendLog(content, "", "")
			}
		} else if logType == "WARNING" {
			appendLog(content, "WARNING", "N/A")
		}
	}
}

func appendLog(server, rate, time string) {
	logMutex.Lock()
	defer logMutex.Unlock()

	fmt.Printf("Appending log: Server: %s, Rate: %s, Time: %s\n", server, rate, time)

	glib.IdleAdd(func() {
		if logTreeView != nil {
			model, _ := logTreeView.GetModel()
			listStore := model.(*gtk.ListStore)
			iter := listStore.Append()
			listStore.Set(iter,
				[]int{0, 1, 2},
				[]interface{}{server, rate, time})

			// Scroll to the bottom of the TreeView
			path, _ := listStore.GetPath(iter)
			logTreeView.ScrollToCell(path, nil, true, 0.5, 0.5)
		} else {
			fmt.Println("logTreeView is nil")
		}
	})
}

func onCountryToggled(btn *gtk.CheckButton) {
	label, _ := btn.GetLabel()
	if btn.GetActive() {
		selectedCountries = append(selectedCountries, label)
	} else {
		for i, country := range selectedCountries {
			if country == label {
				selectedCountries = append(selectedCountries[:i], selectedCountries[i+1:]...)
				break
			}
		}
	}
	updateButton.SetSensitive(len(selectedCountries) > 0)
}
