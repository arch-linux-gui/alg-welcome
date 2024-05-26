package utils

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/theme"
)

func UpdatePaginationDots(dots *fyne.Container, currentIndex int) {
	for i, dot := range dots.Objects[1 : len(dots.Objects)-1] {
		if circle, ok := dot.(*canvas.Circle); ok {
			if i == currentIndex {
				circle.FillColor = theme.PrimaryColor()
			} else {
				circle.FillColor = theme.ForegroundColor()
			}
			circle.Refresh()
		}
	}
}