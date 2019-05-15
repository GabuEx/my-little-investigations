#ifndef CHARACTERFIELDSPRITEMANIPULATOR_H
#define CHARACTERFIELDSPRITEMANIPULATOR_H

#include "../BaseTypes/MultipleSelectionWidget.h"
#include "../TemplateHelpers/ObjectManipulator.h"
#include "CaseCreator/CaseContent/Character.h"

#include <QGraphicsView>
#include <QGraphicsScene>

class CharacterFieldSpriteManipulator : public ObjectManipulator<Character::FieldSprite>
{
    Q_OBJECT

public:
    explicit CharacterFieldSpriteManipulator(QWidget *parent = 0);

    void Init(Character::FieldSprite *pObject);
    void Reset();

    virtual void PullChanges();

public slots:
    void DownSpriteFilePathsSelectionButtonClicked();
    void DiagonalDownSpriteFilePathsSelectionButtonClicked();
    void SideSpriteFilePathsSelectionButtonClicked();
    void DiagonalUpSpriteFilePathsSelectionButtonClicked();
    void UpSpriteFilePathsSelectionButtonClicked();

    void DownFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void DownFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void DownFrameSpriteFilePathSelectorStringRemoved(int index);
    void DiagonalDownFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void DiagonalDownFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void DiagonalDownFrameSpriteFilePathSelectorStringRemoved(int index);
    void SideFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void SideFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void SideFrameSpriteFilePathSelectorStringRemoved(int index);
    void DiagonalUpFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void DiagonalUpFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void DiagonalUpFrameSpriteFilePathSelectorStringRemoved(int index);
    void UpFrameSpriteFilePathSelectorStringChanged(int index, const QString &newString);
    void UpFrameSpriteFilePathSelectorStringAdded(int index, const QString &newString);
    void UpFrameSpriteFilePathSelectorStringRemoved(int index);

private:
    QStringList GetMultipleImages();
    void ReloadDrawingViews();

    QGraphicsView *pSpriteGraphicsView;
    QGraphicsScene *pSpriteGraphicsScene;

    Animation::CompositeDrawingView *pAnimationDrawingView;

    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pDownFrameSpriteFilePathSelector;
    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pDiagonalDownFrameSpriteFilePathSelector;
    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pSideFrameSpriteFilePathSelector;
    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pDiagonalUpFrameSpriteFilePathSelector;
    MultipleSelectionWidget<FilePathMultipleSelectionSelectorWidget> *pUpFrameSpriteFilePathSelector;
};

#endif // CHARACTERFIELDSPRITEMANIPULATOR_H
