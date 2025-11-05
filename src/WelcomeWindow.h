#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <memory>

class QPushButton;
class QCheckBox;
class QVBoxLayout;
class MirrorListDialog;

class WelcomeWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit WelcomeWindow(QWidget *parent = nullptr);
    ~WelcomeWindow() override = default;

private:
    // Constants
    static constexpr int WINDOW_WIDTH = 480;
    static constexpr int WINDOW_HEIGHT = 400;
    static constexpr int LOGO_SIZE = 60;
    static constexpr int ICON_SIZE = 20;
    
    // Setup methods
    void setupWindow();
    void applyStylesheet();
    void setupUI();
    void setupCalamaresMonitoring();
    
    // UI creation methods
    void addHeader(QVBoxLayout *layout);
    void addInstallSetupSection(QVBoxLayout *layout);
    void addSocialMediaSection(QVBoxLayout *layout);
    void addMoreOptionsSection(QVBoxLayout *layout);
    void addAboutUsSection(QVBoxLayout *layout);
    
    QPushButton* createButtonWithIcon(const QString &label, 
                                      const QString &iconName, 
                                      bool fromFile);
    QWidget* createSwitchWithLabel(const QString &labelText, 
                                   QCheckBox **switchOut);
    
    // Slots
    void onInstallAlg();
    void onScreenResolution();
    void onUpdateSystem();
    void onUpdateMirrorlist();
    void onTutorials();
    void onAutostartToggled(bool checked);
    void onThemeToggled(bool checked);
    void onAboutUs();
    void checkCalamaresStatus();
    void onMirrorlistDialogClosed();
    
    // Member variables
    QString desktopEnv;
    bool isLiveISO;
    
    QPushButton *installButton = nullptr;
    QPushButton *updateSystemButton = nullptr;
    QPushButton *updateMirrorlistButton = nullptr;
    QCheckBox *autostartSwitch = nullptr;
    QCheckBox *themeSwitch = nullptr;
    
    std::unique_ptr<QTimer> calamaresTimer;
    MirrorListDialog *mirrorListDialog = nullptr;
};

#endif // WELCOMEWINDOW_H
