#ifndef MIRRORLISTPAGE_H
#define MIRRORLISTPAGE_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <atomic>
#include <thread>

class QCheckBox;
class QComboBox;
class QSpinBox;
class QPushButton;
class QStackedWidget;
class QTreeWidget;

// Signals class for thread-safe UI updates
class MirrorListSignals : public QObject
{
    Q_OBJECT

public:
    explicit MirrorListSignals( QObject* parent = nullptr )
        : QObject( parent )
    {
    }

Q_SIGNALS:
    void logAppended( const QString& server, const QString& rate, const QString& time );
    void updateFinished();
};

// Update MirrorList, as an in-window page: a config sub-view (countries/protocols/sort/settings)
// and a progress-log sub-view, switched between internally. Only the config sub-view's Back/Close
// leaves the page entirely (back to Home) - the log sub-view's Back returns to the config sub-view.
class MirrorlistPage : public QWidget
{
    Q_OBJECT

public:
    explicit MirrorlistPage( bool dark, QWidget* parent = nullptr );

    // Shows the config sub-view. Call before navigating to this page from Home, so re-entering
    // after a completed update doesn't leave the user stranded on the log sub-view.
    void resetToConfig();

    // Re-colors both sub-views' header bars for a possibly-changed system theme.
    void applyTheme( bool dark );

Q_SIGNALS:
    void backRequested();
    void toastRequested( const QString& message );

private:
    void setupUI( bool dark );
    QWidget* buildConfigPage( bool dark );
    QWidget* buildLogPage( bool dark );
    void setupCountriesSection( class QVBoxLayout* layout );
    void setupProtocolSection( class QVBoxLayout* layout );
    void setupSortSection( class QVBoxLayout* layout );
    void setupSettingsSection( class QVBoxLayout* layout );

    void startMirrorListUpdate( const QStringList& args );
    void processLogLine( const QString& logLine );

    // Slots
    void onCountryToggled( bool checked );
    void onUpdateClicked();
    void appendLogToUI( const QString& server, const QString& rate, const QString& time );
    void onUpdateFinished();

    // Member variables
    QMap< QString, QCheckBox* > countryCheckboxes;
    QStringList selectedCountries;
    std::atomic< bool > isUpdating { false };
    int lineCounter = 0;

    QStackedWidget* stack = nullptr;
    QWidget* configPage = nullptr;
    QWidget* logPage = nullptr;
    QWidget* configSubHeader = nullptr;
    QWidget* logSubHeader = nullptr;

    // Config sub-view widgets
    QCheckBox* httpsCheck = nullptr;
    QCheckBox* httpCheck = nullptr;
    QComboBox* sortCombo = nullptr;
    QSpinBox* mirrorSpin = nullptr;
    QSpinBox* timeoutSpin = nullptr;
    QPushButton* updateButton = nullptr;

    // Log sub-view widgets
    QTreeWidget* logTree = nullptr;
    QPushButton* logCloseButton = nullptr;

    // Signals object for thread communication
    MirrorListSignals workerSignals;

    // Update thread (jthread auto-joins on destruction, including mid-update teardown)
    std::jthread updateThread;
};

#endif  // MIRRORLISTPAGE_H
