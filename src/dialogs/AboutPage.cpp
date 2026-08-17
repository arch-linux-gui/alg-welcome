#include "AboutPage.h"
#include "PageChrome.h"

#include <QDesktopServices>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

AboutPage::AboutPage( QWidget* parent )
    : QWidget( parent )
{
    setupUI();
}

void
AboutPage::setupUI()
{
    auto* outer = new QVBoxLayout( this );
    outer->setContentsMargins( 0, 0, 0, 0 );
    outer->setSpacing( 0 );

    const auto header = PageChrome::buildSubHeader( "About Archer", this );
    connect( header.backButton, &QPushButton::clicked, this, &AboutPage::backRequested );
    outer->addWidget( header.widget );

    auto* content = new QWidget( this );
    auto* layout = new QVBoxLayout( content );
    layout->setSpacing( 10 );
    layout->setContentsMargins( 24, 20, 24, 20 );
    layout->setAlignment( Qt::AlignHCenter );

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
    description->setMaximumWidth( 360 );
    description->setAlignment( Qt::AlignJustify );
    layout->addWidget( description, 0, Qt::AlignHCenter );

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
             [ this ]()
             {
                 QDesktopServices::openUrl( QUrl( "https://www.arkalinuxgui.org" ) );
                 Q_EMIT toastRequested( "Opening website..." );
             } );
    layout->addWidget( websiteButton );

    layout->addStretch();

    // License
    auto* licenseText = new QLabel( "Distributed under the MIT License." );
    licenseText->setWordWrap( true );
    licenseText->setAlignment( Qt::AlignCenter );
    layout->addWidget( licenseText );

    outer->addWidget( content, 1 );
}
