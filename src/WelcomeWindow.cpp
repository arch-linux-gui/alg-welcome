#include "WelcomeWindow.h"
#include "AboutUsDialog.h"
#include "MirrorListDialog.h"
#include "utils/Extras.h"
#include "utils/Autostart.h"
#include "utils/Themes.h"
#include "utils/Resolution.h"
#include "utils/Updates.h"
#include "utils/Logger.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QPixmap>
#include <QIcon>
#include <QScreen>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>

WelcomeWindow::WelcomeWindow(QWidget *parent) 
    : QMainWindow(parent)
    , calamaresTimer(std::make_unique<QTimer>())
{
    // Get system information
    desktopEnv = Extras::getDesktopEnvironment();
    isLiveISO = Extras::checkIfLiveISO();
    
    // Setup UI
    setupWindow();
    applyStylesheet();
    setupUI();
    
    // Setup Calamares monitoring
    setupCalamaresMonitoring();
}

void WelcomeWindow::setupWindow() {
    setWindowTitle("ALG Welcome");
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Set window icon
    const QString iconPath = QDir::currentPath() + "/assets/welcome.png";
    if (QFile::exists(iconPath)) {
        setWindowIcon(QIcon(iconPath));
    } else {
        Logger::warning("Window icon not found at: " + iconPath);
    }
    
    // Center window on screen
    if (auto *screen = QApplication::primaryScreen()) {
        const auto screenGeometry = screen->geometry();
        const int x = (screenGeometry.width() - WINDOW_WIDTH) / 2;
        const int y = (screenGeometry.height() - WINDOW_HEIGHT) / 2;
        move(x, y);
    }
}

void WelcomeWindow::applyStylesheet() {
    // Try installed location first, then fall back to current directory
    QStringList paths = {
        "/usr/share/alg-welcome/styles.qss",
        QDir::currentPath() + "/styles.qss"
    };
    
    for (const QString &qssPath : paths) {
        QFile file(qssPath);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            const QString styleSheet = QString::fromUtf8(file.readAll());
            setStyleSheet(styleSheet);
            file.close();
            Logger::info("Loaded stylesheet from: " + qssPath);
            return;
        }
    }
    
    Logger::warning("Could not load stylesheet from any location");
}

void WelcomeWindow::setupUI() {
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    
    // Add sections
    addHeader(mainLayout);
    addInstallSetupSection(mainLayout);
    addSocialMediaSection(mainLayout);
    addMoreOptionsSection(mainLayout);
    addAboutUsSection(mainLayout);
}

void WelcomeWindow::addHeader(QVBoxLayout *layout) {
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);
    
    // Logo
    const QString logoPath = QDir::currentPath() + "/assets/welcome.png";
    if (QFile::exists(logoPath)) {
        auto *logoLabel = new QLabel();
        QPixmap pixmap(logoPath);
        auto scaledPixmap = pixmap.scaled(LOGO_SIZE, LOGO_SIZE, 
                                          Qt::KeepAspectRatio, 
                                          Qt::SmoothTransformation);
        logoLabel->setPixmap(scaledPixmap);
        headerLayout->addWidget(logoLabel);
    }
    
    // Welcome text
    auto *welcomeLabel = new QLabel("Welcome to ALG!");
    welcomeLabel->setObjectName("header");
    welcomeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    headerLayout->addWidget(welcomeLabel, 1);
    
    layout->addLayout(headerLayout);
}

