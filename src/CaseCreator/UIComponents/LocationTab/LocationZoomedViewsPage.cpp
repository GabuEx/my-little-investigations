#include "LocationZoomedViewsPage.h"

#include <QVBoxLayout>

LocationZoomedViewsPage::LocationZoomedViewsPage(QWidget *parent)
    : Page<Location>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pZoomedViewManipulator = new ListManipulator<ZoomedView>();
    pMainLayout->addWidget(pZoomedViewManipulator);

    setLayout(pMainLayout);
}

void LocationZoomedViewsPage::Init(Location *pObject)
{
    Page<Location>::Init(pObject);
    pZoomedViewManipulator->SetParentObject(pObject);
}

void LocationZoomedViewsPage::Reset()
{
    pZoomedViewManipulator->Reset();
}

bool LocationZoomedViewsPage::Undo()
{
    return pZoomedViewManipulator->Undo();
}

bool LocationZoomedViewsPage::Redo()
{
    return pZoomedViewManipulator->Redo();
}

bool LocationZoomedViewsPage::GetIsActive()
{
    return isActive;
}

void LocationZoomedViewsPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pZoomedViewManipulator->SetIsActive(isActive);
    }
}
