#ifndef PAGECHROME_H
#define PAGECHROME_H

#include "AppPalette.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

// Small shared "<- Back    Title    " bar used at the top of every in-window page
// (Mirrorlist config/log, Theme, About) that isn't the home page.
namespace PageChrome
{

struct SubHeader
{
    QWidget* widget;
    QPushButton* backButton;
};

// Set directly (rather than relying on the app-wide stylesheet) so this bar always renders
// consistently even on a page - like ThemePage - that overrides its own background: a widget's
// own style sheet takes precedence over an ancestor's for its whole subtree. Callable again after
// construction (with the container returned by buildSubHeader()) to re-theme it live.
inline void
styleSubHeader( QWidget* container, const AppPalette::Palette& palette )
{
    container->setStyleSheet( QString( "QWidget#subHeader { background-color: %1; "
                                       "border-bottom: 1px solid %1; }"
                                       "QPushButton#backButton { background: none; border: none; color: %2; "
                                       "font-size: 13px; padding: 4px 6px; }"
                                       "QPushButton#backButton:hover { background: none; border: none; }"
                                       "QLabel#subHeaderTitle { background: none; color: %3; font-size: 14px; "
                                       "font-weight: 600; }" )
                                  .arg( palette.chromeBg, palette.chromeAccent, palette.chromeText ) );
}

inline SubHeader
buildSubHeader( const QString& title, const AppPalette::Palette& palette, QWidget* parent = nullptr )
{
    auto* container = new QWidget( parent );
    container->setObjectName( "subHeader" );

    auto* layout = new QHBoxLayout( container );
    layout->setContentsMargins( 14, 8, 14, 8 );

    auto* backButton = new QPushButton( QString::fromUtf8( "\xE2\x80\xB9 Back" ) );
    backButton->setObjectName( "backButton" );
    backButton->setFlat( true );
    backButton->setCursor( Qt::PointingHandCursor );
    backButton->setFocusPolicy( Qt::NoFocus );

    auto* titleLabel = new QLabel( title );
    titleLabel->setObjectName( "subHeaderTitle" );
    titleLabel->setAlignment( Qt::AlignCenter );

    auto* spacer = new QWidget( container );
    spacer->setFixedWidth( 60 );

    layout->addWidget( backButton );
    layout->addWidget( titleLabel, 1 );
    layout->addWidget( spacer );

    styleSubHeader( container, palette );

    return { container, backButton };
}

}  // namespace PageChrome

#endif  // PAGECHROME_H
