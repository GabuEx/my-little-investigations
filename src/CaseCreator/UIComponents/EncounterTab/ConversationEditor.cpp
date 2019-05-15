#include "ConversationEditor.h"

#include "CaseCreator/CaseContent/Encounter.h"
#include "CaseCreator/Utilities/AudioPlayer.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

#include <QListWidget>
#include <QTreeWidget>

ConversationEditor::ConversationEditor(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *pMainLayout = new QGridLayout();
    QGridLayout *pTopLayout = new QGridLayout();
    QGridLayout *pTitleLayout = new QGridLayout();

    //QLabel *pTitleLabel = new QLabel("Conversation");

    pTitleLayout->addWidget(new QWidget(), 0, 0);

    QHBoxLayout *pButtonLayout = new QHBoxLayout();

    pPlayButton = new QPushButton("Play");
    pStopButton = new QPushButton("Stop");

    QObject::connect(pPlayButton, SIGNAL(clicked()), this, SLOT(PlayButtonClicked()));
    QObject::connect(pStopButton, SIGNAL(clicked()), this, SLOT(StopButtonClicked()));

    pButtonLayout->addWidget(pPlayButton);
    pButtonLayout->addWidget(pStopButton);

    pTitleLayout->addLayout(pButtonLayout, 1, 0);

    pTopLayout->addLayout(pTitleLayout, 0, 0);
    pTopLayout->setColumnStretch(0, 1);

    pConversationView = new QGraphicsView();
    pConversationScene = new QGraphicsScene(0, 0, 960, 540);

    pConversationView->setScene(pConversationScene);
    pConversationView->scale(0.5, 0.5);

    pTopLayout->addWidget(pConversationView, 0, 1);
    pTopLayout->setColumnStretch(0, 1);
    pTopLayout->setColumnMinimumWidth(1, 480);
    pTopLayout->setRowMinimumHeight(0, 270);

    pMainLayout->addLayout(pTopLayout, 0, 0);
    pMainLayout->setRowStretch(0, 0);

    pObjectListWidgetHolder = new QVBoxLayout();

    pMainLayout->addLayout(pObjectListWidgetHolder, 1, 0);
    pMainLayout->setRowStretch(1, 1);

    pConversationDrawingView = NULL;
    pCurrentObjectListWidget = NULL;
    pSelectedAction = NULL;

    setLayout(pMainLayout);
}

void ConversationEditor::Init(Conversation *pObject)
{
    pObject->UpdateAndCacheConversationStates();

    AudioPlayer::UnloadSounds();
    AudioPlayer::UnloadDialog();
    pObject->PreloadAudio();

    pConversationDrawingView = pObject->GetDrawingView(NULL);
    pConversationDrawingView->Draw(pConversationScene);

    if (pCurrentObjectListWidget != NULL)
    {
        QObject::disconnect(pCurrentObjectListWidget, SIGNAL(ObjectSelected(void*)), this, SLOT(ConversationListActionSelected(void*)));
        QObject::disconnect(pCurrentObjectListWidget, SIGNAL(ObjectEditingComplete(void*,void*,int,bool,int,int)), this, SLOT(ConversationListActionEditingComplete(void*,void*,int,bool,int,int)));
        QObject::disconnect(pCurrentObjectListWidget, SIGNAL(ObjectDeleteRequested(void*,void*,int)), this, SLOT(ConversationListActionDeleteRequested(void*,void*,int)));
        pObjectListWidgetHolder->removeWidget(pCurrentObjectListWidget);
        pCurrentObjectListWidget->deleteLater();
    }

    pCurrentObjectListWidget = pObject->GetObjectListWidget();
    pObjectListWidgetHolder->addWidget(pCurrentObjectListWidget);
    QObject::connect(pCurrentObjectListWidget, SIGNAL(ObjectSelected(void*)), this, SLOT(ConversationListActionSelected(void*)));
    QObject::connect(pCurrentObjectListWidget, SIGNAL(ObjectEditingComplete(void*,void*,int,bool,int,int)), this, SLOT(ConversationListActionEditingComplete(void*,void*,int,bool,int,int)));
    QObject::connect(pCurrentObjectListWidget, SIGNAL(ObjectDeleteRequested(void*,void*,int)), this, SLOT(ConversationListActionDeleteRequested(void*,void*,int)));

    // Simulate a selection on the currently selected action in order to ensure we're in a good state.
    ConversationListActionSelected(pCurrentObjectListWidget->GetSelectedObject());

    pConversation = pObject;
}

void ConversationEditor::Reset()
{
    if (pSelectedAction != NULL)
    {
        pSelectedAction->StopPlaying();
    }

    if (pConversationDrawingView != NULL)
    {
        pConversationDrawingView->Reset();
        pConversationDrawingView->deleteLater();
        pConversationDrawingView = NULL;
    }
}

void ConversationEditor::PlayButtonClicked()
{
    if (pSelectedAction != NULL)
    {
        pSelectedAction->PlayOnDrawingView(pConversationDrawingView);
    }
}

void ConversationEditor::StopButtonClicked()
{
    if (pSelectedAction != NULL)
    {
        pSelectedAction->StopPlaying();
    }
}

