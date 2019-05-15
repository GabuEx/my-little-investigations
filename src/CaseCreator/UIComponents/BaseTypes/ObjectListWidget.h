#ifndef OBJECTLISTWIDGET_H
#define OBJECTLISTWIDGET_H

#include <QStack>

#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>

#include <cstddef>

#include "CaseCreator/UIComponents/TemplateHelpers/NewObjectDialog.h"

#include "Vector2.h"

QString UnderlineString(const QString &s);
QString ConcatenateStringList(const QStringList &stringList);
QString VectorToString(const Vector2 &v);
void SetOpacity(QWidget *pWidget, qreal opacity);

template <class TState, class TObjectTypeEnum>
class ListItemObject
{
public:
    virtual TObjectTypeEnum GetType() = 0;
    virtual void ExchangeListItemBaseOwnership(ListItemObject *pOther) = 0;
    virtual void CopyProperties(ListItemObject *pOther, bool isForEdit) = 0;
    virtual TState & GetStateDuringObject() = 0;

    virtual bool IsSameType(ListItemObject *pOther)
    {
        return GetType() == pOther->GetType();
    }
};

template <class TObject, class TState>
class EditorDialogContents : public QWidget
{
public:
    EditorDialogContents(const TState &stateBeforeObject, TObject *pOriginalObject);
    virtual ~EditorDialogContents();

    TObject * DetachObject();
    void ClearObject();

    virtual bool ValidateFields() = 0;
    virtual void OnAccepted() { }

protected:
    virtual void FinalizeObject() { }

    TObject *pObject;
    TObject *pOriginalObject;

    TState stateBeforeObject;
};

template <class TObject, class TState, class TObjectTypeEnum>
class EditorDialog : public NewObjectDialog<TObject>
{
public:
    EditorDialog(const TState &stateBeforeObject, TObject *pObjectToEdit = NULL);
    ~EditorDialog();

    TObject * GetNewObject() override;
    void ClearNewObject();

protected:
    void ObjectTypeComboBoxCurrentIndexChangedImpl(const QString &selectedText) override;

    void InitFields() override { }
    bool ValidateFields() override;
    void OnAccepted() override;

private:
    QMap<TObjectTypeEnum, EditorDialogContents<TObject, TState> *> contentsByObjectTypeMap;

    QGridLayout *pContentsLayout;
    EditorDialogContents<TObject, TState> *pCurrentContents;

    TState stateBeforeObject;

    QMap<QString, TObjectTypeEnum> objectTypeByComboBoxStringMap;
    QMap<TObjectTypeEnum, EditorDialogContents<TObject, TState> * (*)(const TState &, TObject *)> editorDialogContentsCreationMethodsByTypeMap;
};

template <class TObject>
QString GetObjectDisplayName();

template <class TObject, class TObjectTypeEnum>
QMap<QString, TObjectTypeEnum> GetTypeByComboBoxStringMap();

template <class TObject, class TState, class TObjectTypeEnum>
QMap<TObjectTypeEnum, EditorDialogContents<TObject, TState> * (*)(const TState &, TObject *)> GetEditorDialogContentsCreationMethodsByTypeMap();

class ListItemTreeWidget : public QTreeWidget
{
public:
    ListItemTreeWidget() : QTreeWidget() {}

    QModelIndex IndexFromItem(QTreeWidgetItem *pItem)
    {
        return indexFromItem(pItem);
    }
};

enum class ListItemSeparatorPosition
{
    Top,
    Middle,
    Bottom
};

template <class TAdjacentObject>
class ListItemSeparator : public QTreeWidgetItem
{
public:
    ListItemSeparator(ListItemTreeWidget *pParentTreeWidget, ListItemSeparatorPosition position);

    void OnMouseOver();
    void OnMouseLeave();

private:
    static QString GetSeparatorString();
    static QString GetAddNewObjectString();

    bool isMouseOver;

    ListItemTreeWidget *pParentTreeWidget;
    ListItemSeparatorPosition position;
};

class ListItemSlots : public QObject
{
    Q_OBJECT

public:
    explicit ListItemSlots(QObject *parent = 0) : QObject(parent) {}

signals:
    void Selected(ListItemSlots *pSender);
};

template <class TObject, class TState, class TObjectTypeEnum>
class ListItemBase : public ListItemSlots
{
public:
    explicit ListItemBase(int indentLevel);

