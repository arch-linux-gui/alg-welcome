# Maintainer: Harsh Upadhyay <amanupadhyay2004@gmail.com>
pkgname=alg-welcome
pkgver=1.3
pkgrel=2
pkgdesc="Welcome App For ALG (GTK Version)"
arch=('x86_64')
url="https://github.com/arch-linux-gui/alg-welcome#branch=2.0/gotk"
license=('GPL3')
depends=('gtk3' 'glib2' 'cairo')
makedepends=('go' 'git')
source=(${pkgname}::"git+${url}")
sha256sums=('SKIP')

prepare() {
  cd "$pkgname"
  mkdir -p build
}

build() {
  cd "$pkgname"

  export CGO_CPPFLAGS="$CPPFLAGS"
  export CGO_CFLAGS="$CFLAGS"
  export CGO_CXXFLAGS="$CXXFLAGS"
  export CGO_LDFLAGS="$LDFLAGS"
  export GOFLAGS="-buildmode=pie -trimpath -ldflags=-linkmode=external -mod=readonly -modcacherw"

  go build -o build/welcome ./
}

package() {
  cd "$pkgname"

  # Binary
  install -Dm755 "build/welcome" "$pkgdir/usr/bin/welcome"

  # Desktop file
  install -Dm644 "assets/welcome.desktop" "$pkgdir/usr/share/applications/welcome.desktop"

  # Icon
  install -Dm644 "assets/welcome.png" "$pkgdir/usr/share/pixmaps/welcome.png"
}