#include "ManipulatableGraphicsView.h"

#include "ManipulatableGraphicsScene.h"

#include <QEnterEvent>

ManipulatableGraphicsView::ManipulatableGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    this->pManipulatableGraphicsScene = NULL;
}

void ManipulatableGraphicsView::SetScene(ManipulatableGraphicsScene *pManipulatableGraphicsScene)
{
    if (this->pManipulatableGraphicsScene != NULL)
    {
        QObject::disconnect(this->pManipulatableGraphicsScene, NULL, NULL, NULL);
    }

    QObject::connect(pManipulatableGraphicsScene, SIGNAL(SelectionChanged(IManipulatable*,IManipulatableObject*)), this, SLOT(OnSceneSelectionChanged(IManipulatable*,IManipulatableObject*)));

    this->pManipulatableGraphicsScene = pManipulatableGraphicsScene;
    setScene(pManipulatableGraphicsScene);

    this->pSelectedObject = NULL;
}

void ManipulatableGraphicsView::OnSceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject)
{
    if (pSelectedObject != pNewSelectedObject)
    {
        pSelectedManipulatable = pNewSelectedManipulatable;
        pSelectedObject = pNewSelectedObject;

        emit SelectionChanged(pSelectedManipulatable, pSelectedObject);
    }
}

void ManipulatableGraphicsView::enterEvent(QEvent *pEvent)
{
    QGraphicsView::enterEvent(pEvent);
    pEvent->accept();

    if (pManipulatableGraphicsScene != NULL)
    {
        pManipulatableGraphicsScene->enterEvent(static_cast<QEnterEvent*>(pEvent));
    }
}

void ManipulatableGraphicsView::leaveEvent(QEvent *pEvent)
{
    QGraphicsView::leaveEvent(pEvent);
    pEvent->accept();

    if (pManipulatableGraphicsScene != NULL)
    {
        pManipulatableGraphicsScene->leaveEvent(pEvent);
    }
}
