#ifndef MIRRORLISTDIALOG_H
#define MIRRORLISTDIALOG_H

#include <QDialog>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QObject>
#include <memory>
#include <thread>
#include <atomic>

class QCheckBox;
class QComboBox;
class QSpinBox;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

// Signals class for thread-safe UI updates
class MirrorListSignals : public QObject {
    Q_OBJECT
    
public:
    explicit MirrorListSignals(QObject *parent = nullptr) : QObject(parent) {}
    
Q_SIGNALS:
    void logAppended(const QString &server, const QString &rate, const QString &time);
    void updateFinished();
};

class MirrorListDialog : public QDialog {
    Q_OBJECT

public:
    explicit MirrorListDialog(QWidget *parent = nullptr);
    ~MirrorListDialog() override;

private:
    void setupUI();
    void setupCountriesSection(class QVBoxLayout *layout);
    void setupProtocolSection(class QVBoxLayout *layout);
    void setupSortSection(class QVBoxLayout *layout);
    void setupSettingsSection(class QVBoxLayout *layout);
    
    void showLogDialog();
    void startMirrorListUpdate(const QStringList &args);
    void startMirrorListUpdate(const QString &command);
    void processLogLine(const QString &logLine);
    
    // Slots
    void onCountryToggled(bool checked);
    void onUpdateClicked();
    void appendLogToUI(const QString &server, const QString &rate, const QString &time);
    void onUpdateFinished();
    
    // Member variables
    QMap<QString, QCheckBox*> countryCheckboxes;
    QStringList selectedCountries;
    std::atomic<bool> isUpdating{false};
    int lineCounter = 0;
    
    // UI widgets
    QCheckBox *httpsCheck = nullptr;
    QCheckBox *httpCheck = nullptr;
    QComboBox *sortCombo = nullptr;
    QSpinBox *mirrorSpin = nullptr;
    QSpinBox *timeoutSpin = nullptr;
    QPushButton *updateButton = nullptr;
    QPushButton *mainCloseButton = nullptr;
    
    // Log dialog
    QDialog *logDialog = nullptr;
    QTreeWidget *logTree = nullptr;
    QPushButton *closeButton = nullptr;
    
    // Signals object for thread communication
    MirrorListSignals *workerSignals;
    
    // Update thread
    std::unique_ptr<std::thread> updateThread;
};

#endif // MIRRORLISTDIALOG_H
