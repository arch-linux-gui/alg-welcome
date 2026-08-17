#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QWidget>

class AboutPage : public QWidget
{
    Q_OBJECT

public:
    explicit AboutPage( QWidget* parent = nullptr );

Q_SIGNALS:
    void backRequested();
    void toastRequested( const QString& message );

private:
    void setupUI();
};

#endif  // ABOUTPAGE_H
