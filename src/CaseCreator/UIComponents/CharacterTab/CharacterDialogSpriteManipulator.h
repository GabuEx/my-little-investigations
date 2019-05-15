#ifndef CHARACTERDIALOGSPRITEMANIPULATOR_H
#define CHARACTERDIALOGSPRITEMANIPULATOR_H

#include "../BaseTypes/MultipleSelectionWidget.h"
#include "../TemplateHelpers/ObjectManipulator.h"
#include "CaseCreator/CaseContent/Character.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>

class CharacterDialogSpriteManipulator : public ObjectManipulator<Character::DialogEmotion>
{
    Q_OBJECT

public:
    explicit CharacterDialogSpriteManipulator(QWidget *parent = 0);

    void Init(Character::DialogEmotion *pObject);
    void Reset();

    virtual void PullChanges();

public slots:
    void BaseSpriteFilePathSelectionButtonClicked();
    void EyeSpriteFilePathsSelectionButtonClicked();
    void MouthSpriteFilePathsSelectionButtonClicked();

    void EyeSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void EyeSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void EyeSpriteFilePathSelectorStringRemoved(int index);

    void MouthSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void MouthSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void MouthSpriteFilePathSelectorStringRemoved(int index);

private:
    QStringList GetMultipleImages();
    void ReloadDrawingViews();

    QGraphicsView *pSilentSpriteGraphicsView;
    QGraphicsScene *pSilentSpriteGraphicsScene;
    QGraphicsView *pTalkingSpriteGraphicsView;
    QGraphicsScene *pTalkingSpriteGraphicsScene;

    Character::DialogEmotion::DrawingView *pSilentSpriteDrawingView;
    Character::DialogEmotion::DrawingView *pTalkingSpriteDrawingView;

    QLabel *pBaseSpriteFilePathLabel;
    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pEyeSpriteFilePathSelector;
    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pMouthSpriteFilePathSelector;
};

#endif // CHARACTERDIALOGSPRITEMANIPULATOR_H
