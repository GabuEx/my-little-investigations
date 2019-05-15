#ifndef CONVERSATIONMANIPULATOR_H
#define CONVERSATIONMANIPULATOR_H

#include "../TemplateHelpers/ObjectManipulator.h"

class Conversation;
class ConversationEditor;

class ConversationManipulator : public ObjectManipulator<Conversation>
{
    Q_OBJECT
public:
    explicit ConversationManipulator(QWidget *parent = 0);

    void Init(Conversation *pObject);
    void Reset();

signals:

public slots:

private:
    ConversationEditor *pConversationEditor;
};

#endif // CONVERSATIONMANIPULATOR_H
