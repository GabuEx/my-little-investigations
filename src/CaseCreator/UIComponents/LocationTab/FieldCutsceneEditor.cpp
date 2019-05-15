#include "FieldCutsceneEditor.h"

#include "CaseCreator/CaseContent/Encounter.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

#include <QListWidget>
#include <QTreeWidget>

FieldCutsceneEditor::FieldCutsceneEditor(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *pMainLayout = new QGridLayout();
    QGridLayout *pTopLayout = new QGridLayout();
    QGridLayout *pTitleLayout = new QGridLayout();

    QLabel *pTitleLabel = new QLabel("Cutscene");

    pTitleLayout->addWidget(pTitleLabel, 0, 0);

    pMainLayout->addLayout(pTopLayout, 0, 0);
    pMainLayout->setRowStretch(0, 0);

    pObjectListWidgetHolder = new QVBoxLayout();

    pMainLayout->addLayout(pObjectListWidgetHolder, 1, 0);
    pMainLayout->setRowStretch(1, 1);

    pCurrentObjectListWidget = NULL;
    pSelectedPhase = NULL;

    setLayout(pMainLayout);
    setMinimumWidth(300);
}

void FieldCutsceneEditor::Init(FieldCutscene *pObject)
{
    pObject->UpdateAndCacheCutsceneStates();

    if (pCurrentObjectListWidget != NULL)
    {
        QObject::disconnect(pCurrentObjectListWidget, SIGNAL(ObjectSelected(void*)), this, SLOT(CutsceneListPhaseSelected(void*)));
        QObject::disconnect(pCurrentObjectListWidget, SIGNAL(ObjectEditingComplete(void*,void*,int,bool,int,int)), this, SLOT(CutsceneListPhaseEditingComplete(void*,void*,int,bool,int,int)));
        QObject::disconnect(pCurrentObjectListWidget, SIGNAL(ObjectDeleteRequested(void*,void*,int)), this, SLOT(CutsceneListPhaseDeleteRequested(void*,void*,int)));
        pObjectListWidgetHolder->removeWidget(pCurrentObjectListWidget);
        pCurrentObjectListWidget->deleteLater();
    }

    pCurrentObjectListWidget = pObject->GetObjectListWidget();
    pObjectListWidgetHolder->addWidget(pCurrentObjectListWidget);
    QObject::connect(pCurrentObjectListWidget, SIGNAL(ObjectSelected(void*)), this, SLOT(CutsceneListPhaseSelected(void*)));
    QObject::connect(pCurrentObjectListWidget, SIGNAL(ObjectEditingComplete(void*,void*,int,bool,int,int)), this, SLOT(CutsceneListPhaseEditingComplete(void*,void*,int,bool,int,int)));
    QObject::connect(pCurrentObjectListWidget, SIGNAL(ObjectDeleteRequested(void*,void*,int)), this, SLOT(CutsceneListPhaseDeleteRequested(void*,void*,int)));

    // Simulate a selection on the currently selected Phase in order to ensure we're in a good state.
    CutsceneListPhaseSelected(pCurrentObjectListWidget->GetSelectedObject());

    pFieldCutscene = pObject;
}

void FieldCutsceneEditor::Reset()
{
}

void FieldCutsceneEditor::Update(bool reselectPhase)
{
    if (pFieldCutscene != NULL)
    {
        pFieldCutscene->UpdateAndCacheCutsceneStates();
    }

    if (pCurrentObjectListWidget != NULL)
    {
        pCurrentObjectListWidget->UpdateListItemStatesBeforeObject();
    }

    if (pSelectedPhase != NULL && reselectPhase)
    {
        CutsceneListPhaseSelected(pSelectedPhase);
    }
}

void FieldCutsceneEditor::CutsceneListPhaseSelected(void *pPhase)
{
    CutsceneListPhaseSelectedImpl(reinterpret_cast<FieldCutscene::FieldCutscenePhase *>(pPhase));
}

void FieldCutsceneEditor::CutsceneListPhaseEditingComplete(
        void *pPhase,
        void *pContainingPhaseList,
        int positionInList,
        bool shouldInsert,
        int indentLevel,
        int rowIndex)
{
    CutsceneListPhaseEditingCompleteImpl(
                reinterpret_cast<FieldCutscene::FieldCutscenePhase *>(pPhase),
                reinterpret_cast<QList<FieldCutscene::FieldCutscenePhase *> *>(pContainingPhaseList),
                positionInList,
                shouldInsert,
                indentLevel,
                rowIndex);
}

