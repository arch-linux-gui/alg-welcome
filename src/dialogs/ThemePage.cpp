#include "ThemePage.h"
#include "PageChrome.h"

#include <spdlog/spdlog.h>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

namespace
{

QString
displayNameForDesktopEnv( const QString& desktopEnv )
{
    if ( desktopEnv == "kde" )
    {
        return "KDE Plasma";
    }
    if ( desktopEnv == "gnome" )
    {
        return "GNOME";
    }
    if ( desktopEnv == "xfce" )
    {
        return "Xfce";
    }
    return desktopEnv;
}

// Small stand-in preview: two overlapping rounded "window" swatches on a background tinted to
// roughly match the preset's light/dark variant - just enough to tell cards apart at a glance.
QPixmap
buildPreviewPixmap( bool dark )
{
    const QSize size( 130, 90 );
    QPixmap pixmap( size );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing );

    QPainterPath background;
    background.addRoundedRect( QRectF( 0, 0, size.width(), size.height() ), 8, 8 );
    painter.fillPath( background, QColor( dark ? "#1c1e22" : "#eef0f4" ) );

    QPainterPath rectA;
    rectA.addRoundedRect( QRectF( 14, 16, 70, 44 ), 6, 6 );
    painter.fillPath( rectA, QColor( dark ? "#2f3237" : "#ffffff" ) );

    QPainterPath rectB;
    rectB.addRoundedRect( QRectF( 52, 34, 50, 32 ), 6, 6 );
    painter.fillPath( rectB, QColor( dark ? "#3a3d42" : "#f4f5f7" ) );

    return pixmap;
}

}  // namespace

ThemePage::ThemePage( const QString& desktopEnv, QWidget* parent )
    : QWidget( parent )
    , desktopEnv( desktopEnv )
    , themeManager( Themes::getThemeManager( desktopEnv ) )
{
    if ( themeManager )
    {
        setupUI();
        rebuildCards();
    }
    else
    {
        setupUnsupportedUI();
    }
}

void
ThemePage::setupUI()
{
    setObjectName( "themePage" );
    setStyleSheet( "#themePage { background-color: #f4f5f7; }"
                   "#themePage QLabel { color: #2b2e33; }"
                   "#themePage QLabel#deCaption { color: #8a8f99; font-size: 11px; font-weight: 700; "
                   "letter-spacing: 1px; }"
                   "#themePage QLabel#deBadge { background-color: #e2ebff; color: #3d6fe0; font-size: 11px; "
                   "font-weight: 600; padding: 3px 10px; border-radius: 9px; }"
                   "#themePage QLabel#familyLabel { font-size: 13px; font-weight: 700; }"
                   "#themePage QLabel#themeNote { color: #9aa0a8; font-style: italic; font-size: 11px; }"
                   "#themePage QToolButton { border: 2px solid transparent; border-radius: 8px; padding: 6px; "
                   "font-size: 12px; font-weight: 600; color: #5a5d63; background: transparent; }"
                   "#themePage QToolButton:checked { border: 2px solid #3d7eff; color: #3d7eff; }" );

    auto* outer = new QVBoxLayout( this );
    outer->setContentsMargins( 0, 0, 0, 0 );
    outer->setSpacing( 0 );

    const auto header = PageChrome::buildSubHeader( "Set System Theme", this );
    connect( header.backButton, &QPushButton::clicked, this, &ThemePage::backRequested );
    outer->addWidget( header.widget );

    auto* content = new QWidget();
    auto* contentLayout = new QVBoxLayout( content );
    contentLayout->setContentsMargins( 24, 20, 24, 20 );
    contentLayout->setSpacing( 18 );

    auto* deRow = new QHBoxLayout();
    deRow->setSpacing( 10 );
    auto* deCaption = new QLabel( "DESKTOP ENVIRONMENT" );
    deCaption->setObjectName( "deCaption" );
    auto* deBadge = new QLabel( QString( "Detected: %1" ).arg( displayNameForDesktopEnv( desktopEnv ) ) );
    deBadge->setObjectName( "deBadge" );
    deRow->addWidget( deCaption );
    deRow->addWidget( deBadge );
    deRow->addStretch();
    contentLayout->addLayout( deRow );

    groupsLayout = new QVBoxLayout();
    groupsLayout->setSpacing( 18 );
    contentLayout->addLayout( groupsLayout );
    contentLayout->addStretch();

    outer->addWidget( content, 1 );

    confirmBar = new QWidget();
    confirmBar->setObjectName( "themeConfirmBar" );
    confirmBar->setStyleSheet( "#themeConfirmBar { background-color: #24262b; }"
                               "#themeConfirmBar QLabel { color: #eef0f2; }" );
    auto* confirmLayout = new QHBoxLayout( confirmBar );
    confirmLayout->setContentsMargins( 18, 12, 18, 12 );

    confirmLabel = new QLabel();
    confirmLayout->addWidget( confirmLabel, 1 );

    auto* cancelButton = new QPushButton( "Cancel" );
    connect( cancelButton, &QPushButton::clicked, this, &ThemePage::hideConfirmBar );
    confirmLayout->addWidget( cancelButton );

    auto* applyButton = new QPushButton( "Apply" );
    applyButton->setObjectName( "primaryButton" );
    connect( applyButton,
             &QPushButton::clicked,
             [ this ]()
             {
                 if ( pendingPresetId.isEmpty() )
                 {
                     return;
                 }
                 spdlog::info( "Applying theme preset: {}", pendingPresetId.toStdString() );
                 themeManager->applyPreset( pendingPresetId );
                 appliedPresetId = pendingPresetId;
                 hideConfirmBar();
                 Q_EMIT toastRequested( "Theme applied" );
             } );
    confirmLayout->addWidget( applyButton );

    confirmBar->hide();
    outer->addWidget( confirmBar );
}

