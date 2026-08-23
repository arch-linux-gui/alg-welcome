#include "WelcomeWindow.h"
#include "dialogs/AboutPage.h"
#include "dialogs/AppPalette.h"
#include "dialogs/MirrorlistPage.h"
#include "dialogs/ThemePage.h"
#include "utils/Autostart.h"
#include "utils/Extras.h"
#include "utils/Resolution.h"
#include "utils/Themes.h"
#include "utils/Updates.h"

#include <spdlog/spdlog.h>

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QFrame>
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

namespace
{

QFrame*
buildSeparator( const QString& color )
{
    auto* line = new QFrame();
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Plain );
    line->setFixedHeight( 1 );
    line->setStyleSheet( QString( "background-color: %1; border: none;" ).arg( color ) );
    return line;
}

}  // namespace

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
    isDarkTheme = Themes::isSystemDark( desktopEnv );

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
        QString styleSheet = QString::fromUtf8( file.readAll() );
        file.close();

        // styles.qss is a template: substitute the handful of @TOKEN@ color placeholders for the
        // system's current light/dark state (see src/dialogs/AppPalette.h) - everything else in
        // the file is the same regardless of theme.
        const auto& palette = AppPalette::forSystem( isDarkTheme );
        const QString btnBg = isDarkTheme ? "#2a2a2a" : "#ffffff";
        const QString btnText = isDarkTheme ? "#ffffff" : "#2b2e33";
        const QString btnBorder = isDarkTheme ? "#444444" : "#d0d3d8";
        const QString btnHoverBg = isDarkTheme ? "#3a3a3a" : "#f0f2f4";
        const QString btnHoverBorder = isDarkTheme ? "#555555" : "#c3c7cc";
        const QString btnPressedBg = isDarkTheme ? "#1a1a1a" : "#e4e7ea";
        const QString btnPressedBorder = isDarkTheme ? "#333333" : "#b5b9bf";
        const QString btnDisabledBg = isDarkTheme ? "#3a3a3a" : "#f0f2f4";
        const QString btnDisabledText = isDarkTheme ? "#888888" : "#a3a8ad";
        const QString btnDisabledBorder = isDarkTheme ? "#4a4a4a" : "#dde0e4";
        const QString logViewBg = isDarkTheme ? "#24262b" : "#ffffff";
        const QString logViewText = isDarkTheme ? "#8a9099" : palette.cardText;
        const QString toastBg = isDarkTheme ? "#3a3d42" : palette.chromeBg;
        const QString toastText = isDarkTheme ? "#eef0f2" : palette.chromeText;

        styleSheet.replace( "@btnBg@", btnBg );
        styleSheet.replace( "@btnText@", btnText );
        styleSheet.replace( "@btnBorder@", btnBorder );
        styleSheet.replace( "@btnHoverBg@", btnHoverBg );
        styleSheet.replace( "@btnHoverBorder@", btnHoverBorder );
        styleSheet.replace( "@btnPressedBg@", btnPressedBg );
        styleSheet.replace( "@btnPressedBorder@", btnPressedBorder );
        styleSheet.replace( "@btnDisabledBg@", btnDisabledBg );
        styleSheet.replace( "@btnDisabledText@", btnDisabledText );
        styleSheet.replace( "@btnDisabledBorder@", btnDisabledBorder );
        styleSheet.replace( "@subtitleText@", palette.mutedText );
        styleSheet.replace( "@sectionLabelText@", palette.accentText );
        styleSheet.replace( "@logToggleText@", palette.mutedText );
        styleSheet.replace( "@logViewBg@", logViewBg );
        styleSheet.replace( "@logViewBorder@", palette.dividerColor );
        styleSheet.replace( "@logViewText@", logViewText );
        styleSheet.replace( "@toastBg@", toastBg );
        styleSheet.replace( "@toastText@", toastText );

        setStyleSheet( styleSheet );
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

    mirrorlistPage = new MirrorlistPage( isDarkTheme, this );
    connect( mirrorlistPage, &MirrorlistPage::backRequested, this, &WelcomeWindow::goHome );
    connect( mirrorlistPage, &MirrorlistPage::toastRequested, this, &WelcomeWindow::showToast );
    pages->addWidget( mirrorlistPage );

    themePage = new ThemePage( desktopEnv, isDarkTheme, this );
    connect( themePage, &ThemePage::backRequested, this, &WelcomeWindow::goHome );
    connect( themePage, &ThemePage::toastRequested, this, &WelcomeWindow::showToast );
    connect( themePage, &ThemePage::themeApplied, this, &WelcomeWindow::applySystemTheme );
    pages->addWidget( themePage );

    aboutPage = new AboutPage( isDarkTheme, this );
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

    updatesRunner = new Updates::Runner( this );
    connect( updatesRunner, &Updates::Runner::lineOutput, this, &WelcomeWindow::onUpdatesLineOutput );
    connect( updatesRunner, &Updates::Runner::finished, this, &WelcomeWindow::onUpdatesFinished );
}

