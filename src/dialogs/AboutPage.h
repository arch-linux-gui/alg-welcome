#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QWidget>

class AboutPage : public QWidget
{
    Q_OBJECT

public:
    explicit AboutPage( bool dark, QWidget* parent = nullptr );

    // Re-colors the page for a possibly-changed system theme; layout is untouched.
    void applyTheme( bool dark );

Q_SIGNALS:
    void backRequested();
    void toastRequested( const QString& message );

private:
    void setupUI( bool dark );

    QWidget* subHeader = nullptr;
};

#endif  // ABOUTPAGE_H