    int GetIndentLevel() { return indentLevel; }
    virtual bool GetIsPlaceholder() { return false; }
    virtual bool GetCanHaveChildren() { return false; }

    virtual TObject * GetObject()
    {
        return NULL;
    }

    virtual void SetObject(TObject */*pObject*/)
    {
    }

    void SetStateBeforeObject(const TState &stateBeforeObject)
    {
        this->stateBeforeObject = stateBeforeObject.Clone();
    }

    QList<TObject *> * GetContainingObjectList() { return pContainingObjectList; }
    int GetPositionInList() { return positionInList; }
    bool GetShouldInsert() { return shouldInsert; }

    void SetContainingObjectList(QList<TObject *> *pContainingObjectList, int positionInList, bool shouldInsert);
    void AdjustPositionInList(int adjustmentAmount) { positionInList += adjustmentAmount; }

    QTreeWidgetItem * GetTreeWidgetItem(ListItemTreeWidget *pParentTreeWidget);
    void SetTreeWidgetItem(QTreeWidgetItem *pTreeWidgetItem);

    virtual void UpdateTreeWidgetItem(ListItemTreeWidget */*pParentTreeWidget*/) { }
    static ListItemBase<TObject, TState, TObjectTypeEnum> * FromTreeWidgetItem(QTreeWidgetItem *pItem);

    virtual EditorDialog<TObject, TState, TObjectTypeEnum> * GetEditorDialog();

protected:
    virtual QTreeWidgetItem * GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget) = 0;

    TState stateBeforeObject;

private:
    int indentLevel;
    QTreeWidgetItem *pTreeWidgetItem;

    QList<TObject *> *pContainingObjectList;
    int positionInList;
    bool shouldInsert;
};

template <class TObject, class TState, class TObjectTypeEnum>
class ListItem : public ListItemBase<TObject, TState, TObjectTypeEnum>
{
public:
    explicit ListItem(int indentLevel) : ListItemBase<TObject, TState, TObjectTypeEnum>(indentLevel) { }

    TObject * GetObject() override
    {
        return GetObjectImpl();
    }

    virtual void SetObject(TObject *pObject)
    {
        SetObjectImpl(pObject);
    }

    void UpdateTreeWidgetItem(ListItemTreeWidget *pParentTreeWidget) override;

    virtual QString GetDisplayString() = 0;
    EditorDialog<TObject, TState, TObjectTypeEnum> * GetEditorDialog() override;

protected:
    virtual TObject * GetObjectImpl() = 0;
    virtual void SetObjectImpl(TObject *pObject) = 0;

    QTreeWidgetItem * GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget) override;
};

template <class TAdjacentObject, class TAdjacentObjectState, class TAdjacentObjectTypeEnum>
class TopSeparatorListItem : public ListItemBase<TAdjacentObject, TAdjacentObjectState, TAdjacentObjectTypeEnum>
{
public:
    explicit TopSeparatorListItem(int indentLevel, QList<TAdjacentObject *> *pContainingObjectList, int positionInList, bool shouldInsert);

protected:
    QTreeWidgetItem * GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget) override;
};

template <class TAdjacentObject, class TAdjacentObjectState, class TAdjacentObjectTypeEnum>
class MiddleSeparatorListItem : public ListItemBase<TAdjacentObject, TAdjacentObjectState, TAdjacentObjectTypeEnum>
{
public:
    explicit MiddleSeparatorListItem(int indentLevel, QList<TAdjacentObject *> *pContainingObjectList, int positionInList, bool shouldInsert);

protected:
    QTreeWidgetItem * GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget) override;
};

template <class TAdjacentObject, class TAdjacentObjectState, class TAdjacentObjectTypeEnum>
class BottomSeparatorListItem : public ListItemBase<TAdjacentObject, TAdjacentObjectState, TAdjacentObjectTypeEnum>
{
public:
    explicit BottomSeparatorListItem(int indentLevel, QList<TAdjacentObject *> *pContainingObjectList, int positionInList, bool shouldInsert);

protected:
    QTreeWidgetItem * GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget) override;
};

template <class TAdjacentObject, class TAdjacentObjectState, class TAdjacentObjectTypeEnum>
class TextDisplay : public ListItem<TAdjacentObject, TAdjacentObjectState, TAdjacentObjectTypeEnum>
{
public:
    explicit TextDisplay(QString textToDisplay, int indentLevel)
        : ListItem<TAdjacentObject, TAdjacentObjectState, TAdjacentObjectTypeEnum>(indentLevel)
    {
        this->textToDisplay = textToDisplay;
    }

