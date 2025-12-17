#include "AboutUsDialog.h"
#include "version.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QDesktopServices>
#include <QUrl>

AboutUsDialog::AboutUsDialog(QWidget *parent) 
    : QDialog(parent)
{
    setupUI();
}

void AboutUsDialog::setupUI() {
    setWindowTitle("About Us");
    setFixedSize(400, 350);
    setModal(true);
    
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // App name
    auto *appName = new QLabel("Arka Linux GUI");
    QFont font;
    font.setPointSize(18);
    font.setBold(true);
    appName->setFont(font);
    appName->setAlignment(Qt::AlignCenter);
    layout->addWidget(appName);
    
    // Version
    auto *version = new QLabel(QString("Version %1").arg(ALG_WELCOME_VERSION));
    version->setAlignment(Qt::AlignCenter);
    layout->addWidget(version);
    
    // Description
    auto *description = new QLabel("ALG Welcome was made to help you onboard quickly");
    description->setWordWrap(true);
    description->setAlignment(Qt::AlignJustify);
    layout->addWidget(description);
    
    // Developers label
    auto *devLabel = new QLabel("<b>Developers:</b>");
    devLabel->setAlignment(Qt::AlignLeft);
    layout->addWidget(devLabel);
    
    // Developer names
    auto *devNames = new QLabel(
        "• DemonKiller (Core Team)\n"
        "• Akash6222 (Core Team)\n"
        "• harshau007 (Core Team)"
    );
    devNames->setAlignment(Qt::AlignLeft);
    layout->addWidget(devNames);
    
    // Website button
    auto *websiteButton = new QPushButton("Visit our website");
    websiteButton->setFocusPolicy(Qt::NoFocus);
    connect(websiteButton, &QPushButton::clicked, []{
        QDesktopServices::openUrl(QUrl("https://www.arkalinuxgui.org"));
    });
    layout->addWidget(websiteButton);
    
    // License
    auto *licenseText = new QLabel("Distributed under the MIT License.");
    licenseText->setWordWrap(true);
    licenseText->setAlignment(Qt::AlignCenter);
    layout->addWidget(licenseText);
    
    layout->addStretch();
}
