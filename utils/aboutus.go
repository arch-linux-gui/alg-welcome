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
	dialog.SetSizeRequest(400, 200)
	dialog.SetTransientFor(parentWindow)
	dialog.SetModal(true)

	content, _ := dialog.GetContentArea()
	content.SetSpacing(10)
	content.SetMarginTop(20)
	content.SetMarginBottom(20)
	content.SetMarginStart(20)
	content.SetMarginEnd(10)

	appName, _ := gtk.LabelNew("ALG Welcome")
	appName.SetMarkup("<span size='x-large' weight='bold'>ALG Welcome</span>")
	content.Add(appName)

	// Version
	version, _ := gtk.LabelNew("Version 1.0.0")
	content.Add(version)

	// Description
	description, _ := gtk.LabelNew("Your application description goes here. This can be a brief overview of what your application does and its main features.")
	description.SetLineWrap(true)
	description.SetMaxWidthChars(50)
	description.SetJustify(gtk.JUSTIFY_CENTER)
	content.Add(description)

	// Developers
	developers, _ := gtk.LabelNew("")
	developers.SetMarkup("<span weight='bold'>Developers:</span>")
	content.Add(developers)

	devNames, _ := gtk.LabelNew("Developer 1\nDeveloper 2\nDeveloper 3")
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
