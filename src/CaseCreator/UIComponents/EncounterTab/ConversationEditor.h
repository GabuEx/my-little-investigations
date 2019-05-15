#ifndef CONVERSATIONEDITOR_H
#define CONVERSATIONEDITOR_H

#include <QWidget>
#include <QPushButton>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QVBoxLayout>

#include <QItemSelection>

#include "CaseCreator/CaseContent/Conversation.h"

class ConversationEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ConversationEditor(QWidget *parent = 0);

    void Init(Conversation *pObject);
    void Reset();

signals:

public slots:
    void PlayButtonClicked();
    void StopButtonClicked();

    void ConversationListActionSelected(void *pAction);

    void ConversationListActionEditingComplete(
            void *pAction,
            void *pContainingActionList,
            int positionInList,
            bool shouldInsert,
            int indentLevel,
            int rowIndex);

    void ConversationListActionDeleteRequested(
            void *pAction,
            void *pContainingActionList,
            int positionInList);

private:
    void ConversationListActionSelectedImpl(Conversation::Action *pAction);

    void ConversationListActionEditingCompleteImpl(
            Conversation::Action *pAction,
            QList<Conversation::Action *> *pContainingActionList,
            int positionInList,
            bool shouldInsert,
            int indentLevel,
            int rowIndex);

    void ConversationListActionDeleteRequestedImpl(
            Conversation::Action *pAction,
            QList<Conversation::Action *> *pContainingActionList,
            int positionInList);

    QVBoxLayout *pObjectListWidgetHolder;
    ObjectListWidget<Conversation::Action, Conversation::State, Conversation::ActionType> *pCurrentObjectListWidget;

    QGraphicsView *pConversationView;
    QGraphicsScene *pConversationScene;

    Conversation::DrawingView *pConversationDrawingView;

    QPushButton *pPlayButton;
    QPushButton *pStopButton;
    Conversation *pConversation;
    Conversation::Action *pSelectedAction;
};

#endif // CONVERSATIONEDITOR_H
