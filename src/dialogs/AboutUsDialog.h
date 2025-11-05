#ifndef ABOUTUSDIALOG_H
#define ABOUTUSDIALOG_H

#include <QDialog>

class AboutUsDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutUsDialog(QWidget *parent = nullptr);
    ~AboutUsDialog() override = default;

private:
    void setupUI();
};

#endif // ABOUTUSDIALOG_H