QWidget*
WelcomeWindow::buildHomePage()
{
    auto* homePage = new QWidget();

    auto* mainLayout = new QVBoxLayout( homePage );
    mainLayout->setSpacing( 10 );
    mainLayout->setContentsMargins( 20, 12, 20, 12 );

    const QString dividerColor = AppPalette::forSystem( isDarkTheme ).dividerColor;

    addHeader( mainLayout );
    headerSeparator = buildSeparator( dividerColor );
    mainLayout->addWidget( headerSeparator );
    addBasicUtilitiesSection( mainLayout );
    addProjectInformationSection( mainLayout );
    logSeparator = buildSeparator( dividerColor );
    mainLayout->addWidget( logSeparator );
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
    syncRepositoriesButton = createButtonWithIcon( "Sync Repositories ", "package-x-generic", false );
    connect( syncRepositoriesButton, &QPushButton::clicked, this, &WelcomeWindow::onSyncRepositories );
    grid->addWidget( syncRepositoriesButton, 1, 0 );

    updateSystemButton = createButtonWithIcon( "Update System ", "system-software-update", false );
    connect( updateSystemButton, &QPushButton::clicked, this, &WelcomeWindow::onUpdateSystem );
    grid->addWidget( updateSystemButton, 1, 1 );

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
WelcomeWindow::applySystemTheme()
{
    isDarkTheme = Themes::isSystemDark( desktopEnv );

    applyStylesheet();

    const QString dividerColor = AppPalette::forSystem( isDarkTheme ).dividerColor;
    const QString dividerStyle = QString( "background-color: %1; border: none;" ).arg( dividerColor );
    if ( headerSeparator )
    {
        headerSeparator->setStyleSheet( dividerStyle );
    }
    if ( logSeparator )
    {
        logSeparator->setStyleSheet( dividerStyle );
    }

    if ( mirrorlistPage )
    {
        mirrorlistPage->applyTheme( isDarkTheme );
    }
    if ( themePage )
    {
        themePage->applyTheme( isDarkTheme );
    }
    if ( aboutPage )
    {
        aboutPage->applyTheme( isDarkTheme );
    }
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
WelcomeWindow::startUpdatesOperation( const QString& label, const QStringList& pacmanArgs )
{
    if ( updatesRunner->isRunning() )
    {
        showToast( "An update is already running" );
        return;
    }

    currentUpdatesOperationLabel = label;

    if ( updateSystemButton )
    {
        updateSystemButton->setEnabled( false );
    }
    if ( syncRepositoriesButton )
    {
        syncRepositoriesButton->setEnabled( false );
    }

    // Surface live output as it streams in rather than making the user click to find it.
    if ( !logExpanded )
    {
        toggleLogSection();
    }

    appendActivityLog( QString( "%1 started." ).arg( label ) );
    showToast( QString( "%1..." ).arg( label ) );

    updatesRunner->start( pacmanArgs );
}

void
WelcomeWindow::onUpdateSystem()
{
    startUpdatesOperation( "System update", Updates::updateArgs() );
}

void
WelcomeWindow::onSyncRepositories()
{
    startUpdatesOperation( "Repository sync", Updates::syncArgs() );
}

void
WelcomeWindow::onUpdatesLineOutput( const QString& line )
{
    appendActivityLog( line );
}

void
WelcomeWindow::onUpdatesFinished( int exitCode )
{
    if ( updateSystemButton )
    {
        updateSystemButton->setEnabled( true );
    }
    if ( syncRepositoriesButton )
    {
        syncRepositoriesButton->setEnabled( true );
    }

    if ( exitCode == 0 )
    {
        appendActivityLog( QString( "%1 completed successfully." ).arg( currentUpdatesOperationLabel ) );
        showToast( QString( "%1 complete" ).arg( currentUpdatesOperationLabel ) );
    }
    else
    {
        appendActivityLog( QString( "%1 failed (exit code %2)." ).arg( currentUpdatesOperationLabel ).arg( exitCode ) );
        showToast( QString( "%1 failed" ).arg( currentUpdatesOperationLabel ) );
    }
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
