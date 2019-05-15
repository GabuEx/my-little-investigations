#ifndef PAGE_H
#define PAGE_H

#include <QWidget>

class PageSlots : public QWidget
{
    Q_OBJECT

public:
    explicit PageSlots(QWidget *parent = 0) : QWidget(parent) { }

public slots:
};

template <class T>
class Page : public PageSlots
{
public:
    explicit Page(QWidget *parent = 0) : PageSlots(parent) { }

    // Called when a page is about to be shown.
    virtual void Init(T *pObject)
    {
        this->pObject = pObject;
    }

    // Called when a page is ceasing to be shown.
    virtual void Reset() = 0;

    virtual bool Undo() { return false; }
    virtual bool Redo() { return false; }

    virtual bool GetIsActive() { return false; }
    virtual void SetIsActive(bool) { }

private:
    T *pObject;
};

#endif // PAGE_H