void
ThemePage::setupUnsupportedUI()
{
    auto* outer = new QVBoxLayout( this );
    outer->setContentsMargins( 0, 0, 0, 0 );
    outer->setSpacing( 0 );

    const auto header = PageChrome::buildSubHeader( "Set System Theme", this );
    connect( header.backButton, &QPushButton::clicked, this, &ThemePage::backRequested );
    outer->addWidget( header.widget );

    auto* label = new QLabel( QString( "Theme switching isn't supported on the detected desktop environment (%1)." )
                                  .arg( desktopEnv.isEmpty() ? QString( "unknown" ) : desktopEnv ) );
    label->setWordWrap( true );
    label->setContentsMargins( 24, 20, 24, 20 );
    outer->addWidget( label );
    outer->addStretch();
}

void
ThemePage::rebuildCards()
{
    appliedPresetId = themeManager->currentPresetId();
    pendingPresetId.clear();

    cardGroup = new QButtonGroup( this );
    connect( cardGroup,
             &QButtonGroup::idClicked,
             [ this ]( int )
             {
                 auto* button = qobject_cast< QToolButton* >( cardGroup->checkedButton() );
                 if ( !button )
                 {
                     return;
                 }
                 selectPreset( button->property( "presetId" ).toString() );
             } );

    const auto presets = themeManager->availablePresets();

    QString currentFamily;
    QHBoxLayout* cardsRow = nullptr;
    QVBoxLayout* familySection = nullptr;

    for ( const auto& preset : presets )
    {
        if ( preset.family != currentFamily )
        {
            currentFamily = preset.family;

            familySection = new QVBoxLayout();
            familySection->setSpacing( 10 );

            auto* familyLabel = new QLabel( currentFamily );
            familyLabel->setObjectName( "familyLabel" );
            familySection->addWidget( familyLabel );

            cardsRow = new QHBoxLayout();
            cardsRow->setSpacing( 14 );
            familySection->addLayout( cardsRow );

            if ( currentFamily == "ALG Theme" )
            {
                auto* note = new QLabel( "Includes McMojave cursors and Tela Circle icons." );
                note->setObjectName( "themeNote" );
                familySection->addWidget( note );
            }

            groupsLayout->addLayout( familySection );
        }

        auto* card = new QToolButton();
        card->setCheckable( true );
        card->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
        card->setIconSize( QSize( 130, 90 ) );
        card->setIcon( buildPreviewPixmap( preset.isDark ) );
        card->setText( preset.displayName );
        card->setProperty( "presetId", preset.id );
        card->setChecked( preset.id == appliedPresetId );
        card->setCursor( Qt::PointingHandCursor );
        card->setFocusPolicy( Qt::NoFocus );

        cardGroup->addButton( card );
        cardsRow->addWidget( card );
    }
}

void
ThemePage::selectPreset( const QString& id )
{
    if ( id == appliedPresetId )
    {
        hideConfirmBar();
        return;
    }
    showConfirmBar( id );
}

void
ThemePage::showConfirmBar( const QString& id )
{
    pendingPresetId = id;

    QString label = id;
    for ( const auto& preset : themeManager->availablePresets() )
    {
        if ( preset.id == id )
        {
            label = preset.displayName;
            break;
        }
    }

    confirmLabel->setText( QString( "Apply \"%1\" theme?" ).arg( label ) );
    confirmBar->show();
}

void
ThemePage::hideConfirmBar()
{
    pendingPresetId.clear();
    confirmBar->hide();

    // Re-check whichever card matches the currently-applied preset (undoes a tentative click).
    for ( auto* button : cardGroup->buttons() )
    {
        auto* toolButton = qobject_cast< QToolButton* >( button );
        if ( toolButton && toolButton->property( "presetId" ).toString() == appliedPresetId )
        {
            toolButton->setChecked( true );
            break;
        }
    }
}
