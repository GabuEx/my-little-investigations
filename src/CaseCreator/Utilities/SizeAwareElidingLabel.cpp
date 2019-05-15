#include "SizeAwareElidingLabel.h"

#include <QFontMetrics>

SizeAwareElidingLabel::SizeAwareElidingLabel(Qt::TextElideMode elideMode, QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    this->elideMode = elideMode;
}

void SizeAwareElidingLabel::SetText(const QString &text)
{
    //QLabel::setText("");
    //QLabel::setText(QFontMetrics(font()).elidedText(fullText, elideMode, width()));
    fullText = text;
    QLabel::setText(text);
}

void SizeAwareElidingLabel::resizeEvent(QResizeEvent * /*event*/)
{
    SetText(fullText);
}
