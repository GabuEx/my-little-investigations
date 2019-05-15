#ifndef MANIPULATABLEGRAPHICSVIEW_H
#define MANIPULATABLEGRAPHICSVIEW_H

#include <QGraphicsView>

#include <QEvent>

class ManipulatableGraphicsScene;

class IManipulatable;
class IManipulatableObject;

class ManipulatableGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ManipulatableGraphicsView(QWidget *parent = 0);

    ~ManipulatableGraphicsView()
    {
        this->pManipulatableGraphicsScene = NULL;
        this->pSelectedObject = NULL;
    }

    void SetScene(ManipulatableGraphicsScene *pManipulatableGraphicsScene);

signals:
    void SelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);

public slots:
    void OnSceneSelectionChanged(IManipulatable *pNewSelectedManipulatable, IManipulatableObject *pNewSelectedObject);

protected:
    void enterEvent(QEvent *pEvent);
    void leaveEvent(QEvent *pEvent);

private:
    ManipulatableGraphicsScene *pManipulatableGraphicsScene;

    IManipulatable *pSelectedManipulatable;
    IManipulatableObject *pSelectedObject;
};

#endif // MANIPULATABLEGRAPHICSVIEW_H
