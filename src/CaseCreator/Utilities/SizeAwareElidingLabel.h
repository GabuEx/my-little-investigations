#ifndef SIZEAWAREELIDINGLABEL_H
#define SIZEAWAREELIDINGLABEL_H

#include <QLabel>

class SizeAwareElidingLabel : public QLabel
{
public:
    SizeAwareElidingLabel(Qt::TextElideMode elideMode, QWidget *parent = 0, Qt::WindowFlags f = 0);

    void SetText(const QString &text);

protected:
    void resizeEvent(QResizeEvent * event) override;

private:
    Qt::TextElideMode elideMode;
    QString fullText;
};

#endif // SIZEAWAREELIDINGLABEL_H
