#include "HtmlDelegate.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPainter>

void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget != NULL ? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(optionV4.rect.width());
    const_cast<QAbstractItemModel *>(index.model())->setData(index, QVariant(option.rect.width()), WidthRole);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    painter->save();

    QVariant topHeightModifier = index.model()->data(index, TopHeightModifierRole);
    QVariant heightPadding = index.model()->data(index, HeightPaddingRole);

    painter->translate(textRect.topLeft() + QPoint(0, qvariant_cast<qreal>(topHeightModifier) + qvariant_cast<qreal>(heightPadding)));
    painter->setClipRect(textRect.translated(-textRect.topLeft() - QPoint(0, qvariant_cast<qreal>(topHeightModifier) + qvariant_cast<qreal>(heightPadding))));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    QVariant width = index.model()->data(index, WidthRole);
    doc.setTextWidth(qvariant_cast<qreal>(width));

    QVariant topHeightModifier = index.model()->data(index, TopHeightModifierRole);
    QVariant bottomHeightModifier = index.model()->data(index, BottomHeightModifierRole);
    QVariant heightPadding = index.model()->data(index, HeightPaddingRole);

    return QSize(
                doc.idealWidth(),
                doc.size().height() +
                    qvariant_cast<qreal>(topHeightModifier) +
                    qvariant_cast<qreal>(bottomHeightModifier) +
                    qvariant_cast<qreal>(heightPadding) * 2);
}