    bool GetIsPlaceholder() override { return true; }
    bool GetCanHaveChildren() override { return true; }

    QString GetDisplayString() override
    {
        return QString("<span style='color: gray'><em>") + textToDisplay + "</em></span>";
    }

    TAdjacentObject * GetObjectImpl() override { return NULL; }
    void SetObjectImpl(TAdjacentObject * /*pObject*/) override { }

private:
    QString textToDisplay;
};

class ObjectListWidgetSlots : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectListWidgetSlots() : QWidget(NULL) { }

signals:
    void ObjectSelected(void *pObject);
    void ObjectEditingComplete(void *pObject, void *pContainingObjectList, int positionInList, bool shouldInsert, int indentLevel, int rowIndex);
    void ObjectDeleteRequested(void *pObject, void *pContainingObjectList, int positionInList);

public slots:
    void EditorHolderCurrentItemChanged(QTreeWidgetItem *pCurrentItem, QTreeWidgetItem *pPreviousItem)
    {
        EditorHolderCurrentItemChangedImpl(pCurrentItem, pPreviousItem);
    }

    void EditorHolderItemEntered(QTreeWidgetItem *pItem, int column)
    {
        EditorHolderItemEnteredImpl(pItem, column);
    }

    void EditorHolderItemDoubleClicked(QTreeWidgetItem *pItem, int column)
    {
        EditorHolderItemDoubleClickedImpl(pItem, column);
    }

protected:
    virtual void EditorHolderCurrentItemChangedImpl(QTreeWidgetItem * /*pCurrentItem*/, QTreeWidgetItem * /*pPreviousItem*/) { }
    virtual void EditorHolderItemEnteredImpl(QTreeWidgetItem * /*pItem*/, int /*column*/) { }
    virtual void EditorHolderItemDoubleClickedImpl(QTreeWidgetItem * /*pItem*/, int /*column*/) { }
};

template <class TObject, class TState, class TObjectTypeEnum>
class ObjectListWidget : public ObjectListWidgetSlots
{
    friend class ListItemSeparator<TObject>;

public:
    explicit ObjectListWidget();

    void BeginListItemChanges();
    void AddListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem);
    void AddListItems(const QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> &listItemList);
    void InsertListItems(int insertionPosition, const QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> &listItemList, bool replaceInitialItem);
    void NotifyListItemChangesComplete();

    void ReplaceSelectedListItem(const QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> &listItemList);
    void UpdateSelectedListItem();
    void DeleteSelectedListItem(bool shouldRemovePreviousSeparator);

    void UpdateListItemStatesBeforeObject();

    TObject * GetSelectedObject();

protected:
    void EditorHolderCurrentItemChangedImpl(QTreeWidgetItem *pCurrentItem, QTreeWidgetItem *pPreviousItem) override;
    void EditorHolderItemEnteredImpl(QTreeWidgetItem *pItem, int column) override;
    void EditorHolderItemDoubleClickedImpl(QTreeWidgetItem * pItem, int column) override;

    void leaveEvent(QEvent *pEvent) override;
    void keyPressEvent(QKeyEvent *pEvent) override;

private:
    void InsertItem(QTreeWidgetItem *pItem);

    void EditListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem);
    void DeleteListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem);

    bool FindAndRemoveListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem, bool removeFromListWidgetToo);

    QList<QTreeWidgetItem *> treeWidgetItems;
    QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> listItems;

    QList<QTreeWidgetItem *> newTreeWidgetItems;
    int topLevelItemCount;
    int currentLevelItemCount;
    QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> newListItems;

    int currentInsertionPosition;
    int lastIndentLevel;
    QTreeWidgetItem *pLastTreeWidgetItem;
    QStack<QTreeWidgetItem *> treeWidgetStack;
    QStack<int> currentLevelItemCountStack;
    TState currentState;

    int selectedListItemIndex;
    ListItemBase<TObject, TState, TObjectTypeEnum> *pSelectedListItem;
    ListItemSeparator<TObject> *pEnteredSeparator;

    ListItemTreeWidget *pListItemHolder;

    bool editingDialogIsOpen;
    bool onMouseLeaveQueued;
};

#endif // OBJECTLISTWIDGET_H
