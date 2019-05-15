#ifndef PAGETABWIDGET_H
#define PAGETABWIDGET_H

#include <QTabWidget>

#include <QList>

#include "Page.h"

class PageTabWidgetSlots : public QTabWidget
{
    Q_OBJECT
public:
    explicit PageTabWidgetSlots(QWidget *parent = 0) :
        QTabWidget(parent)
    {
        QObject::connect(this, SIGNAL(currentChanged(int)), this, SLOT(OnPageChangedSlot(int)));
    }

public slots:
    void OnPageChangedSlot(int newPageIndex)
    {
        OnPageChanged(newPageIndex);
    }

protected:
    virtual void OnPageChanged(int newPageIndex) = 0;
};

template <class T>
class PageTabWidget : public PageTabWidgetSlots
{
public:
    explicit PageTabWidget(QWidget *parent = 0) :
        PageTabWidgetSlots(parent)
    {
        currentSelectedPage = 0;
        pObject = NULL;
        isActive = false;
    }

    void Init(T *pObject)
    {
        this->pObject = pObject;

        if (pageList.count() > currentSelectedPage)
        {
            pageList[currentSelectedPage]->Init(pObject);
        }
    }

    void Reset()
    {
        pageList[currentSelectedPage]->Reset();
    }

    bool Undo()
    {
        return pageList[currentSelectedPage]->Undo();
    }

    bool Redo()
    {
        return pageList[currentSelectedPage]->Redo();
    }

    void AddPage(Page<T> *pPage, QString tabTitle)
    {
        pageList.push_back(pPage);
        addTab(pPage, tabTitle);
    }

    bool GetIsActive()
    {
        return isActive;
    }

    void SetIsActive(bool isActive)
    {
        if (isActive != this->isActive)
        {
            this->isActive = isActive;
            pageList[currentSelectedPage]->SetIsActive(isActive);
        }
    }

protected:
    void OnPageChanged(int newPageIndex)
    {
        if (currentSelectedPage != newPageIndex)
        {
            pageList[currentSelectedPage]->Reset();
            pageList[currentSelectedPage]->SetIsActive(false);
            pageList[newPageIndex]->Init(pObject);
            pageList[newPageIndex]->SetIsActive(isActive);

            currentSelectedPage = newPageIndex;
        }
    }

private:
    QList<Page<T> *> pageList;
    int currentSelectedPage;

    T *pObject;

    bool isActive;
};

#endif // PAGETABWIDGET_H
