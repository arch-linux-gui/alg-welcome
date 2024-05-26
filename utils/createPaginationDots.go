package utils

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/theme"
)

func CreatePaginationDots(total int) *fyne.Container {
	dots := make([]*canvas.Circle, total)
	for i := 0; i < total; i++ {
		dot := canvas.NewCircle(theme.ForegroundColor())
		dot.MinSize().Min(fyne.NewSize(40, 40)) 
		dot.StrokeWidth = 3
		dots[i] = dot
	}

	dotContainer := container.NewHBox(layout.NewSpacer())
	for _, dot := range dots {
		dotContainer.Add(dot)
	}
	dotContainer.Add(layout.NewSpacer())

	UpdatePaginationDots(dotContainer, 0)
	return dotContainer
}