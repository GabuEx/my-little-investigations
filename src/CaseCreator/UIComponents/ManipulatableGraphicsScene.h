#ifndef MANIPULATABLEGRAPHICSSCENE_H
#define MANIPULATABLEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QEnterEvent>

class IManipulationSurface;

class IManipulatable;
class IManipulatableObject;

class ManipulatableGraphicsScene : public QGraphicsScene
{
    friend class ManipulatableGraphicsView;

    Q_OBJECT

public:
    explicit ManipulatableGraphicsScene(QObject *parent = 0);

    ~ManipulatableGraphicsScene()
    {
        this->pManipulationSurface = NULL;
        this->pSelectedObject = NULL;
    }

    void SetManipulationSurface(IManipulationSurface *pManipulationSurface);

signals:
    void SelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);

public slots:
    void OnSurfaceSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);

protected:
    void enterEvent(QEnterEvent *pEvent);
    void leaveEvent(QEvent *pEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *pEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *pEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *pEvent) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *pEvent) override;

private:
    IManipulationSurface *pManipulationSurface;

    IManipulatable *pSelectedManipulatable;
    IManipulatableObject *pSelectedObject;
};

#endif // MANIPULATABLEGRAPHICSSCENE_H
