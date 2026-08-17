#ifndef THEMEPAGE_H
#define THEMEPAGE_H

#include "../utils/Themes.h"

#include <QString>
#include <QWidget>
#include <memory>

class QButtonGroup;
class QLabel;
class QToolButton;
class QVBoxLayout;

// "Set System Theme" page: detects the desktop environment, then presents every preset the
// active Themes::ThemeManager reports (grouped by family - "Default" vs. "ALG Theme" - with no
// per-DE naming hardcoded here) as a grid of preview cards. Picking a card doesn't apply
// immediately; a confirm bar appears so the user commits deliberately, since applying now runs
// several real system commands rather than a single instant toggle.
class ThemePage : public QWidget
{
    Q_OBJECT

public:
    explicit ThemePage( const QString& desktopEnv, QWidget* parent = nullptr );

Q_SIGNALS:
    void backRequested();
    void toastRequested( const QString& message );

private:
    void setupUI();
    void setupUnsupportedUI();
    void rebuildCards();
    void selectPreset( const QString& id );
    void showConfirmBar( const QString& id );
    void hideConfirmBar();

    QString desktopEnv;
    std::unique_ptr< Themes::ThemeManager > themeManager;

    QVBoxLayout* groupsLayout = nullptr;
    QButtonGroup* cardGroup = nullptr;
    QWidget* confirmBar = nullptr;
    QLabel* confirmLabel = nullptr;
    QString pendingPresetId;
    QString appliedPresetId;
};

#endif  // THEMEPAGE_H
