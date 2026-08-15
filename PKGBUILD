# Maintainer: Arka Linux GUI <https://www.arkalinuxgui.org>
pkgname=archer
pkgver=4.1.8
pkgrel=1
pkgdesc="ALG Welcome App - onboarding and maintenance utility for Arch Linux GUI"
arch=('x86_64')
url="https://github.com/arch-linux-gui/alg-welcome"
license=('MIT')

# `depends` covers only what's needed regardless of desktop environment: pkexec (autostart
# removal, reflector), xdg-open (social links), reflector/sudo (mirrorlist update, system
# update), and this app's own Qt6/logging libraries. Desktop-environment-specific tools
# (theme switching, display settings, terminal emulators) are `optdepends` instead of
# `depends`, since a KDE install has no use for the Xfce/GNOME tooling and vice versa, and
# every supported ALG edition already ships its own DE's tools as part of the desktop
# install itself.
depends=(
    'qt6-base'
    'spdlog'
    'fmt'
    'polkit'    # pkexec
    'xdg-utils' # xdg-open
    'reflector'
    'sudo'
)
makedepends=('cmake' 'git')
checkdepends=('catch2')
optdepends=(
    'plasma-workspace: KDE Plasma theme switching and display settings (kcmshell6, qdbus6, lookandfeeltool, plasma-apply-colorscheme)'
    'kconfig: KDE Plasma theme switching (kwriteconfig6)'
    'konsole: KDE Plasma system-update terminal'
    'gnome-control-center: GNOME display settings'
    'gnome-console: GNOME system-update terminal (kgx)'
    'glib2: GNOME theme switching (gsettings)'
    'xfconf: Xfce theme switching (xfconf-query)'
    'xfce4-settings: Xfce display settings (xfce4-display-settings)'
    'xfce4-terminal: Xfce system-update terminal'
)

source=("archer::git+https://github.com/arch-linux-gui/alg-welcome.git#tag=v$pkgver")
sha256sums=('SKIP')

# The `VERSION` file (see project_management.md) is this project's single source of truth
# for its version, so derive pkgver from it directly instead of a git-describe heuristic.
pkgver() {
    cd "$srcdir/archer"
    tr -d '[:space:]' <VERSION
}

build() {
    cmake -B build -S "$srcdir/archer" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTS=ON
    cmake --build build
}

check() {
    ctest --test-dir build --output-on-failure
}

package() {
    DESTDIR="$pkgdir" cmake --install build
    install -Dm644 "$srcdir/archer/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
