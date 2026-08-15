#include "MirrorListDialog.h"
#include "MirrorListParsing.h"

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
#include <QTreeWidget>
#include <QVBoxLayout>

MirrorListDialog::MirrorListDialog( QWidget* parent )
    : QDialog( parent )
    , workerSignals( new MirrorListSignals() )
{
    // Connect signals for thread-safe UI updates
    connect( workerSignals, &MirrorListSignals::logAppended, this, &MirrorListDialog::appendLogToUI );
    connect( workerSignals, &MirrorListSignals::updateFinished, this, &MirrorListDialog::onUpdateFinished );

    setupUI();

    // Position dialog next to parent
    if ( parent )
    {
        const auto parentGeometry = parent->geometry();
        if ( parentGeometry.isValid() )
        {
            move( parentGeometry.x() + parentGeometry.width() + 10, parentGeometry.y() );
        }
    }

    spdlog::debug( "MirrorListDialog initialized" );
}

MirrorListDialog::~MirrorListDialog()
{
    // Wait for update thread to finish if it's running
    if ( updateThread && updateThread->joinable() )
    {
        updateThread->join();
    }
    delete workerSignals;
}

void
MirrorListDialog::setupUI()
{
    setWindowTitle( "Update MirrorList" );
    setMinimumSize( 500, 450 );
    setModal( false );

    setWindowFlags( windowFlags() | Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                    | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint );

    auto* layout = new QVBoxLayout( this );
    layout->setSpacing( 10 );
    layout->setContentsMargins( 10, 10, 10, 10 );

    // Countries selection
    setupCountriesSection( layout );

    // Protocol selection
    setupProtocolSection( layout );

    // Sort by selection
    setupSortSection( layout );

    // Settings (max mirrors and timeout)
    setupSettingsSection( layout );

    // Buttons (Update and Close)
    auto* buttonLayout = new QHBoxLayout();

    updateButton = new QPushButton( "Update" );
    updateButton->setEnabled( false );
    connect( updateButton, &QPushButton::clicked, this, &MirrorListDialog::onUpdateClicked );

    mainCloseButton = new QPushButton( "Close" );
    connect( mainCloseButton, &QPushButton::clicked, this, &QDialog::close );

    buttonLayout->addWidget( updateButton );
    buttonLayout->addWidget( mainCloseButton );
    layout->addLayout( buttonLayout );
}

void
MirrorListDialog::setupCountriesSection( QVBoxLayout* layout )
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
        connect( checkbox, &QCheckBox::toggled, this, &MirrorListDialog::onCountryToggled );
        countryCheckboxes[ country ] = checkbox;
        grid->addWidget( checkbox, i / 2, i % 2 );
    }

    group->setLayout( grid );
    layout->addWidget( group );
}

void
MirrorListDialog::setupProtocolSection( QVBoxLayout* layout )
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
MirrorListDialog::setupSortSection( QVBoxLayout* layout )
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
MirrorListDialog::setupSettingsSection( QVBoxLayout* layout )
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
MirrorListDialog::onCountryToggled( bool checked )
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
MirrorListDialog::onUpdateClicked()
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

    // Show log dialog and start update
    showLogDialog();
    startMirrorListUpdate( args );
}

void
MirrorListDialog::showLogDialog()
{
    if ( logDialog )
    {
        spdlog::trace( "Log dialog already exists, showing it" );
        logDialog->show();
        return;
    }

    spdlog::trace( "Creating new log dialog" );
    lineCounter = 0;

    logDialog = new QDialog( this );
    logDialog->setWindowTitle( "Update Progress" );
    logDialog->setMinimumSize( 600, 400 );
    logDialog->setModal( false );
    logDialog->setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint
                               | Qt::WindowCloseButtonHint );

    auto* layout = new QVBoxLayout( logDialog );
    layout->setSpacing( 10 );
    layout->setContentsMargins( 10, 10, 10, 10 );

    // Create tree widget for logs
    logTree = new QTreeWidget();
    logTree->setHeaderLabels( { "Server", "Rate", "Time" } );
    logTree->header()->setSectionResizeMode( 0, QHeaderView::Stretch );
    logTree->setAlternatingRowColors( true );

    layout->addWidget( logTree );

    // Add Close button at the bottom
    closeButton = new QPushButton( "Close" );
    closeButton->setEnabled( false );  // Disabled during update
    closeButton->setMinimumHeight( 35 );
    connect( closeButton, &QPushButton::clicked, logDialog, &QDialog::close );
    layout->addWidget( closeButton );

    // Position next to parent
    const auto parentGeometry = geometry();
    logDialog->move( parentGeometry.x() + parentGeometry.width() + 10, parentGeometry.y() );

    spdlog::trace( "Log dialog created and showing" );
    logDialog->show();
}

void
MirrorListDialog::startMirrorListUpdate( const QStringList& args )
{
    spdlog::debug( "Starting mirror list update thread" );
    isUpdating = true;
    updateButton->setEnabled( false );

    // Start update in separate thread
    updateThread = std::make_unique< std::thread >(
        [ this, args ]()
        {
            spdlog::trace( "Update thread started" );

            // Clean environment to avoid Qt library conflicts
            auto env = QProcessEnvironment::systemEnvironment();
            env.remove( "LD_LIBRARY_PATH" );
            env.remove( "QT_PLUGIN_PATH" );
            env.remove( "QT_QPA_PLATFORM_THEME" );

            spdlog::debug( "Executing: pkexec {}", args.join( " " ).toStdString() );

            // Add initial log entry
            QMetaObject::invokeMethod( workerSignals,
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
                QMetaObject::invokeMethod( workerSignals,
                                           "logAppended",
                                           Qt::QueuedConnection,
                                           Q_ARG( QString, "Update completed successfully!" ),
                                           Q_ARG( QString, "" ),
                                           Q_ARG( QString, "" ) );
            }
            else
            {
                QMetaObject::invokeMethod( workerSignals,
                                           "logAppended",
                                           Qt::QueuedConnection,
                                           Q_ARG( QString, QString( "Update failed with code %1" ).arg( returnCode ) ),
                                           Q_ARG( QString, "" ),
                                           Q_ARG( QString, "" ) );
            }

            spdlog::trace( "Update thread finishing" );
            QMetaObject::invokeMethod( workerSignals, "updateFinished", Qt::QueuedConnection );
        } );
}

// Backward-compat overload (unused): keep for now
void
MirrorListDialog::startMirrorListUpdate( const QString& command )
{
    // Fallback: run via shell and merged channels
    startMirrorListUpdate( QStringList { QStringLiteral( "sh" ), QStringLiteral( "-c" ), command } );
}

void
MirrorListDialog::processLogLine( const QString& logLine )
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

    QMetaObject::invokeMethod( workerSignals,
                               "logAppended",
                               Qt::QueuedConnection,
                               Q_ARG( QString, parsed.server ),
                               Q_ARG( QString, parsed.rate ),
                               Q_ARG( QString, parsed.time ) );
}

void
MirrorListDialog::appendLogToUI( const QString& server, const QString& rate, const QString& time )
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
MirrorListDialog::onUpdateFinished()
{
    spdlog::trace( "Update finished callback" );
    isUpdating = false;
    updateButton->setEnabled( true );

    // Enable the Close button now that update is complete
    if ( closeButton )
    {
        closeButton->setEnabled( true );
    }

    spdlog::info( "Mirror update finished. Log dialog remains open for review." );
}
