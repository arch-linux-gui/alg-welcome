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
	dialog.SetSizeRequest(400, 300)
	dialog.SetTransientFor(parentWindow)
	dialog.SetModal(true)

	content, _ := dialog.GetContentArea()
	content.SetSpacing(10)
	content.SetMarginTop(20)
	content.SetMarginBottom(20)
	content.SetMarginStart(20)
	content.SetMarginEnd(20)

	// Application Name
	appName, _ := gtk.LabelNew("Arka Linux GUI")
	appName.SetMarkup("<span size='x-large' weight='bold'>Arka Linux GUI</span>")
	content.Add(appName)

	// Version
	version, _ := gtk.LabelNew("Version 1.0.0")
	content.Add(version)

	// Description with mission and history summary
	description, _ := gtk.LabelNew("Welcome to Arka Linux GUI, formerly known as Arch Linux GUI. We provide a fast, offline Arch installer with a graphical user interface. Our mission is to simplify the installation process of Arch Linux, making it accessible to everyone—from beginners to advanced users. Originally launched as Arch Linux GUI, we rebranded to better reflect our vision and goals.")
	description.SetLineWrap(true)
	description.SetMaxWidthChars(50)
	description.SetJustify(gtk.JUSTIFY_CENTER)
	content.Add(description)

	// Developers
	developers, _ := gtk.LabelNew("")
	developers.SetMarkup("<span weight='bold'>Developers:</span>")
	content.Add(developers)

	// Listing team members with roles
	devNames, _ := gtk.LabelNew("DemonKiller (Core Team Developer)\nAkash6222 (Core Team Developer)\nharshau007 (Core Team Developer)")
	devNames.SetLineWrap(true)
	devNames.SetJustify(gtk.JUSTIFY_CENTER)
	content.Add(devNames)

	// Website
	website, _ := gtk.LabelNew("")
	website.SetMarkup("<a href='https://www.arkalinuxgui.org'>Visit our website</a>")
	website.SetUseMarkup(true)
	content.Add(website)

	// License
	license, _ := gtk.LabelNew("This software is distributed under the MIT License.")
	license.SetLineWrap(true)
	license.SetMaxWidthChars(50)
	license.SetJustify(gtk.JUSTIFY_CENTER)
	content.Add(license)

	dialog.ShowAll()
}