void FieldCutsceneEditor::CutsceneListPhaseDeleteRequested(
        void *pPhase,
        void *pContainingPhaseList,
        int positionInList)
{
    CutsceneListPhaseDeleteRequestedImpl(
                reinterpret_cast<FieldCutscene::FieldCutscenePhase *>(pPhase),
                reinterpret_cast<QList<FieldCutscene::FieldCutscenePhase *> *>(pContainingPhaseList),
                positionInList);
}

void FieldCutsceneEditor::CutsceneListPhaseSelectedImpl(FieldCutscene::FieldCutscenePhase *pPhase)
{
    pSelectedPhase = pPhase;
    emit FieldCutscenePhaseSelected(pSelectedPhase);
}

void FieldCutsceneEditor::CutsceneListPhaseEditingCompleteImpl(
        FieldCutscene::FieldCutscenePhase *pPhase,
        QList<FieldCutscene::FieldCutscenePhase *> *pContainingPhaseList,
        int positionInList,
        bool shouldInsert,
        int indentLevel,
        int rowIndex)
{
    // If pPhase is non-null, then that means we need to regenerate our UI.
    // Otherwise, we can just re-run caching and update the selected item text.
    if (pPhase != NULL)
    {
        if (!shouldInsert)
        {
            FieldCutscene::FieldCutscenePhase *pOldPhase = pContainingPhaseList->at(positionInList);
            pContainingPhaseList->removeAt(positionInList);
            delete pOldPhase;
        }

        bool wasPreviouslyEmpty = pContainingPhaseList->empty();
        bool insertAtStart = positionInList == 0;
        pContainingPhaseList->insert(positionInList, pPhase);

        pSelectedPhase = pPhase;

        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> listItems = pPhase->GetListItems(indentLevel);

        if (shouldInsert)
        {
            // Make sure we clean everything up with the separators before we modify the UI tree.
            pCurrentObjectListWidget->EditorHolderItemEntered(NULL, 0);

            // If we're inserting at the beginning of the list,
            // then we'll bump up the row index by one in order
            // to put the new list items after the initial separator.
            if (positionInList == 0)
            {
                rowIndex++;
            }

            // If we're inserting into an empty list, then we'll add
            // a new bottom separator item.
            // If we're inserting at the start of the list,
            // then we'll put the separator after the new list items.
            // Otherwise, we'll put it before them.
            if (wasPreviouslyEmpty)
            {
                listItems.append(new BottomSeparatorListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(indentLevel, pContainingPhaseList, positionInList + 1, shouldInsert));
            }
            else if (insertAtStart)
            {
                listItems.append(new MiddleSeparatorListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(indentLevel, pContainingPhaseList, positionInList + 1, shouldInsert));
            }
            else
            {
                listItems.insert(0, new MiddleSeparatorListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(indentLevel, pContainingPhaseList, positionInList, shouldInsert));
            }

            pCurrentObjectListWidget->BeginListItemChanges();
            pCurrentObjectListWidget->InsertListItems(rowIndex, listItems, false /* replaceInitialItem */);
            pCurrentObjectListWidget->NotifyListItemChangesComplete();
        }
        else
        {
            pCurrentObjectListWidget->ReplaceSelectedListItem(listItems);
        }

        pPhase->GetListItemBase()->SetContainingObjectList(pContainingPhaseList, positionInList, false /* shouldInsert */);
    }
    else
    {
        pCurrentObjectListWidget->UpdateSelectedListItem();
    }

    Update(true /* reselectPhase */);
}


void FieldCutsceneEditor::CutsceneListPhaseDeleteRequestedImpl(
        FieldCutscene::FieldCutscenePhase *pPhase,
        QList<FieldCutscene::FieldCutscenePhase *> *pContainingPhaseList,
        int positionInList)
{
    FieldCutscene::FieldCutscenePhase *pOldPhase = pContainingPhaseList->at(positionInList);

    if (pOldPhase != pPhase)
    {
        throw new MLIException("Phase list is out of sync - we're deleting a phase that isn't in the position we think it's in.");
    }

    pContainingPhaseList->removeAt(positionInList);
    delete pOldPhase;

    bool shouldRemovePreviousSeparator =
            positionInList == pContainingPhaseList->length() &&
            positionInList != 0;

    if (pContainingPhaseList->empty())
    {
        pSelectedPhase = NULL;
    }
    else
    {
        pSelectedPhase =
                positionInList == pContainingPhaseList->length() ?
                    pContainingPhaseList->at(pContainingPhaseList->length() - 1) :
                    pContainingPhaseList->at(positionInList);
    }

    pCurrentObjectListWidget->DeleteSelectedListItem(shouldRemovePreviousSeparator);

    Update(true /* reselectPhase */);
}