void ConversationEditor::ConversationListActionSelected(void *pAction)
{
    ConversationListActionSelectedImpl(reinterpret_cast<Conversation::Action *>(pAction));
}

void ConversationEditor::ConversationListActionEditingComplete(
        void *pAction,
        void *pContainingActionList,
        int positionInList,
        bool shouldInsert,
        int indentLevel,
        int rowIndex)
{
    ConversationListActionEditingCompleteImpl(
                reinterpret_cast<Conversation::Action *>(pAction),
                reinterpret_cast<QList<Conversation::Action *> *>(pContainingActionList),
                positionInList,
                shouldInsert,
                indentLevel,
                rowIndex);
}

void ConversationEditor::ConversationListActionDeleteRequested(
        void *pAction,
        void *pContainingActionList,
        int positionInList)
{
    ConversationListActionDeleteRequestedImpl(
                reinterpret_cast<Conversation::Action *>(pAction),
                reinterpret_cast<QList<Conversation::Action *> *>(pContainingActionList),
                positionInList);
}

void ConversationEditor::ConversationListActionSelectedImpl(Conversation::Action *pAction)
{
    if (pSelectedAction != NULL)
    {
        pSelectedAction->StopPlaying();
    }

    pSelectedAction = pAction;

    if (pSelectedAction != NULL && pSelectedAction->CanPlay())
    {
        pPlayButton->show();
        pStopButton->show();
        pPlayButton->setEnabled(true);
        pStopButton->setEnabled(true);
    }
    else
    {
        pPlayButton->hide();
        pStopButton->hide();
        pPlayButton->setEnabled(false);
        pStopButton->setEnabled(false);
    }

    if (pAction != NULL)
    {
        pAction->PushToDrawingView(pConversationDrawingView);
    }
}

void ConversationEditor::ConversationListActionEditingCompleteImpl(
        Conversation::Action *pAction,
        QList<Conversation::Action *> *pContainingActionList,
        int positionInList,
        bool shouldInsert,
        int indentLevel,
        int rowIndex)
{
    // If pAction is non-null, then that means we need to regenerate our UI.
    // Otherwise, we can just re-run caching and update the selected item text.
    if (pAction != NULL)
    {
        if (!shouldInsert)
        {
            Conversation::Action *pOldAction = pContainingActionList->at(positionInList);
            pContainingActionList->removeAt(positionInList);
            delete pOldAction;
        }

        bool wasPreviouslyEmpty = pContainingActionList->empty();
        bool insertAtStart = positionInList == 0;
        pContainingActionList->insert(positionInList, pAction);

        pSelectedAction = pAction;

        QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> listItems = pAction->GetListItems(indentLevel);

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
                listItems.append(new BottomSeparatorListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel, pContainingActionList, positionInList + 1, shouldInsert));
            }
            else if (insertAtStart)
            {
                listItems.append(new MiddleSeparatorListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel, pContainingActionList, positionInList + 1, shouldInsert));
            }
            else
            {
                listItems.insert(0, new MiddleSeparatorListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel, pContainingActionList, positionInList, shouldInsert));
            }

            pCurrentObjectListWidget->BeginListItemChanges();
            pCurrentObjectListWidget->InsertListItems(rowIndex, listItems, false /* replaceInitialItem */);
            pCurrentObjectListWidget->NotifyListItemChangesComplete();
        }
        else
        {
            pCurrentObjectListWidget->ReplaceSelectedListItem(listItems);
        }

        pAction->GetListItemBase()->SetContainingObjectList(pContainingActionList, positionInList, false /* shouldInsert */);
    }
    else
    {
        pCurrentObjectListWidget->UpdateSelectedListItem();
    }

    pConversation->UpdateAndCacheConversationStates();
    pCurrentObjectListWidget->UpdateListItemStatesBeforeObject();
    ConversationListActionSelected(pSelectedAction);
}


void ConversationEditor::ConversationListActionDeleteRequestedImpl(
        Conversation::Action *pAction,
        QList<Conversation::Action *> *pContainingActionList,
        int positionInList)
{
    Conversation::Action *pOldAction = pContainingActionList->at(positionInList);

    if (pOldAction != pAction)
    {
        throw new MLIException("Action list is out of sync - we're deleting an action that isn't in the position we think it's in.");
    }

    pContainingActionList->removeAt(positionInList);
    delete pOldAction;

    bool shouldRemovePreviousSeparator =
            positionInList == pContainingActionList->length() &&
            positionInList != 0;

    if (pContainingActionList->empty())
    {
        pSelectedAction = NULL;
    }
    else
    {
        pSelectedAction =
                positionInList == pContainingActionList->length() ?
                    pContainingActionList->at(pContainingActionList->length() - 1) :
                    pContainingActionList->at(positionInList);
    }

    pCurrentObjectListWidget->DeleteSelectedListItem(shouldRemovePreviousSeparator);

    pConversation->UpdateAndCacheConversationStates();
    pCurrentObjectListWidget->UpdateListItemStatesBeforeObject();
    ConversationListActionSelected(pSelectedAction);
}
