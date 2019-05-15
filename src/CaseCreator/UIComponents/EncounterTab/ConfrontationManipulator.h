#ifndef CONFRONTATIONMANIPULATOR_H
#define CONFRONTATIONMANIPULATOR_H

#include "../TemplateHelpers/ObjectManipulator.h"

class Confrontation;
class ConversationEditor;

class ConfrontationManipulator : public ObjectManipulator<Confrontation>
{
    Q_OBJECT
public:
    explicit ConfrontationManipulator(QWidget *parent = 0);

    void Init(Confrontation *pObject);
    void Reset();

signals:

public slots:

private:
    ConversationEditor *pConversationEditor;
};

#endif // CONFRONTATIONMANIPULATOR_H
