#include "AboutUsDialog.h"
#include <QDesktopServices>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

AboutUsDialog::AboutUsDialog( QWidget* parent )
    : QDialog( parent )
{
    setupUI();
}

void
AboutUsDialog::setupUI()
{
    setWindowTitle( "About Us" );
    setFixedSize( 400, 350 );
    setModal( true );

    auto* layout = new QVBoxLayout( this );
    layout->setSpacing( 10 );
    layout->setContentsMargins( 20, 20, 20, 20 );

    // App name
    auto* appName = new QLabel( "Archer" );
    QFont font;
    font.setPointSize( 18 );
    font.setBold( true );
    appName->setFont( font );
    appName->setAlignment( Qt::AlignCenter );
    layout->addWidget( appName );

    // Version
    auto* version = new QLabel( QString( "Version %1" ).arg( ARCHER_VERSION ) );
    version->setAlignment( Qt::AlignCenter );
    layout->addWidget( version );

    // Description
    auto* description = new QLabel( "The Archer application was made to help you install ALG "
                                    "and onboard quickly. It also serves as a tool for simple utility tasks." );
    description->setWordWrap( true );
    description->setAlignment( Qt::AlignJustify );
    layout->addWidget( description );

    // Developers label
    auto* devLabel = new QLabel( "<b>Developers:</b>" );
    devLabel->setAlignment( Qt::AlignLeft );
    layout->addWidget( devLabel );

    // Developer names
    auto* devNames = new QLabel( "ALG Core Team" );
    devNames->setAlignment( Qt::AlignLeft );
    layout->addWidget( devNames );

    // Website button
    auto* websiteButton = new QPushButton( "Visit our website" );
    websiteButton->setFocusPolicy( Qt::NoFocus );
    connect( websiteButton,
             &QPushButton::clicked,
             [] { QDesktopServices::openUrl( QUrl( "https://www.arkalinuxgui.org" ) ); } );
    layout->addWidget( websiteButton );

    // License
    auto* licenseText = new QLabel( "Distributed under the MIT License." );
    licenseText->setWordWrap( true );
    licenseText->setAlignment( Qt::AlignCenter );
    layout->addWidget( licenseText );

    layout->addStretch();
}