void WelcomeWindow::addInstallSetupSection(QVBoxLayout *layout) {
    auto *sectionLabel = new QLabel("Install & Setup");
    sectionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(sectionLabel);
    
    auto *grid = new QGridLayout();
    grid->setSpacing(10);
    
    // Define buttons based on whether it's Live ISO
    if (isLiveISO) {
        // Row 0
        installButton = createButtonWithIcon("Install ALG ", "system-software-install", false);
        connect(installButton, &QPushButton::clicked, this, &WelcomeWindow::onInstallAlg);
        grid->addWidget(installButton, 0, 0);
        
        auto *resolutionBtn = createButtonWithIcon("Screen Resolution ", "video-display", false);
        connect(resolutionBtn, &QPushButton::clicked, this, &WelcomeWindow::onScreenResolution);
        grid->addWidget(resolutionBtn, 0, 1);
        
        // Row 1
        updateSystemButton = createButtonWithIcon("Update System ", "system-software-update", false);
        connect(updateSystemButton, &QPushButton::clicked, this, &WelcomeWindow::onUpdateSystem);
        grid->addWidget(updateSystemButton, 1, 0);
        
        updateMirrorlistButton = createButtonWithIcon("Update Mirrorlist ", "view-refresh", false);
        connect(updateMirrorlistButton, &QPushButton::clicked, this, &WelcomeWindow::onUpdateMirrorlist);
        grid->addWidget(updateMirrorlistButton, 1, 1);
    } else {
        // Row 0
        auto *appStoreBtn = createButtonWithIcon("Launch App Store ", "system-software-install", false);
        connect(appStoreBtn, &QPushButton::clicked, this, &WelcomeWindow::onLaunchAppStore);
        grid->addWidget(appStoreBtn, 0, 0);
        
        auto *resolutionBtn = createButtonWithIcon("Screen Resolution ", "video-display", false);
        connect(resolutionBtn, &QPushButton::clicked, this, &WelcomeWindow::onScreenResolution);
        grid->addWidget(resolutionBtn, 0, 1);
        
        // Row 1
        updateSystemButton = createButtonWithIcon("Update System ", "system-software-update", false);
        connect(updateSystemButton, &QPushButton::clicked, this, &WelcomeWindow::onUpdateSystem);
        grid->addWidget(updateSystemButton, 1, 0);
        
        updateMirrorlistButton = createButtonWithIcon("Update Mirrorlist ", "view-refresh", false);
        connect(updateMirrorlistButton, &QPushButton::clicked, this, &WelcomeWindow::onUpdateMirrorlist);
        grid->addWidget(updateMirrorlistButton, 1, 1);
    }
    
    layout->addLayout(grid);
    layout->addStretch();
}

void WelcomeWindow::addSocialMediaSection(QVBoxLayout *layout) {
    auto *sectionLabel = new QLabel("Social Media Links");
    sectionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(sectionLabel);
    
    auto *grid = new QGridLayout();
    grid->setSpacing(10);
    
    // GitHub button
    auto *githubBtn = createButtonWithIcon("GitHub  ", "assets/github.svg", true);
    connect(githubBtn, &QPushButton::clicked, []() {
        Extras::openUrl("https://github.com/arch-linux-gui");
    });
    grid->addWidget(githubBtn, 0, 0);
    
    // Discord button
    auto *discordBtn = createButtonWithIcon("Discord ", "assets/discord.svg", true);
    connect(discordBtn, &QPushButton::clicked, []() {
        Extras::openUrl("https://discord.gg/NgAFEw9Tkf");
    });
    grid->addWidget(discordBtn, 0, 1);
    
    layout->addLayout(grid);
}

void WelcomeWindow::addMoreOptionsSection(QVBoxLayout *layout) {
    auto *sectionLabel = new QLabel("More Options");
    sectionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(sectionLabel);
    
    auto *hbox = new QHBoxLayout();
    hbox->setSpacing(10);
    
    // Autostart toggle
    auto *autostartWidget = createSwitchWithLabel("AutoStart:", &autostartSwitch);
    autostartSwitch->setChecked(Autostart::checkFileExists());
    connect(autostartSwitch, &QCheckBox::toggled, this, &WelcomeWindow::onAutostartToggled);
    hbox->addWidget(autostartWidget);
    
    // Dark theme toggle
    auto *darkthemeWidget = createSwitchWithLabel("Dark Theme:", &themeSwitch);
    const auto currentTheme = Themes::getCurrentTheme(desktopEnv);
    themeSwitch->setChecked(Themes::isDarkTheme(currentTheme));
    connect(themeSwitch, &QCheckBox::toggled, this, &WelcomeWindow::onThemeToggled);
    hbox->addWidget(darkthemeWidget);
    
    layout->addLayout(hbox);
}

