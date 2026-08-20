#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <memory>

class QCheckBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QStackedWidget;
class QVBoxLayout;
class AboutPage;
class MirrorlistPage;
class ThemePage;

namespace Updates
{
class Runner;
}

class WelcomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WelcomeWindow( QWidget* parent = nullptr );
    ~WelcomeWindow() override = default;

protected:
    void resizeEvent( QResizeEvent* event ) override;

private:
    // Constants
    static constexpr int WINDOW_WIDTH = 520;
    static constexpr int WINDOW_HEIGHT = 600;
    static constexpr int LOGO_SIZE = 38;
    static constexpr int ICON_SIZE = 18;
    static constexpr int TOAST_DURATION_MS = 1800;

    // Returns the first path in `candidates` that exists, or an empty string if none do.
    static QString resolveExistingPath( const QStringList& candidates );

    // Setup methods
    void setupWindow();
    void applyStylesheet();
    void setupUI();
    void setupCalamaresMonitoring();

    // Home page construction
    QWidget* buildHomePage();
    void addHeader( QVBoxLayout* layout );
    void addBasicUtilitiesSection( QVBoxLayout* layout );
    void addProjectInformationSection( QVBoxLayout* layout );
    void addLogSection( QVBoxLayout* layout );

    QPushButton* createButtonWithIcon( const QString& label, const QString& iconName, bool fromFile );

    // Navigation between the single-window pages
    void goHome();
    void goMirrorlist();
    void goTheme();
    void goAbout();

    // Activity log (Home page) / toast notifications (float over every page)
    void appendActivityLog( const QString& message );
    void showToast( const QString& message );
    void positionToast();
    void toggleLogSection();

    // Slots
    void onInstallAlg();
    void onScreenResolution();
    void onUpdateSystem();
    void onSyncRepositories();
    void onLaunchAppStore();
    void onAutostartToggled( bool checked );
    void onWebsite();
    void onGithub();
    void onDiscord();
    void checkCalamaresStatus();
    void onUpdatesLineOutput( const QString& line );
    void onUpdatesFinished( int exitCode );

    // Starts an Updates::Runner operation (system upgrade or database sync) with the shared
    // "already running" guard, activity log entry, toast, and log-panel auto-expand.
    void startUpdatesOperation( const QString& label, const QStringList& pacmanArgs );

    // Member variables
    QString desktopEnv;
    bool isLiveISO = false;

    QStackedWidget* pages = nullptr;
    MirrorlistPage* mirrorlistPage = nullptr;
    ThemePage* themePage = nullptr;
    AboutPage* aboutPage = nullptr;

    QPushButton* installButton = nullptr;
    QPushButton* updateSystemButton = nullptr;
    QPushButton* syncRepositoriesButton = nullptr;
    QCheckBox* autostartSwitch = nullptr;

    QPushButton* logToggleButton = nullptr;
    QPlainTextEdit* logView = nullptr;
    bool logExpanded = false;

    Updates::Runner* updatesRunner = nullptr;
    QString currentUpdatesOperationLabel;

    QLabel* toastLabel = nullptr;
    QTimer* toastTimer = nullptr;

    std::unique_ptr< QTimer > calamaresTimer;
};

#endif  // WELCOMEWINDOW_H
