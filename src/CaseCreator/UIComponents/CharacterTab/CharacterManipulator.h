#ifndef CHARACTERMANIPULATOR_H
#define CHARACTERMANIPULATOR_H

#include <QWidget>

#include <QGraphicsScene>
#include <QTimer>

#include "../TemplateHelpers/ObjectManipulator.h"
#include "../BaseTypes/PageTabWidget.h"

class Character;

class CharacterManipulator : public ObjectManipulator<Character>
{
    Q_OBJECT
public:
    explicit CharacterManipulator(QWidget *parent = 0);
    ~CharacterManipulator();

    void Init(Character *pObject);
    void Reset();
    bool Undo();
    bool Redo();

    bool GetIsActive();
    void SetIsActive(bool isActive);

private:
    PageTabWidget<Character> *pTabWidget;

    bool isActive;
};

#endif // CHARACTERMANIPULATOR_H
