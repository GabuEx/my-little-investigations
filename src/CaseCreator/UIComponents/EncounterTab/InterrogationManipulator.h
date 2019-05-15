#ifndef INTERROGATIONMANIPULATOR_H
#define INTERROGATIONMANIPULATOR_H

#include "../TemplateHelpers/ObjectManipulator.h"

class Interrogation;
class ConversationEditor;

class InterrogationManipulator : public ObjectManipulator<Interrogation>
{
    Q_OBJECT
public:
    explicit InterrogationManipulator(QWidget *parent = 0);

    void Init(Interrogation *pObject);
    void Reset();

signals:

public slots:

private:
    ConversationEditor *pConversationEditor;
};

#endif // INTERROGATIONMANIPULATOR_H
