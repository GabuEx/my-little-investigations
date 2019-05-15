#ifndef HTMLDELEGATE_H
#define HTMLDELEGATE_H

#include <QStyledItemDelegate>

#define WidthRole (Qt::UserRole + 1)
#define ListItemRole (Qt::UserRole + 2)
#define TopHeightModifierRole (Qt::UserRole + 3)
#define BottomHeightModifierRole (Qt::UserRole + 4)
#define HeightPaddingRole (Qt::UserRole + 5)

class HtmlDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

#endif // HTMLDELEGATE_H
