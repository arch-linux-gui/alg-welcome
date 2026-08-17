#ifndef PAGECHROME_H
#define PAGECHROME_H

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

inline SubHeader
buildSubHeader( const QString& title, QWidget* parent = nullptr )
{
    auto* container = new QWidget( parent );
    container->setObjectName( "subHeader" );
    // Set directly (rather than relying on the app-wide stylesheet) so this bar always renders
    // consistently dark even on a page - like ThemePage - that overrides its own background:
    // a widget's own style sheet takes precedence over an ancestor's for its whole subtree.
    container->setStyleSheet( "QWidget#subHeader { background-color: #24262b; "
                              "border-bottom: 1px solid #37393e; }"
                              "QPushButton#backButton { background: none; border: none; color: #8fb1ff; "
                              "font-size: 13px; padding: 4px 6px; }"
                              "QPushButton#backButton:hover { background: none; border: none; color: #a9c3ff; }"
                              "QLabel#subHeaderTitle { background: none; color: #eef0f2; font-size: 14px; "
                              "font-weight: 600; }" );

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

    return { container, backButton };
}

}  // namespace PageChrome

#endif  // PAGECHROME_H
