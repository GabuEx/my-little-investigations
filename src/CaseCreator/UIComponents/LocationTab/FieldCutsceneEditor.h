#ifndef FIELDCUTSCENEEDITOR_H
#define FIELDCUTSCENEEDITOR_H

#include <QWidget>
#include <QPushButton>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QVBoxLayout>

#include <QItemSelection>

#include "CaseCreator/CaseContent/FieldCutscene.h"

class FieldCutsceneEditor : public QWidget
{
    Q_OBJECT
public:
    explicit FieldCutsceneEditor(QWidget *parent = 0);

    void Init(FieldCutscene *pObject);
    void Reset();

    void Update(bool reselectPhase);

signals:
    void FieldCutscenePhaseSelected(FieldCutscene::FieldCutscenePhase *pSelectedPhase);

public slots:
    void CutsceneListPhaseSelected(void *pPhase);

    void CutsceneListPhaseEditingComplete(
            void *pPhase,
            void *pContainingPhaseList,
            int positionInList,
            bool shouldInsert,
            int indentLevel,
            int rowIndex);

    void CutsceneListPhaseDeleteRequested(
            void *pPhase,
            void *pContainingPhaseList,
            int positionInList);

private:
    void CutsceneListPhaseSelectedImpl(FieldCutscene::FieldCutscenePhase *pPhase);

    void CutsceneListPhaseEditingCompleteImpl(
            FieldCutscene::FieldCutscenePhase *pPhase,
            QList<FieldCutscene::FieldCutscenePhase *> *pContainingPhaseList,
            int positionInList,
            bool shouldInsert,
            int indentLevel,
            int rowIndex);

    void CutsceneListPhaseDeleteRequestedImpl(
            FieldCutscene::FieldCutscenePhase *pPhase,
            QList<FieldCutscene::FieldCutscenePhase *> *pContainingPhaseList,
            int positionInList);

    QVBoxLayout *pObjectListWidgetHolder;
    FieldCutscenePhaseListWidget *pCurrentObjectListWidget;

    FieldCutscene *pFieldCutscene;
    FieldCutscene::FieldCutscenePhase *pSelectedPhase;
};

#endif // FIELDCUTSCENEEDITOR_H
