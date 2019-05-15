#include "LocationCutscenesPage.h"

#include <QVBoxLayout>

LocationCutscenesPage::LocationCutscenesPage(QWidget *parent)
    : Page<Location>(parent)
{
    isActive = false;

    QVBoxLayout *pMainLayout = new QVBoxLayout();

    pCutsceneManipulator = new ListManipulator<FieldCutscene>();
    pMainLayout->addWidget(pCutsceneManipulator);

    setLayout(pMainLayout);
}

void LocationCutscenesPage::Init(Location *pObject)
{
    Page<Location>::Init(pObject);
    pCutsceneManipulator->SetParentObject(pObject);
}

void LocationCutscenesPage::Reset()
{
    pCutsceneManipulator->Reset();
}

bool LocationCutscenesPage::Undo()
{
    return pCutsceneManipulator->Undo();
}

bool LocationCutscenesPage::Redo()
{
    return pCutsceneManipulator->Redo();
}

bool LocationCutscenesPage::GetIsActive()
{
    return isActive;
}

void LocationCutscenesPage::SetIsActive(bool isActive)
{
    if (this->isActive != isActive)
    {
        this->isActive = isActive;
        pCutsceneManipulator->SetIsActive(isActive);
    }
}
