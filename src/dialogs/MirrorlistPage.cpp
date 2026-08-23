#include "MirrorlistPage.h"
#include "AppPalette.h"
#include "MirrorListParsing.h"
#include "PageChrome.h"

#include <spdlog/spdlog.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMetaObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

MirrorlistPage::MirrorlistPage( bool dark, QWidget* parent )
    : QWidget( parent )
{
    // Connect signals for thread-safe UI updates
    connect( &workerSignals, &MirrorListSignals::logAppended, this, &MirrorlistPage::appendLogToUI );
    connect( &workerSignals, &MirrorListSignals::updateFinished, this, &MirrorlistPage::onUpdateFinished );

    setupUI( dark );

    spdlog::debug( "MirrorlistPage initialized" );
}

void
MirrorlistPage::setupUI( bool dark )
{
    auto* outer = new QVBoxLayout( this );
    outer->setContentsMargins( 0, 0, 0, 0 );
    outer->setSpacing( 0 );

    stack = new QStackedWidget( this );

    configPage = buildConfigPage( dark );
    logPage = buildLogPage( dark );
    stack->addWidget( configPage );
    stack->addWidget( logPage );

    outer->addWidget( stack, 1 );
}

void
MirrorlistPage::resetToConfig()
{
    stack->setCurrentWidget( configPage );
}

void
MirrorlistPage::applyTheme( bool dark )
{
    const auto& palette = AppPalette::forSystem( dark );
    if ( configSubHeader )
    {
        PageChrome::styleSubHeader( configSubHeader, palette );
    }
    if ( logSubHeader )
    {
        PageChrome::styleSubHeader( logSubHeader, palette );
    }
}

QWidget*
MirrorlistPage::buildConfigPage( bool dark )
{
    auto* page = new QWidget();
    auto* outer = new QVBoxLayout( page );
    outer->setContentsMargins( 0, 0, 0, 0 );
    outer->setSpacing( 0 );

    const auto header = PageChrome::buildSubHeader( "Update MirrorList", AppPalette::forSystem( dark ), page );
    configSubHeader = header.widget;
    connect( header.backButton, &QPushButton::clicked, this, &MirrorlistPage::backRequested );
    outer->addWidget( header.widget );

    auto* content = new QWidget();
    auto* layout = new QVBoxLayout( content );
    layout->setSpacing( 12 );
    layout->setContentsMargins( 18, 14, 18, 14 );

    setupCountriesSection( layout );
    setupProtocolSection( layout );
    setupSortSection( layout );
    setupSettingsSection( layout );
    layout->addStretch();

    outer->addWidget( content, 1 );

    // Buttons (Update and Close)
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins( 18, 12, 18, 12 );
    buttonLayout->setSpacing( 10 );

    updateButton = new QPushButton( "Update" );
    updateButton->setEnabled( false );
    connect( updateButton, &QPushButton::clicked, this, &MirrorlistPage::onUpdateClicked );

    auto* closeButton = new QPushButton( "Close" );
    connect( closeButton, &QPushButton::clicked, this, &MirrorlistPage::backRequested );

    buttonLayout->addWidget( updateButton );
    buttonLayout->addWidget( closeButton );
    outer->addLayout( buttonLayout );

    return page;
}

QWidget*
MirrorlistPage::buildLogPage( bool dark )
{
    auto* page = new QWidget();
    auto* outer = new QVBoxLayout( page );
    outer->setContentsMargins( 0, 0, 0, 0 );
    outer->setSpacing( 0 );

    const auto header = PageChrome::buildSubHeader( "Update Progress", AppPalette::forSystem( dark ), page );
    logSubHeader = header.widget;
    connect( header.backButton, &QPushButton::clicked, this, &MirrorlistPage::resetToConfig );
    outer->addWidget( header.widget );

    auto* content = new QWidget();
    auto* contentLayout = new QVBoxLayout( content );
    contentLayout->setContentsMargins( 10, 10, 10, 10 );

    logTree = new QTreeWidget();
    logTree->setHeaderLabels( { "Server", "Rate", "Time" } );
    logTree->header()->setSectionResizeMode( 0, QHeaderView::Stretch );
    logTree->setAlternatingRowColors( true );
    contentLayout->addWidget( logTree );

    outer->addWidget( content, 1 );

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins( 18, 12, 18, 12 );

    logCloseButton = new QPushButton( "Updating..." );
    logCloseButton->setEnabled( false );
    logCloseButton->setObjectName( "primaryButton" );
    connect( logCloseButton, &QPushButton::clicked, this, &MirrorlistPage::resetToConfig );
    buttonLayout->addWidget( logCloseButton );

    outer->addLayout( buttonLayout );

    return page;
}

