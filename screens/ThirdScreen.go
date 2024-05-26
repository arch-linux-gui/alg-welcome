package screens

import (
	"image/color"
	"os"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	"github.com/harshau007/alg-welcome/utils"
)

func showThirdScreen(w fyne.Window, a fyne.App) {
	theme := utils.CheckTheme(a)
	backImage, _ := os.ReadFile("./assets/back-light.png")
	if theme == "light" {
		backImage, _ = os.ReadFile("./assets/back-dark.png")
	}

	backIcon := fyne.NewStaticResource("back.png", backImage)
	backButton := widget.NewButtonWithIcon("Back", backIcon, func() {
		if currentIndex > 0 {
			currentIndex -= 1
			showSecondScreen(w, a)
			utils.UpdatePaginationDots(PaginationDots, currentIndex)
		}
	})

	finishImage, _ := os.ReadFile("./assets/finish-light.png")
	if theme == "light" {
		finishImage, _ = os.ReadFile("./assets/finish-dark.png")
	}
	finishIcon := fyne.NewStaticResource("finish.png", finishImage)
	finishButton := widget.NewButtonWithIcon("Finish", finishIcon, func() {
		w.Close()
	})
	finishButton.IconPlacement = widget.ButtonIconTrailingText

	text := canvas.NewText("Thank You!", color.White)
	text.Alignment = fyne.TextAlignCenter
	text.TextStyle = fyne.TextStyle{
		Bold:      true,
		Monospace: false,
	}
	text.TextSize = 48

	content := container.NewVBox(
		layout.NewSpacer(),
		container.NewHBox(layout.NewSpacer(), text, layout.NewSpacer()),
		layout.NewSpacer(),
		container.NewHBox(backButton, layout.NewSpacer(), PaginationDots, layout.NewSpacer(), finishButton),
	)

	w.SetContent(container.NewBorder(nil, nil, nil, nil, content))
	w.Resize(fyne.NewSize(600, 400))
}