package screens

import (
	"image/color"
	"os"
	"github.com/harshau007/alg-welcome/utils"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
)

var currentIndex = 0

var Screens = [3]string{} // Numbers of Screens

var PaginationDots *fyne.Container


func ShowFirstScreen(w fyne.Window, a fyne.App) {
	image := canvas.NewImageFromFile("./assets/alg.png")
	image.FillMode = canvas.ImageFillContain
	image.SetMinSize(fyne.NewSize(150, 150))

	theme := utils.CheckTheme(a)
	textColor := color.White
	if theme == "light" {
		textColor = color.Black
	}

	text := canvas.NewText("Welcome to ALG", textColor)
	text.Alignment = fyne.TextAlignCenter
	text.TextStyle = fyne.TextStyle{
		Bold:      true,
		Monospace: false,
	}
	text.TextSize = 24

	nextImage, _ := os.ReadFile("./assets/next-light.png")
	if theme == "light" {
		nextImage, _ = os.ReadFile("./assets/next-dark.png")
	}

	nextIcon := fyne.NewStaticResource("next.png", nextImage)
	nextButton := widget.NewButtonWithIcon("Next", nextIcon, func() {
		if currentIndex < len(Screens)-1 {
			currentIndex++
			showSecondScreen(w, a)
			utils.UpdatePaginationDots(PaginationDots, currentIndex)
		}
	})
	nextButton.IconPlacement = widget.ButtonIconTrailingText

	skipImage, _ := os.ReadFile("./assets/skip-light.png")
	if theme == "light" {
		skipImage, _ = os.ReadFile("./assets/skip-dark.png")
	}

	skipIcon := fyne.NewStaticResource("skip.png", skipImage)
	skipButton := widget.NewButtonWithIcon("Skip", skipIcon, func() {
		if currentIndex < len(Screens)-1 {
			currentIndex += 2
			showThirdScreen(w, a)
			utils.UpdatePaginationDots(PaginationDots, currentIndex)
		}
	})
	skipButton.IconPlacement = widget.ButtonIconTrailingText
	
	content := container.NewVBox(
		layout.NewSpacer(),
		container.NewHBox(layout.NewSpacer(), image, layout.NewSpacer()),
		container.NewHBox(layout.NewSpacer(), text, layout.NewSpacer()),
		layout.NewSpacer(),
		container.NewHBox(skipButton, layout.NewSpacer(), PaginationDots,layout.NewSpacer(), nextButton),
	)

	w.SetContent(container.NewBorder(nil, nil, nil, nil, content))
	w.Resize(fyne.NewSize(600, 400))
}