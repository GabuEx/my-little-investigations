#include "LocationForegroundElementsPage.h"

#include <QScrollBar>
#include <QVBoxLayout>

LocationForegroundElementsPage::LocationForegroundElementsPage(QWidget *parent) :
    Page<Location>(parent)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pForegroundElementManipulator = new ListManipulator<ForegroundElement>();
    pMainLayout->addWidget(pForegroundElementManipulator);

    setLayout(pMainLayout);
}

void LocationForegroundElementsPage::Init(Location *pObject)
{
    Page<Location>::Init(pObject);
    pForegroundElementManipulator->SetParentObject(pObject);
}

void LocationForegroundElementsPage::Reset()
{
}
