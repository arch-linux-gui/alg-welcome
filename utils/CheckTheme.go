package utils

import (
	"image/color"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"
)

func CheckTheme(a fyne.App) string {
	bgColor := a.Settings().Theme().Color(theme.ColorNameBackground, theme.VariantLight)
	if bgColor == color.White {
		return "light"
	}
	return "dark"
}