#include "ManipulatableGraphicsScene.h"

#include "CaseCreator/Utilities/Interfaces.h"

ManipulatableGraphicsScene::ManipulatableGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    this->pManipulationSurface = NULL;
}


void ManipulatableGraphicsScene::SetManipulationSurface(IManipulationSurface *pManipulationSurface)
{
    if (pManipulationSurface != NULL)
    {
        QObject::connect(pManipulationSurface, SIGNAL(SelectionChanged(IManipulatable*,IManipulatableObject*)), this, SLOT(OnSurfaceSelectionChanged(IManipulatable*,IManipulatableObject*)));
    }

    this->pManipulationSurface = pManipulationSurface;
    this->pSelectedObject = NULL;
}

void ManipulatableGraphicsScene::OnSurfaceSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject)
{
    if (pSelectedObject != pNewSelectedObject)
    {
        pSelectedManipulatable = pNewSelectedManipulatable;
        pSelectedObject = pNewSelectedObject;

        emit SelectionChanged(pSelectedManipulatable, pSelectedObject);
    }
}

void ManipulatableGraphicsScene::enterEvent(QEnterEvent */*pEvent*/)
{
    if (pManipulationSurface != NULL)
    {
        pManipulationSurface->OnMouseEnter();
    }
}

void ManipulatableGraphicsScene::leaveEvent(QEvent */*pEvent*/)
{
    if (pManipulationSurface != NULL)
    {
        pManipulationSurface->OnMouseLeave();
    }
}

void ManipulatableGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *pEvent)
{
    QGraphicsScene::mousePressEvent(pEvent);
    pEvent->accept();

    if (pManipulationSurface != NULL && pEvent->button() == Qt::LeftButton)
    {
        pManipulationSurface->OnMousePress(Vector2(pEvent->scenePos()));
    }
}

void ManipulatableGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *pEvent)
{
    QGraphicsScene::mouseMoveEvent(pEvent);
    pEvent->accept();

    if (pManipulationSurface != NULL)
    {
        pManipulationSurface->OnMouseMove(Vector2(pEvent->scenePos()), pEvent->buttons() & Qt::LeftButton);
    }
}

void ManipulatableGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *pEvent)
{
    QGraphicsScene::mouseReleaseEvent(pEvent);
    pEvent->accept();

    if (pManipulationSurface != NULL && pEvent->button() == Qt::LeftButton)
    {
        pManipulationSurface->OnMouseRelease(Vector2(pEvent->scenePos()));
    }
}

void ManipulatableGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *pEvent)
{
    QGraphicsScene::mouseDoubleClickEvent(pEvent);
    pEvent->accept();

    if (pManipulationSurface != NULL && pEvent->button() == Qt::LeftButton)
    {
        pManipulationSurface->OnMouseDoubleClick(Vector2(pEvent->scenePos()));
    }
}