void
MirrorlistPage::setupCountriesSection( QVBoxLayout* layout )
{
    auto* group = new QGroupBox( "Countries" );
    auto* grid = new QGridLayout();
    grid->setSpacing( 10 );

    const QStringList countries
        = { "Australia", "Brazil",      "Canada", "China",  "France",         "Germany",       "India",
            "Japan",     "Netherlands", "Russia", "Sweden", "United Kingdom", "United States", "Worldwide" };

    for ( int i = 0; i < countries.size(); ++i )
    {
        const auto& country = countries[ i ];
        auto* checkbox = new QCheckBox( country );
        checkbox->setFocusPolicy( Qt::NoFocus );
        connect( checkbox, &QCheckBox::toggled, this, &MirrorlistPage::onCountryToggled );
        countryCheckboxes[ country ] = checkbox;
        grid->addWidget( checkbox, i / 2, i % 2 );
    }

    group->setLayout( grid );
    layout->addWidget( group );
}

void
MirrorlistPage::setupProtocolSection( QVBoxLayout* layout )
{
    auto* group = new QGroupBox( "Protocols" );
    auto* hbox = new QHBoxLayout();

    httpsCheck = new QCheckBox( "HTTPS" );
    httpsCheck->setChecked( true );
    httpsCheck->setFocusPolicy( Qt::NoFocus );

    httpCheck = new QCheckBox( "HTTP" );
    httpCheck->setFocusPolicy( Qt::NoFocus );

    hbox->addWidget( httpsCheck );
    hbox->addWidget( httpCheck );
    hbox->addStretch();

    group->setLayout( hbox );
    layout->addWidget( group );
}

void
MirrorlistPage::setupSortSection( QVBoxLayout* layout )
{
    auto* group = new QGroupBox( "Sort By" );
    auto* hbox = new QHBoxLayout();

    sortCombo = new QComboBox();
    sortCombo->addItems( { "Rate", "Age", "Score", "Delay", "Country" } );
    sortCombo->setCurrentIndex( 0 );

    hbox->addWidget( sortCombo );
    group->setLayout( hbox );
    layout->addWidget( group );
}

void
MirrorlistPage::setupSettingsSection( QVBoxLayout* layout )
{
    auto* hbox = new QHBoxLayout();

    // Max fresh mirrors
    auto* mirrorLabel = new QLabel( "Max Fresh Mirrors:" );
    mirrorSpin = new QSpinBox();
    mirrorSpin->setRange( 1, 20 );
    mirrorSpin->setValue( 5 );

    hbox->addWidget( mirrorLabel );
    hbox->addWidget( mirrorSpin );

    // Timeout
    auto* timeoutLabel = new QLabel( "Timeout (s):" );
    timeoutSpin = new QSpinBox();
    timeoutSpin->setRange( 5, 60 );
    timeoutSpin->setValue( 10 );

    hbox->addWidget( timeoutLabel );
    hbox->addWidget( timeoutSpin );
    hbox->addStretch();

    layout->addLayout( hbox );
}

void
MirrorlistPage::onCountryToggled( bool checked )
{
    auto* checkbox = qobject_cast< QCheckBox* >( sender() );
    if ( !checkbox )
    {
        return;
    }

    const QString country = checkbox->text();

    if ( checked )
    {
        if ( !selectedCountries.contains( country ) )
        {
            selectedCountries.append( country );
            spdlog::trace( "Country selected: {}", country.toStdString() );
        }
    }
    else
    {
        selectedCountries.removeAll( country );
        spdlog::trace( "Country deselected: {}", country.toStdString() );
    }

    updateButton->setEnabled( !selectedCountries.isEmpty() );
    spdlog::trace( "Selected countries: {}", selectedCountries.join( ", " ).toStdString() );
}

void
MirrorlistPage::onUpdateClicked()
{
    spdlog::info( "Mirror update started" );

    // Gather protocols
    QStringList protocols;
    if ( httpsCheck->isChecked() )
    {
        protocols << "https";
    }
    if ( httpCheck->isChecked() )
    {
        protocols << "http";
    }
    if ( protocols.isEmpty() )
    {
        protocols << "https";  // Default to https
    }

    const int maxMirrors = mirrorSpin->value();
    const int timeout = timeoutSpin->value();
    const QString sortBy = sortCombo->currentText().toLower();

    spdlog::debug( "Selected countries: {}", selectedCountries.join( ", " ).toStdString() );
    spdlog::debug( "Protocols: {}", protocols.join( ", " ).toStdString() );
    spdlog::debug( "Max mirrors: {}, timeout: {}s, sort by: {}", maxMirrors, timeout, sortBy.toStdString() );

    // Build reflector arguments for pkexec
    QStringList args;
    args << "reflector";
    args << "--country" << selectedCountries.join( "," );
    args << "--protocol" << protocols.join( "," );
    args << "--latest" << QString::number( maxMirrors );
    args << "--sort" << sortBy;
    args << "--download-timeout" << QString::number( timeout );
    args << "--save" << "/etc/pacman.d/mirrorlist";
    args << "--verbose";

    spdlog::debug( "Reflector args: {}", args.join( " " ).toStdString() );

    // Reset the log sub-view and show it
    logTree->clear();
    lineCounter = 0;
    logCloseButton->setEnabled( false );
    logCloseButton->setText( "Updating..." );
    stack->setCurrentWidget( logPage );

    Q_EMIT toastRequested( "Updating mirror list..." );
    startMirrorListUpdate( args );
}

