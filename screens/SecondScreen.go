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

func showSecondScreen(w fyne.Window, a fyne.App) {
	image := canvas.NewImageFromFile("./assets/gnome.jpg")
	image.FillMode = canvas.ImageFillContain
	image.SetMinSize(fyne.NewSize(550, 300))

	theme := utils.CheckTheme(a)
	backImage, _ := os.ReadFile("./assets/back-light.png")
	if theme == "light" {
		backImage, _ = os.ReadFile("./assets/back-dark.png")
	}

	backIcon := fyne.NewStaticResource("back.png", backImage)
	backButton := widget.NewButtonWithIcon("Back", backIcon, func() {
		if currentIndex > 0 {
			currentIndex--
			ShowFirstScreen(w, a)
			utils.UpdatePaginationDots(PaginationDots, currentIndex)
		}
	})

	nextImage, _ := os.ReadFile("./assets/next-light.png")
	if theme == "light" {
		nextImage, _ = os.ReadFile("./assets/next-dark.png")
	}
	nextIcon := fyne.NewStaticResource("next.png", nextImage)
	nextButton := widget.NewButtonWithIcon("Next", nextIcon, func() {
		if currentIndex < len(Screens)-1 {
			currentIndex++
			showThirdScreen(w, a)
			utils.UpdatePaginationDots(PaginationDots, currentIndex)
		}
	})
	nextButton.IconPlacement = widget.ButtonIconTrailingText

	text := canvas.NewText("This is Second Screen", color.White)
	text.Alignment = fyne.TextAlignCenter
	text.TextStyle = fyne.TextStyle{
		Bold:      true,
		Monospace: false,
	}
	text.TextSize = 24

	content := container.NewVBox(
		layout.NewSpacer(),
		// container.NewHBox(layout.NewSpacer(), image, layout.NewSpacer()),
		container.NewHBox(layout.NewSpacer(), text, layout.NewSpacer()),
		layout.NewSpacer(),
		container.NewHBox(backButton, layout.NewSpacer(), PaginationDots,layout.NewSpacer(), nextButton),
	)

	w.SetContent(container.NewBorder(nil, nil, nil, nil, content))
	w.Resize(fyne.NewSize(600, 400))
}