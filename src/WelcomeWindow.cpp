#include "WelcomeWindow.h"
#include "dialogs/AboutPage.h"
#include "dialogs/MirrorlistPage.h"
#include "dialogs/ThemePage.h"
#include "utils/Autostart.h"
#include "utils/Extras.h"
#include "utils/Resolution.h"
#include "utils/Updates.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTime>
#include <QVBoxLayout>

WelcomeWindow::WelcomeWindow( QWidget* parent )
    : QMainWindow( parent )
    , calamaresTimer( std::make_unique< QTimer >() )
{
    // Setup search paths for assets
    QDir::setSearchPaths( "assets",
                          {
                              "/usr/share/archer/assets",
                              QDir::currentPath() + "/assets",
                              QDir::currentPath() + "/../assets"  // Support running from build directory
                          } );

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

QString
WelcomeWindow::resolveExistingPath( const QStringList& candidates )
{
    for ( const QString& path : candidates )
    {
        if ( QFile::exists( path ) )
        {
            return path;
        }
    }
    return QString();
}

void
WelcomeWindow::setupWindow()
{
    setWindowTitle( "Archer" );
    setMinimumSize( WINDOW_WIDTH, WINDOW_HEIGHT );
    resize( WINDOW_WIDTH, WINDOW_HEIGHT );

    // Set window icon - try installed location first, then fall back to current directory
    const QString iconPath
        = resolveExistingPath( { "/usr/share/pixmaps/archer.png", QDir::currentPath() + "/assets/archer.png" } );
    if ( !iconPath.isEmpty() )
    {
        setWindowIcon( QIcon( iconPath ) );
    }

    // Center window on screen
    if ( auto* screen = QApplication::primaryScreen() )
    {
        const auto screenGeometry = screen->geometry();
        const int x = ( screenGeometry.width() - WINDOW_WIDTH ) / 2;
        const int y = ( screenGeometry.height() - WINDOW_HEIGHT ) / 2;
        move( x, y );
    }
}

void
WelcomeWindow::applyStylesheet()
{
    // Try installed location first, then fall back to current directory
    const QString qssPath
        = resolveExistingPath( { "/usr/share/archer/styles.qss", QDir::currentPath() + "/styles.qss" } );

    QFile file( qssPath );
    if ( !qssPath.isEmpty() && file.open( QFile::ReadOnly | QFile::Text ) )
    {
        const QString styleSheet = QString::fromUtf8( file.readAll() );
        setStyleSheet( styleSheet );
        file.close();
        spdlog::debug( "Loaded stylesheet from: {}", qssPath.toStdString() );
        return;
    }

    spdlog::warn( "Could not load stylesheet from any location" );
}

void
WelcomeWindow::setupUI()
{
    pages = new QStackedWidget( this );
    setCentralWidget( pages );

    pages->addWidget( buildHomePage() );

    mirrorlistPage = new MirrorlistPage( this );
    connect( mirrorlistPage, &MirrorlistPage::backRequested, this, &WelcomeWindow::goHome );
    connect( mirrorlistPage, &MirrorlistPage::toastRequested, this, &WelcomeWindow::showToast );
    pages->addWidget( mirrorlistPage );

    themePage = new ThemePage( desktopEnv, this );
    connect( themePage, &ThemePage::backRequested, this, &WelcomeWindow::goHome );
    connect( themePage, &ThemePage::toastRequested, this, &WelcomeWindow::showToast );
    pages->addWidget( themePage );

    aboutPage = new AboutPage( this );
    connect( aboutPage, &AboutPage::backRequested, this, &WelcomeWindow::goHome );
    connect( aboutPage, &AboutPage::toastRequested, this, &WelcomeWindow::showToast );
    pages->addWidget( aboutPage );

    // Floating toast, overlaid on the whole window rather than the current page so it survives
    // navigation between pages.
    toastLabel = new QLabel( this );
    toastLabel->setObjectName( "toast" );
    toastLabel->hide();
    toastLabel->setAttribute( Qt::WA_TransparentForMouseEvents );

    toastTimer = new QTimer( this );
    toastTimer->setSingleShot( true );
    connect( toastTimer, &QTimer::timeout, toastLabel, &QLabel::hide );
}

QWidget*
WelcomeWindow::buildHomePage()
{
    auto* homePage = new QWidget();

    auto* mainLayout = new QVBoxLayout( homePage );
    mainLayout->setSpacing( 10 );
    mainLayout->setContentsMargins( 20, 12, 20, 12 );

    addHeader( mainLayout );
    addBasicUtilitiesSection( mainLayout );
    addProjectInformationSection( mainLayout );
    addLogSection( mainLayout );

    return homePage;
}

void
WelcomeWindow::addHeader( QVBoxLayout* layout )
{
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing( 12 );

    // Logo - try installed location first, then fall back to current directory
    const QString logoPath
        = resolveExistingPath( { "/usr/share/pixmaps/archer.png", QDir::currentPath() + "/assets/archer.png" } );
    if ( !logoPath.isEmpty() )
    {
        auto* logoLabel = new QLabel();
        QPixmap pixmap( logoPath );
        auto scaledPixmap = pixmap.scaled( LOGO_SIZE, LOGO_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation );
        logoLabel->setPixmap( scaledPixmap );
        headerLayout->addWidget( logoLabel );
    }

    auto* titleBox = new QVBoxLayout();
    titleBox->setSpacing( 0 );

    auto* titleLabel = new QLabel( "Archer" );
    titleLabel->setObjectName( "header" );
    titleBox->addWidget( titleLabel );

    auto* subtitleLabel = new QLabel( "Welcome & onboarding for ALG" );
    subtitleLabel->setObjectName( "subtitle" );
    titleBox->addWidget( subtitleLabel );

    headerLayout->addLayout( titleBox, 1 );

    autostartSwitch = new QCheckBox( "AutoStart" );
    autostartSwitch->setFocusPolicy( Qt::NoFocus );
    autostartSwitch->setChecked( Autostart::checkFileExists() );
    connect( autostartSwitch, &QCheckBox::toggled, this, &WelcomeWindow::onAutostartToggled );
    headerLayout->addWidget( autostartSwitch );

    layout->addLayout( headerLayout );
}

void
WelcomeWindow::addBasicUtilitiesSection( QVBoxLayout* layout )
{
    auto* sectionLabel = new QLabel( "Basic Utilities" );
    sectionLabel->setObjectName( "sectionLabel" );
    sectionLabel->setAlignment( Qt::AlignCenter );
    layout->addWidget( sectionLabel );

    auto* grid = new QGridLayout();
    grid->setSpacing( 10 );

    // Row 0 - differs based on whether it's Live ISO
    if ( isLiveISO )
    {
        installButton = createButtonWithIcon( "Install ALG ", "system-software-install", false );
        connect( installButton, &QPushButton::clicked, this, &WelcomeWindow::onInstallAlg );
        grid->addWidget( installButton, 0, 0 );
    }
    else
    {
        auto* appStoreBtn = createButtonWithIcon( "Launch App Store ", "system-software-install", false );
        connect( appStoreBtn, &QPushButton::clicked, this, &WelcomeWindow::onLaunchAppStore );
        grid->addWidget( appStoreBtn, 0, 0 );
    }

    auto* resolutionBtn = createButtonWithIcon( "Screen Resolution ", "video-display", false );
    connect( resolutionBtn, &QPushButton::clicked, this, &WelcomeWindow::onScreenResolution );
    grid->addWidget( resolutionBtn, 0, 1 );

    // Row 1
    auto* syncBtn = createButtonWithIcon( "Sync Repositories ", "package-x-generic", false );
    connect( syncBtn, &QPushButton::clicked, this, &WelcomeWindow::onSyncRepositories );
    grid->addWidget( syncBtn, 1, 0 );

    auto* updateSystemBtn = createButtonWithIcon( "Update System ", "system-software-update", false );
    connect( updateSystemBtn, &QPushButton::clicked, this, &WelcomeWindow::onUpdateSystem );
    grid->addWidget( updateSystemBtn, 1, 1 );

    // Row 2
    auto* themeBtn = createButtonWithIcon( "Set System Theme ", "preferences-desktop-theme", false );
    connect( themeBtn, &QPushButton::clicked, this, &WelcomeWindow::goTheme );
    grid->addWidget( themeBtn, 2, 0 );

    auto* mirrorlistBtn = createButtonWithIcon( "Update Mirrorlist ", "view-refresh", false );
    connect( mirrorlistBtn, &QPushButton::clicked, this, &WelcomeWindow::goMirrorlist );
    grid->addWidget( mirrorlistBtn, 2, 1 );

    layout->addLayout( grid );
}

void
WelcomeWindow::addProjectInformationSection( QVBoxLayout* layout )
{
    auto* sectionLabel = new QLabel( "Project Information" );
    sectionLabel->setObjectName( "sectionLabel" );
    sectionLabel->setAlignment( Qt::AlignCenter );
    layout->addWidget( sectionLabel );

    auto* grid = new QGridLayout();
    grid->setSpacing( 10 );

    auto* websiteBtn = createButtonWithIcon( "Website ", "applications-internet", false );
    connect( websiteBtn, &QPushButton::clicked, this, &WelcomeWindow::onWebsite );
    grid->addWidget( websiteBtn, 0, 0 );

    auto* githubBtn = createButtonWithIcon( "GitHub  ", "assets/github.svg", true );
    connect( githubBtn, &QPushButton::clicked, this, &WelcomeWindow::onGithub );
    grid->addWidget( githubBtn, 0, 1 );

    auto* discordBtn = createButtonWithIcon( "Discord ", "assets/discord.svg", true );
    connect( discordBtn, &QPushButton::clicked, this, &WelcomeWindow::onDiscord );
    grid->addWidget( discordBtn, 1, 0 );

    auto* aboutBtn = createButtonWithIcon( "About Archer ", "help-about", false );
    connect( aboutBtn, &QPushButton::clicked, this, &WelcomeWindow::goAbout );
    grid->addWidget( aboutBtn, 1, 1 );

    layout->addLayout( grid );
}

void
WelcomeWindow::addLogSection( QVBoxLayout* layout )
{
    // Wrapped in its own container (added with stretch below) so it - not the button grids above
    // it - absorbs the window's extra height, whether or not the log view is currently shown.
    auto* container = new QWidget();
    auto* containerLayout = new QVBoxLayout( container );
    containerLayout->setContentsMargins( 0, 0, 0, 0 );
    containerLayout->setSpacing( 6 );

    logToggleButton = new QPushButton( QString::fromUtf8( "\xE2\x80\xBA View Logs" ) );
    logToggleButton->setObjectName( "logToggleButton" );
    logToggleButton->setFlat( true );
    logToggleButton->setCursor( Qt::PointingHandCursor );
    logToggleButton->setFocusPolicy( Qt::NoFocus );
    connect( logToggleButton, &QPushButton::clicked, this, &WelcomeWindow::toggleLogSection );
    containerLayout->addWidget( logToggleButton );

    logView = new QPlainTextEdit();
    logView->setObjectName( "logView" );
    logView->setReadOnly( true );
    logView->setPlaceholderText( "No activity yet." );
    logView->hide();
    containerLayout->addWidget( logView, 1 );

    layout->addWidget( container, 1 );
}

QPushButton*
WelcomeWindow::createButtonWithIcon( const QString& label, const QString& iconName, bool fromFile )
{
    auto* button = new QPushButton( label );
    button->setFocusPolicy( Qt::NoFocus );

    // Set icon
    if ( fromFile )
    {
        // Use search path prefix if iconName doesn't already have it
        QString iconPath = iconName;
        if ( !iconPath.startsWith( "assets:" ) )
        {
            // Strip leading 'assets/' if present since we use the 'assets:' search path
            if ( iconPath.startsWith( "assets/" ) )
            {
                iconPath = "assets:" + iconPath.mid( 7 );
            }
            else
            {
                iconPath = "assets:" + iconPath;
            }
        }

        if ( QFile::exists( iconPath ) )
        {
            button->setIcon( QIcon( iconPath ) );
            button->setIconSize( QSize( ICON_SIZE, ICON_SIZE ) );
        }
    }
    else
    {
        // Use system theme icon
        button->setIcon( QIcon::fromTheme( iconName ) );
        button->setIconSize( QSize( ICON_SIZE, ICON_SIZE ) );
    }

    button->setLayoutDirection( Qt::RightToLeft );  // Icon on right
    return button;
}

void
WelcomeWindow::goHome()
{
    pages->setCurrentIndex( 0 );
}

void
WelcomeWindow::goMirrorlist()
{
    mirrorlistPage->resetToConfig();
    pages->setCurrentWidget( mirrorlistPage );
}

void
WelcomeWindow::goTheme()
{
    pages->setCurrentWidget( themePage );
}

void
WelcomeWindow::goAbout()
{
    pages->setCurrentWidget( aboutPage );
}

void
WelcomeWindow::appendActivityLog( const QString& message )
{
    const QString timestamp = QTime::currentTime().toString( "HH:mm:ss" );
    logView->appendPlainText( QString( "%1  %2" ).arg( timestamp, message ) );
}

void
WelcomeWindow::toggleLogSection()
{
    logExpanded = !logExpanded;
    logView->setVisible( logExpanded );
    logToggleButton->setText( QString::fromUtf8( logExpanded ? "\xE2\x8C\xA5 View Logs" : "\xE2\x80\xBA View Logs" ) );
}

void
WelcomeWindow::showToast( const QString& message )
{
    toastLabel->setText( message );
    toastLabel->adjustSize();
    positionToast();
    toastLabel->raise();
    toastLabel->show();
    toastTimer->start( TOAST_DURATION_MS );
}

void
WelcomeWindow::positionToast()
{
    if ( !toastLabel )
    {
        return;
    }
    const int x = ( width() - toastLabel->width() ) / 2;
    const int y = height() - toastLabel->height() - 20;
    toastLabel->move( x, y );
}

void
WelcomeWindow::resizeEvent( QResizeEvent* event )
{
    QMainWindow::resizeEvent( event );
    if ( toastLabel && toastLabel->isVisible() )
    {
        positionToast();
    }
}

void
WelcomeWindow::setupCalamaresMonitoring()
{
    if ( !isLiveISO || !installButton )
    {
        return;
    }

    // Create timer to check Calamares status every 2 seconds
    connect( calamaresTimer.get(), &QTimer::timeout, this, &WelcomeWindow::checkCalamaresStatus );
    calamaresTimer->start( 2000 );  // Check every 2 seconds

    // Initial check
    checkCalamaresStatus();
}

void
WelcomeWindow::checkCalamaresStatus()
{
    if ( !installButton )
    {
        return;
    }

    const bool isRunning = Extras::isCalamaresRunning();

    installButton->setEnabled( !isRunning );
    if ( isRunning )
    {
        installButton->setText( "Installing... " );
    }
    else
    {
        installButton->setText( "Install ALG " );
    }
}

// Slots
void
WelcomeWindow::onInstallAlg()
{
    Extras::runCalamaresIfLiveISO( isLiveISO );
    showToast( "Launching installer..." );
}

void
WelcomeWindow::onScreenResolution()
{
    Resolution::screenResolution( desktopEnv );
    showToast( "Opening resolution settings..." );
}

void
WelcomeWindow::onUpdateSystem()
{
    Updates::updateSystem( desktopEnv );
    appendActivityLog( "System update launched in a terminal." );
    showToast( "Updating system..." );
}

void
WelcomeWindow::onSyncRepositories()
{
    Updates::syncDatabases( desktopEnv );
    appendActivityLog( "Repository sync launched in a terminal." );
    showToast( "Syncing repositories..." );
}

void
WelcomeWindow::onLaunchAppStore()
{
    QProcess::startDetached( "alg-app-store", QStringList() );
    showToast( "Launching App Store..." );
}

void
WelcomeWindow::onAutostartToggled( bool checked )
{
    Autostart::toggleAutostart( checked );
}

void
WelcomeWindow::onWebsite()
{
    Extras::openUrl( "https://www.arkalinuxgui.org" );
    showToast( "Opening website..." );
}

void
WelcomeWindow::onGithub()
{
    Extras::openUrl( "https://github.com/arch-linux-gui" );
    showToast( "Opening GitHub..." );
}

void
WelcomeWindow::onDiscord()
{
    Extras::openUrl( "https://discord.gg/NgAFEw9Tkf" );
    showToast( "Opening Discord..." );
}