void WelcomeWindow::addAboutUsSection(QVBoxLayout *layout) {
    auto *aboutButton = new QPushButton("About Us");
    aboutButton->setFocusPolicy(Qt::NoFocus);
    connect(aboutButton, &QPushButton::clicked, this, &WelcomeWindow::onAboutUs);
    layout->addWidget(aboutButton);
}

QPushButton* WelcomeWindow::createButtonWithIcon(const QString &label, 
                                                  const QString &iconName, 
                                                  bool fromFile) {
    auto *button = new QPushButton(label);
    button->setFocusPolicy(Qt::NoFocus);
    
    // Set icon
    if (fromFile) {
        const QString iconPath = QDir::currentPath() + "/" + iconName;
        if (QFile::exists(iconPath)) {
            button->setIcon(QIcon(iconPath));
            button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
        }
    } else {
        // Use system theme icon
        button->setIcon(QIcon::fromTheme(iconName));
        button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    }
    
    button->setLayoutDirection(Qt::RightToLeft); // Icon on right
    return button;
}

QWidget* WelcomeWindow::createSwitchWithLabel(const QString &labelText, 
                                               QCheckBox **switchOut) {
    auto *widget = new QWidget();
    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    
    auto *label = new QLabel(labelText);
    auto *checkbox = new QCheckBox();
    checkbox->setFocusPolicy(Qt::NoFocus);
    
    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(checkbox);
    
    *switchOut = checkbox;
    return widget;
}

void WelcomeWindow::setupCalamaresMonitoring() {
    if (!isLiveISO || !installButton) {
        return;
    }
    
    // Create timer to check Calamares status every 2 seconds
    connect(calamaresTimer.get(), &QTimer::timeout, 
            this, &WelcomeWindow::checkCalamaresStatus);
    calamaresTimer->start(2000); // Check every 2 seconds
    
    // Initial check
    checkCalamaresStatus();
}

void WelcomeWindow::checkCalamaresStatus() {
    if (!installButton) {
        return;
    }
    
    const bool isRunning = Extras::isCalamaresRunning();
    
    installButton->setEnabled(!isRunning);
    if (isRunning) {
        installButton->setText("Installing... ");
    } else {
        installButton->setText("Install ALG ");
    }
}

// Slots
void WelcomeWindow::onInstallAlg() {
    Extras::runCalamaresIfLiveISO(isLiveISO);
}

void WelcomeWindow::onScreenResolution() {
    Resolution::screenResolution(desktopEnv);
}

void WelcomeWindow::onUpdateSystem() {
    Updates::updateSystem(desktopEnv);
}

void WelcomeWindow::onUpdateMirrorlist() {
    if (!mirrorListDialog || !mirrorListDialog->isVisible()) {
        mirrorListDialog = new MirrorListDialog(this);
        
        // Disable buttons when dialog opens
        if (updateSystemButton) {
            updateSystemButton->setEnabled(false);
        }
        if (updateMirrorlistButton) {
            updateMirrorlistButton->setEnabled(false);
        }
        
        // Re-enable buttons when dialog closes
        connect(mirrorListDialog, &QDialog::finished, 
                this, &WelcomeWindow::onMirrorlistDialogClosed);
        
        mirrorListDialog->show();
    } else {
        mirrorListDialog->activateWindow();
        mirrorListDialog->raise();
    }
}

void WelcomeWindow::onMirrorlistDialogClosed() {
    if (updateSystemButton) {
        updateSystemButton->setEnabled(true);
    }
    if (updateMirrorlistButton) {
        updateMirrorlistButton->setEnabled(true);
    }
}

void WelcomeWindow::onLaunchAppStore() {
    QProcess::startDetached("alg-app-store", QStringList());
}

void WelcomeWindow::onTutorials() {
    Extras::openUrl("https://arkalinuxgui.org/tutorials");
}

void WelcomeWindow::onAutostartToggled(bool checked) {
    Autostart::toggleAutostart(checked);
}

void WelcomeWindow::onThemeToggled(bool checked) {
    Themes::toggleTheme(checked, desktopEnv);
}

void WelcomeWindow::onAboutUs() {
    AboutUsDialog dialog(this);
    dialog.exec();
}
