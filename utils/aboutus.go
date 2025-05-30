package utils

import (
	"fmt"

	"github.com/gotk3/gotk3/gtk"
)

var aboutUsDialog *gtk.Dialog

func AboutUs(parentWindow *gtk.Window) {
	if aboutUsDialog != nil {
		aboutUsDialog.Present()
		return
	}

	dialog, err := gtk.DialogNew()
	if err != nil {
		fmt.Println("Error creating dialog:", err)
		return
	}
	aboutUsDialog = dialog
	dialog.SetTitle("About Us")
	dialog.SetTransientFor(parentWindow)
	dialog.SetSizeRequest(400, 300)
	dialog.SetResizable(false)
	dialog.SetModal(true)
	dialog.Connect("delete-event", func() bool {
		dialog.Hide()
		return true
	})
	dialog.Connect("response", func(d *gtk.Dialog, _ gtk.ResponseType) {
		d.Hide()
	})

	content, _ := dialog.GetContentArea()
	content.SetSpacing(10)
	content.SetMarginTop(20)
	content.SetMarginBottom(20)
	content.SetMarginStart(20)
	content.SetMarginEnd(20)

	appName, _ := gtk.LabelNew("<span size='xx-large' weight='bold'>Arka Linux GUI</span>")
	appName.SetUseMarkup(true)
	appName.SetHAlign(gtk.ALIGN_CENTER)
	content.Add(appName)

	version, _ := gtk.LabelNew("Version 1.0.0")
	version.SetHAlign(gtk.ALIGN_CENTER)
	content.Add(version)

	description, _ := gtk.LabelNew(
		"Welcome to Arka Linux GUI, formerly Arch Linux GUI. We simplify Arch Linux installation with a fast, offline graphical installer. Our mission is to make Arch accessible to both beginners and power users.",
	)
	description.SetLineWrap(true)
	description.SetMaxWidthChars(60)
	description.SetJustify(gtk.JUSTIFY_FILL)
	content.Add(description)

	devLabel, _ := gtk.LabelNew("<span weight='bold'>Developers:</span>")
	devLabel.SetUseMarkup(true)
	devLabel.SetHAlign(gtk.ALIGN_START)
	content.Add(devLabel)

	devNames, _ := gtk.LabelNew(
		"• DemonKiller (Core Team)\n" +
			"• Akash6222 (Core Team)\n" +
			"• harshau007 (Core Team)",
	)
	devNames.SetLineWrap(true)
	devNames.SetJustify(gtk.JUSTIFY_LEFT)
	content.Add(devNames)

	website, _ := gtk.LinkButtonNewWithLabel(
		"https://www.arkalinuxgui.org", "Visit our website",
	)
	website.SetHAlign(gtk.ALIGN_CENTER)
	content.Add(website)

	license, _ := gtk.LabelNew("Distributed under the MIT License.")
	license.SetLineWrap(true)
	license.SetMaxWidthChars(60)
	license.SetJustify(gtk.JUSTIFY_CENTER)
	content.Add(license)

	dialog.ShowAll()
}