void
MirrorlistPage::startMirrorListUpdate( const QStringList& args )
{
    spdlog::debug( "Starting mirror list update thread" );
    isUpdating = true;
    updateButton->setEnabled( false );

    // Start update in separate thread (jthread requests-stop-and-joins any previous one on assignment)
    updateThread = std::jthread(
        [ this, args ]( std::stop_token )
        {
            spdlog::trace( "Update thread started" );

            // Clean environment to avoid Qt library conflicts
            auto env = QProcessEnvironment::systemEnvironment();
            env.remove( "LD_LIBRARY_PATH" );
            env.remove( "QT_PLUGIN_PATH" );
            env.remove( "QT_QPA_PLATFORM_THEME" );

            spdlog::debug( "Executing: pkexec {}", args.join( " " ).toStdString() );

            // Add initial log entry
            QMetaObject::invokeMethod( &workerSignals,
                                       "logAppended",
                                       Qt::QueuedConnection,
                                       Q_ARG( QString, "Starting reflector..." ),
                                       Q_ARG( QString, "" ),
                                       Q_ARG( QString, "" ) );

            QProcess process;
            process.setProcessEnvironment( env );
            process.setProcessChannelMode( QProcess::MergedChannels );
            process.start( "pkexec", args );
            process.waitForStarted();

            spdlog::trace( "Process spawned, reading output..." );

            // Read output line by line
            while ( process.state() != QProcess::NotRunning || process.canReadLine() )
            {
                if ( process.canReadLine() )
                {
                    const QString line = QString::fromUtf8( process.readLine() ).trimmed();
                    if ( !line.isEmpty() )
                    {
                        processLogLine( line );
                    }
                }
                else
                {
                    process.waitForReadyRead( 100 );
                }
            }

            // Read any remaining output
            while ( process.canReadLine() )
            {
                const QString line = QString::fromUtf8( process.readLine() ).trimmed();
                if ( !line.isEmpty() )
                {
                    processLogLine( line );
                }
            }

            const int returnCode = process.exitCode();
            spdlog::debug( "Process completed with return code: {}", returnCode );


            if ( returnCode == 0 )
            {
                QMetaObject::invokeMethod( &workerSignals,
                                           "logAppended",
                                           Qt::QueuedConnection,
                                           Q_ARG( QString, "Update completed successfully!" ),
                                           Q_ARG( QString, "" ),
                                           Q_ARG( QString, "" ) );
            }
            else
            {
                QMetaObject::invokeMethod( &workerSignals,
                                           "logAppended",
                                           Qt::QueuedConnection,
                                           Q_ARG( QString, QString( "Update failed with code %1" ).arg( returnCode ) ),
                                           Q_ARG( QString, "" ),
                                           Q_ARG( QString, "" ) );
            }

            spdlog::trace( "Update thread finishing" );
            QMetaObject::invokeMethod( &workerSignals, "updateFinished", Qt::QueuedConnection );
        } );
}

void
MirrorlistPage::processLogLine( const QString& logLine )
{
    lineCounter++;

    if ( logLine.isEmpty() )
    {
        return;
    }

    spdlog::trace( "Processing log line #{}: {}", lineCounter, logLine.left( 100 ).toStdString() );

    const auto parsed = MirrorListParsing::parseLine( logLine );

    if ( parsed.logType == "WARNING" )
    {
        spdlog::warn( "reflector: {}", parsed.server.toStdString() );
    }
    else if ( parsed.logType == "ERROR" )
    {
        spdlog::error( "reflector: {}", parsed.server.toStdString() );
    }
    else
    {
        spdlog::trace( "Parsed -> server: {}, rate: {}, time: {}",
                       parsed.server.toStdString(),
                       parsed.rate.toStdString(),
                       parsed.time.toStdString() );
    }

    QMetaObject::invokeMethod( &workerSignals,
                               "logAppended",
                               Qt::QueuedConnection,
                               Q_ARG( QString, parsed.server ),
                               Q_ARG( QString, parsed.rate ),
                               Q_ARG( QString, parsed.time ) );
}

void
MirrorlistPage::appendLogToUI( const QString& server, const QString& rate, const QString& time )
{
    spdlog::trace(
        "Appending to UI: {}, {}, {}", server.left( 50 ).toStdString(), rate.toStdString(), time.toStdString() );

    if ( logTree )
    {
        auto* item = new QTreeWidgetItem( logTree );
        item->setText( 0, server );
        item->setText( 1, rate );
        item->setText( 2, time );
        logTree->scrollToItem( item );
    }
    else
    {
        spdlog::error( "appendLogToUI: logTree is nullptr" );
    }
}

void
MirrorlistPage::onUpdateFinished()
{
    spdlog::trace( "Update finished callback" );
    isUpdating = false;
    updateButton->setEnabled( true );

    // Enable the Close button now that update is complete
    if ( logCloseButton )
    {
        logCloseButton->setEnabled( true );
        logCloseButton->setText( "Close" );
    }

    Q_EMIT toastRequested( "Mirror list update finished" );
    spdlog::info( "Mirror update finished. Progress page remains open for review." );
}
