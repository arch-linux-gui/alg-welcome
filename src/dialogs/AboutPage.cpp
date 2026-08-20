#include "AboutPage.h"
#include "PageChrome.h"

#include <QDesktopServices>
#include <QHBoxLayout>
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
    // Set directly (rather than relying on the app-wide stylesheet's 16px QLabel default) so
    // this page's body text renders at the design's compact sizes regardless of where it's
    // embedded - a widget's own style sheet otherwise gets shadowed by an ancestor's.
    setStyleSheet( "QLabel#aboutAppName { font-size: 20px; font-weight: 700; color: #eef0f2; }"
                   "QLabel#aboutVersion { font-size: 13px; color: #9aa0a8; }"
                   "QLabel#aboutDescription { font-size: 13px; color: #c7cbd1; }"
                   "QLabel#aboutDevLabel { font-size: 13px; }"
                   "QLabel#aboutDevNames { font-size: 13px; color: #c7cbd1; }"
                   "QLabel#aboutLicense { font-size: 12px; color: #7a8087; }" );

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
    layout->setAlignment( Qt::AlignTop );

    // App name
    auto* appName = new QLabel( "Archer" );
    appName->setObjectName( "aboutAppName" );
    appName->setAlignment( Qt::AlignCenter );
    layout->addWidget( appName );

    // Version
    auto* version = new QLabel( QString( "Version %1" ).arg( ARCHER_VERSION ) );
    version->setObjectName( "aboutVersion" );
    version->setAlignment( Qt::AlignCenter );
    layout->addWidget( version );

    // Description
    auto* description = new QLabel( "The Archer application was made to help you install ALG "
                                    "and onboard quickly. It also serves as a tool for simple utility tasks." );
    description->setObjectName( "aboutDescription" );
    description->setWordWrap( true );
    // A fixed width (not just a maximum) - a word-wrapped QLabel with only setMaximumWidth() and
    // no other width constraint from its layout falls back to Qt's "roughly square" auto-wrap
    // heuristic instead of actually using the full 360px, wrapping far narrower than intended.
    description->setFixedWidth( 360 );
    description->setAlignment( Qt::AlignCenter );

    // Centered via a stretch-wrapped row rather than an addWidget() alignment flag: combining a
    // word-wrapped, max-width label with that flag breaks Qt's heightForWidth calculation and
    // makes the label overlap the widget below it.
    auto* descriptionRow = new QHBoxLayout();
    descriptionRow->addStretch();
    descriptionRow->addWidget( description );
    descriptionRow->addStretch();
    layout->addLayout( descriptionRow );

    // Developers label - flush left against the content area, not centered as a block
    auto* devLabel = new QLabel( "<b>Developers:</b>" );
    devLabel->setObjectName( "aboutDevLabel" );
    devLabel->setAlignment( Qt::AlignLeft );
    layout->addWidget( devLabel );

    // Developer names
    auto* devNames = new QLabel( "ALG Core Team" );
    devNames->setObjectName( "aboutDevNames" );
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

    // License
    auto* licenseText = new QLabel( "Distributed under the MIT License." );
    licenseText->setObjectName( "aboutLicense" );
    licenseText->setWordWrap( true );
    licenseText->setAlignment( Qt::AlignCenter );
    layout->addWidget( licenseText );

    // content fills the remaining space below the header but packs its own children at the top
    // (via the AlignTop set on `layout` above), matching the design: the text sits near the top
    // with modest padding, not centered in - or stretched across - the whole window.
    outer->addWidget( content, 1 );
}
