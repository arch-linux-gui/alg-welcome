package main

import (
	"os"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	screens "github.com/harshau007/alg-welcome/screens"
	"github.com/harshau007/alg-welcome/utils"
)

func main() {
	a := app.New()
	w := a.NewWindow("Welcome")

	imageBytes, err := os.ReadFile("./assets/alg.png")
	if err != nil {
		panic(err)
	}
	image := fyne.NewStaticResource("alg.png", imageBytes)
	w.SetIcon(image)

	screens.PaginationDots = utils.CreatePaginationDots(len(screens.Screens))

	screens.ShowFirstScreen(w, a)

	w.SetFixedSize(true)
	w.ShowAndRun()
}
