#include "ObjectListWidget.h"

#include <QWidget>
#include <QComboBox>
#include <QPushButton>

#include <QHBoxLayout>

#include <QKeyEvent>

#include "CaseCreator/Utilities/HtmlDelegate.h"

#include "CaseCreator/CaseContent/Character.h"
#include "CaseCreator/CaseContent/FieldCutscene.h"

#include "MLIException.h"

QString UnderlineString(const QString &s)
{
    return QString("<span style='text-decoration: underline;'>") + s + QString("</span>");
}

QString ConcatenateStringList(const QStringList &stringList)
{
    QString concatenatedString = "";

    for (int i = 0; i < stringList.length(); i++)
    {
        concatenatedString += stringList.at(i);

        if (stringList.length() == 2 && i == 0)
        {
            concatenatedString += " or ";
        }
        else
        {
            if (i < stringList.length() - 1)
            {
                concatenatedString += ", ";
            }

            if (i == stringList.length() - 2)
            {
                concatenatedString += "or ";
            }
        }
    }

    return concatenatedString;
}

QString VectorToString(const Vector2 &v)
{
    char buffer[256];
    sprintf(&buffer[0], "(%.0f, %.0f)", v.GetX(), v.GetY());
    return QString(buffer);
}

void SetOpacity(QWidget *pWidget, qreal opacity)
{
    QGraphicsOpacityEffect *pOpacityEffect = dynamic_cast<QGraphicsOpacityEffect *>(pWidget->graphicsEffect());

    if (pOpacityEffect == NULL)
    {
        pOpacityEffect = new QGraphicsOpacityEffect();
        pWidget->setGraphicsEffect(pOpacityEffect);
    }

    pOpacityEffect->setOpacity(opacity);
    pWidget->setEnabled(opacity > 0.0);
}

template <class TObject, class TState>
EditorDialogContents<TObject, TState>::EditorDialogContents(const TState &stateBeforeObject, TObject *pOriginalObject)
    : QWidget()
{
    pObject = NULL;
    this->pOriginalObject = pOriginalObject;
    this->stateBeforeObject = stateBeforeObject.Clone();
}

template <class TObject, class TState>
EditorDialogContents<TObject, TState>::~EditorDialogContents()
{
    delete pObject;
    pObject = NULL;

    pOriginalObject = NULL;
}

template <class TObject, class TState>
TObject * EditorDialogContents<TObject, TState>::DetachObject()
{
    if (pOriginalObject != NULL && pOriginalObject->IsSameType(pObject))
    {
        FinalizeObject();

        pOriginalObject->ExchangeListItemBaseOwnership(pObject);
        pObject->CopyProperties(pOriginalObject, false /* isForEdit */);
        pObject->ExchangeListItemBaseOwnership(pOriginalObject);

        delete pObject;
        pObject = NULL;

        pOriginalObject = NULL;

        return NULL;
    }
    else
    {
        TObject *pReturnedObject = pObject;
        pObject = NULL;
        return pReturnedObject;
    }
}

template <class TObject, class TState>
void EditorDialogContents<TObject, TState>::ClearObject()
{
    delete pObject;
    pObject = NULL;
}

template <class TObject, class TState, class TObjectTypeEnum>
EditorDialog<TObject, TState, TObjectTypeEnum>::EditorDialog(const TState &stateBeforeObject, TObject *pObjectToEdit)
{
    this->stateBeforeObject = stateBeforeObject.Clone();

    objectTypeByComboBoxStringMap = GetTypeByComboBoxStringMap<TObject, TObjectTypeEnum>();
    editorDialogContentsCreationMethodsByTypeMap = GetEditorDialogContentsCreationMethodsByTypeMap<TObject, TState, TObjectTypeEnum>();

    TObjectTypeEnum initialType;
    pCurrentContents = NULL;

    QGridLayout *pMainLayout = new QGridLayout();

    QHBoxLayout *pTopLayout = new QHBoxLayout();
    pContentsLayout = new QGridLayout();
    QHBoxLayout *pButtonLayout = new QHBoxLayout();

    pMainLayout->addLayout(pTopLayout, 0, 0);
    pMainLayout->addLayout(pContentsLayout, 1, 0);
    pMainLayout->addLayout(pButtonLayout, 2, 0);

    QComboBox *pObjectSelectorComboBox = new QComboBox();
    pObjectSelectorComboBox->addItems(objectTypeByComboBoxStringMap.keys());
    pTopLayout->addWidget(pObjectSelectorComboBox);

    QPushButton *pOkButton = new QPushButton("OK");
    QPushButton *pCancelButton = new QPushButton("Cancel");

    QObject::connect(pOkButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    pButtonLayout->addWidget(pOkButton);
    pButtonLayout->addWidget(pCancelButton);

    pMainLayout->setRowStretch(0, 0);
    pMainLayout->setRowStretch(1, 1);
    pMainLayout->setRowStretch(2, 0);
    this->setLayout(pMainLayout);

    if (pObjectToEdit != NULL)
    {
        for (const QString &comboBoxText : objectTypeByComboBoxStringMap.keys())
        {
            if (objectTypeByComboBoxStringMap[comboBoxText] == pObjectToEdit->GetType())
            {
                pObjectSelectorComboBox->setCurrentText(comboBoxText);
                pCurrentContents = editorDialogContentsCreationMethodsByTypeMap[pObjectToEdit->GetType()](stateBeforeObject, pObjectToEdit);
                initialType = pObjectToEdit->GetType();
                break;
            }
        }

        this->setWindowTitle("Edit " + GetObjectDisplayName<TObject>());
    }
    else
    {
        this->setWindowTitle("Add new " + GetObjectDisplayName<TObject>());
    }

    if (pCurrentContents == NULL)
    {
        pObjectSelectorComboBox->setCurrentIndex(0);
        initialType = objectTypeByComboBoxStringMap[pObjectSelectorComboBox->itemText(0)];
        pCurrentContents = editorDialogContentsCreationMethodsByTypeMap[initialType](stateBeforeObject, NULL);
    }

    pContentsLayout->addWidget(pCurrentContents, 0, 0);
    contentsByObjectTypeMap.insert(initialType, pCurrentContents);
    QObject::connect(pObjectSelectorComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(ObjectTypeComboBoxCurrentIndexChanged(QString)));

    this->setSizeGripEnabled(false);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

template <class TObject, class TState, class TObjectTypeEnum>
EditorDialog<TObject, TState, TObjectTypeEnum>::~EditorDialog()
{
    for (TObjectTypeEnum objectType : contentsByObjectTypeMap.keys())
    {
        delete contentsByObjectTypeMap[objectType];
    }

    contentsByObjectTypeMap.clear();
}

template <class TObject, class TState, class TObjectTypeEnum>
TObject * EditorDialog<TObject, TState, TObjectTypeEnum>::GetNewObject()
{
    if (pCurrentContents != NULL)
    {
        return pCurrentContents->DetachObject();
    }
    else
    {
        return NULL;
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void EditorDialog<TObject, TState, TObjectTypeEnum>::ClearNewObject()
{
    if (pCurrentContents != NULL)
    {
        return pCurrentContents->ClearObject();
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void EditorDialog<TObject, TState, TObjectTypeEnum>::ObjectTypeComboBoxCurrentIndexChangedImpl(const QString &selectedText)
{
    pCurrentContents->hide();

    TObjectTypeEnum objectType = objectTypeByComboBoxStringMap[selectedText];

    if (contentsByObjectTypeMap.contains(objectType))
    {
        pCurrentContents = contentsByObjectTypeMap[objectType];
        pCurrentContents->show();
    }
    else
    {
        pCurrentContents = editorDialogContentsCreationMethodsByTypeMap[objectType](stateBeforeObject, NULL);
        pContentsLayout->addWidget(pCurrentContents, 0, 0);
        contentsByObjectTypeMap.insert(objectType, pCurrentContents);
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
bool EditorDialog<TObject, TState, TObjectTypeEnum>::ValidateFields()
{
    if (pCurrentContents != NULL)
    {
        return pCurrentContents->ValidateFields();
    }
    else
    {
        return false;
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void EditorDialog<TObject, TState, TObjectTypeEnum>::OnAccepted()
{
    if (pCurrentContents != NULL)
    {
        pCurrentContents->OnAccepted();
    }
}

template <class TAdjacentObject>
ListItemSeparator<TAdjacentObject>::ListItemSeparator(ListItemTreeWidget *pParentTreeWidget, ListItemSeparatorPosition position)
    : QTreeWidgetItem()
{
    isMouseOver = false;
    this->pParentTreeWidget = pParentTreeWidget;
    this->position = position;

    switch (position)
    {
    case ListItemSeparatorPosition::Top:
        setText(0, GetSeparatorString() + GetAddNewObjectString() + GetSeparatorString());
        break;

    case ListItemSeparatorPosition::Middle:
        setText(0, GetSeparatorString());
        break;

    case ListItemSeparatorPosition::Bottom:
        setText(0, GetSeparatorString() + GetAddNewObjectString() + GetSeparatorString());
        break;

    default:
        throw new MLIException("Unknown list item separator position.");
    }

    setFlags(Qt::ItemNeverHasChildren);
}

template <class TAdjacentObject>
void ListItemSeparator<TAdjacentObject>::OnMouseOver()
{
    if (position == ListItemSeparatorPosition::Middle && !isMouseOver)
    {
        QTreeWidgetItem *pParentItem = parent();
        QModelIndex indexInParentTreeWidget =
                pParentTreeWidget->IndexFromItem(this);
        int indexInParentItem =
                pParentItem != NULL ?
                    pParentItem->indexOfChild(this) :
                    pParentTreeWidget->indexOfTopLevelItem(this);
        int parentItemChildrenCount =
                pParentItem != NULL ?
                    pParentItem->childCount() :
                    pParentTreeWidget->topLevelItemCount();

        int heightBefore = pParentTreeWidget->sizeHintForIndex(indexInParentTreeWidget).height();

        isMouseOver = true;
        setText(0, GetSeparatorString() + GetAddNewObjectString() + GetSeparatorString());

        int heightAfter = pParentTreeWidget->sizeHintForIndex(indexInParentTreeWidget).height();

        if (indexInParentItem > 0 && indexInParentItem < parentItemChildrenCount - 1)
        {
            if (pParentItem != NULL)
            {
                pParentItem->child(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant((heightBefore - heightAfter) / 2));
                pParentItem->child(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant((heightBefore - heightAfter) / 2));
            }
            else
            {
                pParentTreeWidget->topLevelItem(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant((heightBefore - heightAfter) / 2));
                pParentTreeWidget->topLevelItem(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant((heightBefore - heightAfter) / 2));
            }
        }
        else if (indexInParentItem > 0)
        {
            if (pParentItem != NULL)
            {
                pParentItem->child(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant(heightBefore - heightAfter));
            }
            else
            {
                pParentTreeWidget->topLevelItem(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant(heightBefore - heightAfter));
            }
        }
        else if (indexInParentItem < parentItemChildrenCount)
        {
            if (pParentItem != NULL)
            {
                pParentItem->child(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant(heightBefore - heightAfter));
            }
            else
            {
                pParentTreeWidget->topLevelItem(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant(heightBefore - heightAfter));
            }
        }
    }
}

template <class TAdjacentObject>
void ListItemSeparator<TAdjacentObject>::OnMouseLeave()
{
    if (position == ListItemSeparatorPosition::Middle && isMouseOver)
    {
        QTreeWidgetItem *pParentItem = parent();
        int indexInParentItem =
                pParentItem != NULL ?
                    pParentItem->indexOfChild(this) :
                    pParentTreeWidget->indexOfTopLevelItem(this);
        int parentItemChildrenCount =
                pParentItem != NULL ?
                    pParentItem->childCount() :
                    pParentTreeWidget->topLevelItemCount();

        isMouseOver = false;
        setText(0, GetSeparatorString());

        if (indexInParentItem > 0 && indexInParentItem < parentItemChildrenCount - 1)
        {
            if (pParentItem != NULL)
            {
                pParentItem->child(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant(0));
                pParentItem->child(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant(0));
            }
            else
            {
                pParentTreeWidget->topLevelItem(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant(0));
                pParentTreeWidget->topLevelItem(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant(0));
            }
        }
        else if (indexInParentItem > 0)
        {
            if (pParentItem != NULL)
            {
                pParentItem->child(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant(0));
            }
            else
            {
                pParentTreeWidget->topLevelItem(indexInParentItem - 1)->setData(0, BottomHeightModifierRole, QVariant(0));
            }
        }
        else if (indexInParentItem < parentItemChildrenCount)
        {
            if (pParentItem != NULL)
            {
                pParentItem->child(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant(0));
            }
            else
            {
                pParentTreeWidget->topLevelItem(indexInParentItem + 1)->setData(0, TopHeightModifierRole, QVariant(0));
            }
        }
    }
}

template <class TAdjacentObject>
QString ListItemSeparator<TAdjacentObject>::GetSeparatorString()
{
    return "<hr />";
}

template <class TAdjacentObject>
QString ListItemSeparator<TAdjacentObject>::GetAddNewObjectString()
{
    return QString("<span style='color: gray;'><em>(Double-click to add a new ") + GetObjectDisplayName<TAdjacentObject>() + QString(" here.)</em></span>");
}

template <class TObject, class TState, class TObjectTypeEnum>
ListItemBase<TObject, TState, TObjectTypeEnum>::ListItemBase(int indentLevel)
    : ListItemSlots(NULL)
{
    this->indentLevel = indentLevel;
    this->pTreeWidgetItem = NULL;

    this->pContainingObjectList = NULL;
    this->positionInList = -1;
    this->shouldInsert = false;
}

template <class TObject, class TState, class TObjectTypeEnum>
void ListItemBase<TObject, TState, TObjectTypeEnum>::SetContainingObjectList(QList<TObject *> *pContainingObjectList, int positionInList, bool shouldInsert)
{
    this->pContainingObjectList = pContainingObjectList;
    this->positionInList = positionInList;
    this->shouldInsert = shouldInsert;
}

template <class TObject, class TState, class TObjectTypeEnum>
QTreeWidgetItem * ListItemBase<TObject, TState, TObjectTypeEnum>::GetTreeWidgetItem(ListItemTreeWidget *pParentTreeWidget)
{
    if (pTreeWidgetItem == NULL)
    {
        pTreeWidgetItem = GetTreeWidgetItemImpl(pParentTreeWidget);
        pTreeWidgetItem->setData(0, ListItemRole, QVariant::fromValue(static_cast<ListItemSlots *>(this)));
    }

    return pTreeWidgetItem;
}

template <class TObject, class TState, class TObjectTypeEnum>
void ListItemBase<TObject, TState, TObjectTypeEnum>::SetTreeWidgetItem(QTreeWidgetItem *pTreeWidgetItem)
{
    this->pTreeWidgetItem = pTreeWidgetItem;
    pTreeWidgetItem->setData(0, ListItemRole, QVariant::fromValue(static_cast<ListItemSlots *>(this)));
}

template <class TObject, class TState, class TObjectTypeEnum>
ListItemBase<TObject, TState, TObjectTypeEnum> * ListItemBase<TObject, TState, TObjectTypeEnum>::FromTreeWidgetItem(QTreeWidgetItem *pItem)
{
    QVariant v = pItem->data(0, ListItemRole);
    ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem =
            static_cast<ListItemBase<TObject, TState, TObjectTypeEnum> *>(v.value<ListItemSlots *>());

    return pListItem;
}

template <class TObject, class TState, class TObjectTypeEnum>
void ListItem<TObject, TState, TObjectTypeEnum>::UpdateTreeWidgetItem(ListItemTreeWidget *pParentTreeWidget)
{
    QTreeWidgetItem *pItem = this->GetTreeWidgetItem(pParentTreeWidget);

    pItem->setText(0, GetDisplayString());
    pItem->setData(0, ListItemRole, QVariant::fromValue(this));
}

template <class TObject, class TState, class TObjectTypeEnum>
EditorDialog<TObject, TState, TObjectTypeEnum> * ListItem<TObject, TState, TObjectTypeEnum>::GetEditorDialog()
{
    return new EditorDialog<TObject, TState, TObjectTypeEnum>(this->stateBeforeObject, GetObject());
}

template <class TObject, class TState, class TObjectTypeEnum>
QTreeWidgetItem * ListItem<TObject, TState, TObjectTypeEnum>::GetTreeWidgetItemImpl(ListItemTreeWidget */*pParentTreeWidget*/)
{
    QTreeWidgetItem *pItem = new QTreeWidgetItem();

    pItem->setText(0, GetDisplayString());
    pItem->setData(0, HeightPaddingRole, QVariant(10));
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if (!this->GetCanHaveChildren())
    {
        pItem->setFlags(pItem->flags() | Qt::ItemNeverHasChildren);
    }

    return pItem;
}

template <class TObject, class TState, class TObjectTypeEnum>
EditorDialog<TObject, TState, TObjectTypeEnum> * ListItemBase<TObject, TState, TObjectTypeEnum>::GetEditorDialog()
{
    return new EditorDialog<TObject, TState, TObjectTypeEnum>(stateBeforeObject, NULL);
}

template <class TObject, class TState, class TObjectTypeEnum>
TopSeparatorListItem<TObject, TState, TObjectTypeEnum>::TopSeparatorListItem(int indentLevel, QList<TObject *> *pContainingObjectList, int positionInList, bool shouldInsert)
    : ListItemBase<TObject, TState, TObjectTypeEnum>(indentLevel)
{
    this->SetContainingObjectList(pContainingObjectList, positionInList, shouldInsert);
}

template <class TObject, class TState, class TObjectTypeEnum>
QTreeWidgetItem * TopSeparatorListItem<TObject, TState, TObjectTypeEnum>::GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget)
{
    return new ListItemSeparator<TObject>(pParentTreeWidget, ListItemSeparatorPosition::Top);
}

template <class TObject, class TState, class TObjectTypeEnum>
MiddleSeparatorListItem<TObject, TState, TObjectTypeEnum>::MiddleSeparatorListItem(int indentLevel, QList<TObject *> *pContainingObjectList, int positionInList, bool shouldInsert)
    : ListItemBase<TObject, TState, TObjectTypeEnum>(indentLevel)
{
    this->SetContainingObjectList(pContainingObjectList, positionInList, shouldInsert);
}

template <class TObject, class TState, class TObjectTypeEnum>
QTreeWidgetItem * MiddleSeparatorListItem<TObject, TState, TObjectTypeEnum>::GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget)
{
    return new ListItemSeparator<TObject>(pParentTreeWidget, ListItemSeparatorPosition::Middle);
}

template <class TObject, class TState, class TObjectTypeEnum>
BottomSeparatorListItem<TObject, TState, TObjectTypeEnum>::BottomSeparatorListItem(int indentLevel, QList<TObject *> *pContainingObjectList, int positionInList, bool shouldInsert)
    : ListItemBase<TObject, TState, TObjectTypeEnum>(indentLevel)
{
    this->SetContainingObjectList(pContainingObjectList, positionInList, shouldInsert);
}

template <class TObject, class TState, class TObjectTypeEnum>
QTreeWidgetItem * BottomSeparatorListItem<TObject, TState, TObjectTypeEnum>::GetTreeWidgetItemImpl(ListItemTreeWidget *pParentTreeWidget)
{
    return new ListItemSeparator<TObject>(pParentTreeWidget, ListItemSeparatorPosition::Bottom);
}

template <class TObject, class TState, class TObjectTypeEnum>
ObjectListWidget<TObject, TState, TObjectTypeEnum>::ObjectListWidget()
    : ObjectListWidgetSlots()
{
    topLevelItemCount = 0;
    currentInsertionPosition = 0;
    lastIndentLevel = 0;
    pLastTreeWidgetItem = NULL;
    currentState = TState();

    pSelectedListItem = NULL;
    selectedListItemIndex = -1;
    pEnteredSeparator = NULL;
    editingDialogIsOpen = false;
    onMouseLeaveQueued = false;

    QGridLayout *pMainLayout = new QGridLayout();

    pListItemHolder = new ListItemTreeWidget();
    pListItemHolder->setHeaderHidden(true);
    pListItemHolder->setWordWrap(true);
    pListItemHolder->setExpandsOnDoubleClick(false);
    pListItemHolder->setMouseTracking(true);
    pListItemHolder->setItemDelegate(new HtmlDelegate());
    pListItemHolder->setStyleSheet("QTreeView::item:hover { background-color:#FFFF00; }");

    QObject::connect(pListItemHolder, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(EditorHolderCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    QObject::connect(pListItemHolder, SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(EditorHolderItemEntered(QTreeWidgetItem*,int)));
    QObject::connect(pListItemHolder, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(EditorHolderItemDoubleClicked(QTreeWidgetItem*,int)));

    pMainLayout->addWidget(pListItemHolder);

    setLayout(pMainLayout);
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::BeginListItemChanges()
{
    newTreeWidgetItems.clear();
    topLevelItemCount = 0;
    newListItems.clear();

    currentInsertionPosition = 0;
    lastIndentLevel = 0;
    pLastTreeWidgetItem = NULL;
    treeWidgetStack.clear();
    currentLevelItemCountStack.clear();
    currentState = TState();
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::AddListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem)
{
    QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> listItems;
    listItems.push_back(pListItem);
    AddListItems(listItems);
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::AddListItems(const QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> &listItemList)
{
    InsertListItems(listItems.length(), listItemList, false /* replaceInitialItem */);
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::InsertListItems(int insertionPosition, const QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> &listItemList, bool replaceInitialItem)
{
    int initialPosition = insertionPosition;

    if (insertionPosition < currentInsertionPosition)
    {
        BeginListItemChanges();
    }

    while (currentInsertionPosition < insertionPosition)
    {
        ListItemBase<TObject, TState, TObjectTypeEnum> *pCurrentListItem = listItems[currentInsertionPosition];
        TObject *pObject = pCurrentListItem->GetObject();

        if (pObject != NULL)
        {
            currentState = pObject->GetStateDuringObject().Clone();
        }

        pCurrentListItem->SetStateBeforeObject(currentState);

        if (pCurrentListItem->GetIndentLevel() > lastIndentLevel)
        {
            if (pCurrentListItem->GetIndentLevel() > lastIndentLevel + 1)
            {
                throw new MLIException("Indentation levels should only increment by 1 at a time.");
            }

            if (pLastTreeWidgetItem != NULL)
            {
                treeWidgetStack.push(pLastTreeWidgetItem);
            }

            lastIndentLevel++;
            currentLevelItemCountStack.push(0);
        }
        else
        {
            while (pCurrentListItem->GetIndentLevel() < lastIndentLevel)
            {
                pLastTreeWidgetItem = treeWidgetStack.pop();
                currentLevelItemCountStack.pop();
                lastIndentLevel--;
            }
        }

        pLastTreeWidgetItem = treeWidgetItems[currentInsertionPosition];

        if (pCurrentListItem->GetIndentLevel() == 0)
        {
            topLevelItemCount++;
        }
        else
        {
            currentLevelItemCountStack.top()++;
        }

        currentInsertionPosition++;
    }

    int originalIndentLevel = lastIndentLevel;

    for (ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem : listItemList)
    {
        TObject *pObject = pListItem->GetObject();

        if (pObject != NULL)
        {
            currentState = pObject->GetStateDuringObject().Clone();
        }

        pListItem->SetStateBeforeObject(currentState);

        if (pListItem->GetIndentLevel() > lastIndentLevel)
        {
            if (pListItem->GetIndentLevel() > lastIndentLevel + 1)
            {
                throw new MLIException("Indentation levels should only increment by 1 at a time.");
            }

            if (pLastTreeWidgetItem != NULL)
            {
                treeWidgetStack.push(pLastTreeWidgetItem);
            }

            lastIndentLevel++;
            currentLevelItemCountStack.push(0);
        }
        else
        {
            while (pListItem->GetIndentLevel() < lastIndentLevel)
            {
                pLastTreeWidgetItem = treeWidgetStack.pop();
                currentLevelItemCountStack.pop();
                lastIndentLevel--;
            }
        }

        QTreeWidgetItem *pTreeWidgetItem = NULL;
        bool shouldInsertItem = false;

        if (!replaceInitialItem || currentInsertionPosition > initialPosition || currentInsertionPosition >= treeWidgetItems.length())
        {
            pTreeWidgetItem = pListItem->GetTreeWidgetItem(pListItemHolder);
            shouldInsertItem = true;
        }
        else
        {
            // If a tree widget item exists for the initial position, then we
            // don't need to create a new tree widget item for that position -
            // we'll just reuse the one we already have, since we'll just be
            // replacing its text and data object.
            pTreeWidgetItem = treeWidgetItems[currentInsertionPosition];
            pListItem->SetTreeWidgetItem(pTreeWidgetItem);
            pListItem->UpdateTreeWidgetItem(pListItemHolder);
        }

        if (shouldInsertItem)
        {
            InsertItem(pTreeWidgetItem);
        }

        newTreeWidgetItems.append(pTreeWidgetItem);
        newListItems.append(pListItem);
        listItems.insert(currentInsertionPosition, pListItem);

        pLastTreeWidgetItem = pTreeWidgetItem;

        currentInsertionPosition++;
    }

    // If we inserted a new item, we want to increment the position in list by 1
    // of all of the list items that come after it.
    if (!replaceInitialItem)
    {
        int currentPosition = currentInsertionPosition;

        while (currentPosition < listItems.length() && listItems[currentPosition]->GetIndentLevel() >= originalIndentLevel)
        {
            if (listItems[currentPosition]->GetIndentLevel() == originalIndentLevel)
            {
                listItems[currentPosition]->AdjustPositionInList(1);
            }

            currentPosition++;
        }
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::NotifyListItemChangesComplete()
{
    if (newListItems.length() > 0 &&
            (pListItemHolder->currentItem() == NULL ||
            !listItems.contains(pSelectedListItem)))
    {
        // If we don't have any selected items at the moment,
        // or if the selected item has been removed from the list,
        // then we'll select the first selectable item, to ensure
        // we always have one selected.
        for (ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem : newListItems)
        {
            QTreeWidgetItem *pItem = pListItem->GetTreeWidgetItem(pListItemHolder);

            if (pItem->flags() & Qt::ItemIsSelectable)
            {
                pListItemHolder->setCurrentItem(pItem);
                pSelectedListItem = pListItem;
                selectedListItemIndex = listItems.indexOf(pSelectedListItem);
                break;
            }
        }
    }

    for (QTreeWidgetItem *pItem : newTreeWidgetItems)
    {
        pItem->setExpanded(true);
    }

    newTreeWidgetItems.clear();
    newListItems.clear();

    lastIndentLevel = 0;
    pLastTreeWidgetItem = NULL;
    treeWidgetStack.clear();
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::ReplaceSelectedListItem(const QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> &listItemList)
{
    if (pSelectedListItem == NULL)
    {
        return;
    }

    BeginListItemChanges();

    if (FindAndRemoveListItem(pSelectedListItem, false /* removeFromListWidgetToo */) == false)
    {
        return;
    }

    InsertListItems(currentInsertionPosition, listItemList, true /* replaceInitialItem */);
    NotifyListItemChangesComplete();
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::UpdateSelectedListItem()
{
    pSelectedListItem->UpdateTreeWidgetItem(pListItemHolder);

    if (pSelectedListItem->GetObject() == NULL)
    {
        return;
    }

    int initialIndentLevel = pSelectedListItem->GetIndentLevel();
    int selectedPosition = listItems.indexOf(pSelectedListItem);
    int currentPosition = selectedPosition + 1;

    BeginListItemChanges();

    while (currentPosition < listItems.size() && initialIndentLevel < listItems[currentPosition]->GetIndentLevel())
    {
        FindAndRemoveListItem(listItems[currentPosition], true /* removeFromListWidgetToo */);
    }

    QList<ListItemBase<TObject, TState, TObjectTypeEnum> *> listItems = pSelectedListItem->GetObject()->GetListItems(initialIndentLevel);

    // listItems will begin with the list item for the selected list item, which is already included in the list,
    // so we'll remove it before inserting.
    // Calling GetListItems() will have overwritten its list item, so we'll undo that, too.
    TObject *pSelectedObject = pSelectedListItem->GetObject();

    if (pSelectedObject != NULL)
    {
        pSelectedObject->SetListItemBase(pSelectedListItem);
    }

    ListItemBase<TObject, TState, TObjectTypeEnum> *pFirstListItem = listItems[0];
    listItems.removeAt(0);
    delete pFirstListItem;

    if (listItems.size() > 0)
    {
        BeginListItemChanges();
        InsertListItems(selectedPosition + 1, listItems, false /* replaceInitialItem */);
    }

    NotifyListItemChangesComplete();
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::DeleteSelectedListItem(bool shouldRemovePreviousSeparator)
{
    int rowIndex = listItems.indexOf(pSelectedListItem);
    int originalIndentLevel = pSelectedListItem->GetIndentLevel();

    BeginListItemChanges();

    if (shouldRemovePreviousSeparator)
    {
        FindAndRemoveListItem(listItems[rowIndex - 1], true /* removeFromListWidgetToo */);
    }

    FindAndRemoveListItem(pSelectedListItem, true /* removeFromListWidgetToo */);

    if (!shouldRemovePreviousSeparator)
    {
        FindAndRemoveListItem(listItems[rowIndex], true /* removeFromListWidgetToo */);
    }

    NotifyListItemChangesComplete();

    int currentPosition = currentInsertionPosition;

    while (currentPosition < listItems.length() && listItems[currentPosition]->GetIndentLevel() >= originalIndentLevel)
    {
        if (listItems[currentPosition]->GetIndentLevel() == originalIndentLevel)
        {
            listItems[currentPosition]->AdjustPositionInList(-1);
        }

        currentPosition++;
    }

    if (listItems.length() > 1)
    {
        pSelectedListItem = listItems[rowIndex == listItems.length() ? rowIndex - 2 : rowIndex];
        selectedListItemIndex = listItems.indexOf(pSelectedListItem);
        pListItemHolder->setCurrentItem(pSelectedListItem->GetTreeWidgetItem(pListItemHolder));
    }
    else
    {
        // If we have only a single item left, it's a separator, so we should just act
        // like we have no items remaining.
        pSelectedListItem = NULL;
        selectedListItemIndex = -1;
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::UpdateListItemStatesBeforeObject()
{
    TState currentState;

    for (ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem : listItems)
    {
        TObject *pObject = pListItem->GetObject();

        if (pObject != NULL)
        {
            currentState = pObject->GetStateDuringObject().Clone();
        }

        pListItem->SetStateBeforeObject(currentState);
        pListItem->UpdateTreeWidgetItem(pListItemHolder);
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
TObject * ObjectListWidget<TObject, TState, TObjectTypeEnum>::GetSelectedObject()
{
    if (pSelectedListItem != NULL)
    {
        return pSelectedListItem->GetObject();
    }
    else
    {
        return NULL;
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::EditorHolderCurrentItemChangedImpl(QTreeWidgetItem *pCurrentItem, QTreeWidgetItem * /*pPreviousItem*/)
{
    if (pCurrentItem != NULL)
    {
        ListItemBase<TObject, TState, TObjectTypeEnum> *pListItemSelected = ListItemBase<TObject, TState, TObjectTypeEnum>::FromTreeWidgetItem(pCurrentItem);

        if (pListItemSelected != pSelectedListItem)
        {
            pSelectedListItem = pListItemSelected;
            selectedListItemIndex = listItems.indexOf(pSelectedListItem);

            emit this->ObjectSelected(pSelectedListItem->GetObject());
        }
    }
    else
    {
        pSelectedListItem = NULL;
        selectedListItemIndex = -1;

        emit this->ObjectSelected(NULL);
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::EditorHolderItemEnteredImpl(QTreeWidgetItem *pItem, int /*column*/)
{
    if (pEnteredSeparator != pItem)
    {
        if (pEnteredSeparator != NULL)
        {
            if (editingDialogIsOpen)
            {
                onMouseLeaveQueued = true;
            }
            else
            {
                pEnteredSeparator->OnMouseLeave();
            }

            pEnteredSeparator = NULL;
        }

        ListItemSeparator<TObject> *pSeparator = dynamic_cast<ListItemSeparator<TObject> *>(pItem);

        if (pSeparator != NULL)
        {
            pSeparator->OnMouseOver();
            pEnteredSeparator = pSeparator;
        }
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::EditorHolderItemDoubleClickedImpl(QTreeWidgetItem *pItem, int /*column*/)
{
    ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem = ListItemBase<TObject, TState, TObjectTypeEnum>::FromTreeWidgetItem(pItem);

    if (pListItem == NULL)
    {
        // Nothing to do here - we've double-clicked on an item that doesn't have an editor associated.
        return;
    }

    EditListItem(pListItem);
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::leaveEvent(QEvent *pEvent)
{
    if (pEnteredSeparator != NULL)
    {
        if (editingDialogIsOpen)
        {
            onMouseLeaveQueued = true;
        }
        else
        {
            pEnteredSeparator->OnMouseLeave();
        }
    }

    pEvent->accept();
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::keyPressEvent(QKeyEvent *pEvent)
{
    if (pEvent->key() == Qt::Key_Return || pEvent->key() == Qt::Key_Enter)
    {
        EditListItem(listItems[selectedListItemIndex]);
        pEvent->accept();
    }
    else if (pEvent->key() == Qt::Key_Delete)
    {
        DeleteListItem(listItems[selectedListItemIndex]);
        pEvent->accept();
    }
    else
    {
        QWidget::keyPressEvent(pEvent);
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::InsertItem(QTreeWidgetItem *pItem)
{
    ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem = ListItemBase<TObject, TState, TObjectTypeEnum>::FromTreeWidgetItem(pItem);

    if (pListItem->GetIndentLevel() == 0)
    {
        pListItemHolder->insertTopLevelItem(topLevelItemCount, pItem);
        topLevelItemCount++;
    }
    else
    {
        treeWidgetStack.top()->insertChild(currentLevelItemCountStack.top(), pItem);
        currentLevelItemCountStack.top()++;
    }

    treeWidgetItems.insert(currentInsertionPosition, pItem);
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::EditListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem)
{
    if (pListItem->GetIsPlaceholder())
    {
        // We should not be editing placeholder items.
        return;
    }

    TObject *pNewObject = NULL;
    EditorDialog<TObject, TState, TObjectTypeEnum> *pEditorDialog = pListItem->GetEditorDialog();
    int rowIndex = listItems.indexOf(pListItem);

    editingDialogIsOpen = true;

    if (pEditorDialog->exec() == QDialog::Accepted)
    {
        pNewObject = pEditorDialog->GetNewObject();
    }
    else
    {
        pEditorDialog->ClearNewObject();
    }

    editingDialogIsOpen = false;
    bool onMouseLeaveQueuedLocal = onMouseLeaveQueued;
    onMouseLeaveQueued = false;

    pEditorDialog->deleteLater();

    if (pNewObject != NULL)
    {
        emit this->ObjectEditingComplete(
                    pNewObject,
                    pListItem->GetContainingObjectList(),
                    pListItem->GetPositionInList(),
                    pListItem->GetShouldInsert(),
                    pListItem->GetIndentLevel(),
                    rowIndex);
    }
    else
    {
        emit this->ObjectEditingComplete(NULL, NULL, -1, false, -1, -1);

        if (onMouseLeaveQueuedLocal && pEnteredSeparator != NULL)
        {
            pEnteredSeparator->OnMouseLeave();
        }
    }
}

template <class TObject, class TState, class TObjectTypeEnum>
void ObjectListWidget<TObject, TState, TObjectTypeEnum>::DeleteListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem)
{
    if (pListItem->GetIsPlaceholder())
    {
        // We should not be deleting placeholder items.
        return;
    }

    TObject *pObject = pListItem->GetObject();

    if (pObject == NULL)
    {
        // If this doesn't have an associated Object, then there's nothing to delete.
        return;
    }

    emit this->ObjectDeleteRequested(
                pObject,
                pListItem->GetContainingObjectList(),
                pListItem->GetPositionInList());
}

template <class TObject, class TState, class TObjectTypeEnum>
bool ObjectListWidget<TObject, TState, TObjectTypeEnum>::FindAndRemoveListItem(ListItemBase<TObject, TState, TObjectTypeEnum> *pListItem, bool removeFromListWidgetToo)
{
    while (currentInsertionPosition < treeWidgetItems.length())
    {
        ListItemBase<TObject, TState, TObjectTypeEnum> *pCurrentListItem = listItems[currentInsertionPosition];

        if (pCurrentListItem->GetIndentLevel() > lastIndentLevel)
        {
            if (pCurrentListItem->GetIndentLevel() > lastIndentLevel + 1)
            {
                throw new MLIException("Indentation levels should only increment by 1 at a time.");
            }

            if (currentInsertionPosition > 0)
            {
                treeWidgetStack.push(treeWidgetItems[currentInsertionPosition - 1]);
            }

            lastIndentLevel++;
            currentLevelItemCountStack.push(0);
        }
        else
        {
            while (pCurrentListItem->GetIndentLevel() < lastIndentLevel)
            {
                treeWidgetStack.pop();
                currentLevelItemCountStack.pop();
                lastIndentLevel--;
            }
        }

        if (pCurrentListItem == pListItem)
        {
            break;
        }

        TObject *pObject = pCurrentListItem->GetObject();

        if (pObject != NULL)
        {
            currentState = pObject->GetStateDuringObject().Clone();
        }

        currentInsertionPosition++;

        pCurrentListItem->SetStateBeforeObject(currentState);

        if (pCurrentListItem->GetIndentLevel() == 0)
        {
            topLevelItemCount++;
        }
        else
        {
            currentLevelItemCountStack.top()++;
        }
    }

    if (currentInsertionPosition == treeWidgetItems.length())
    {
        BeginListItemChanges();
        return false;
    }

    // Now we'll remove the previous item and replace it with the replacement items.
    int currentPosition = currentInsertionPosition + 1;

    while (currentPosition < treeWidgetItems.length() &&
           listItems[currentPosition]->GetIndentLevel() > pListItem->GetIndentLevel())
    {
        ListItemBase<TObject, TState, TObjectTypeEnum> *pListItemToRemove = listItems[currentPosition];
        QTreeWidgetItem *pTreeWidgetItemToRemove = treeWidgetItems[currentPosition];
        QTreeWidgetItem *pParentItem = pTreeWidgetItemToRemove->parent();

        if (pParentItem != NULL)
        {
            pParentItem->removeChild(pTreeWidgetItemToRemove);
        }

        treeWidgetItems.removeAt(currentPosition);
        //delete pTreeWidgetItemToRemove;

        listItems.removeAt(currentPosition);
        pListItemToRemove->deleteLater();
    }

    if (removeFromListWidgetToo)
    {
        if (pListItem->GetIndentLevel() == 0)
        {
            pListItemHolder->takeTopLevelItem(topLevelItemCount);
        }
        else
        {
            treeWidgetStack.top()->takeChild(currentLevelItemCountStack.top());
        }

        treeWidgetItems.removeAt(currentInsertionPosition);
    }

    listItems.removeAt(currentInsertionPosition);
    pListItem->deleteLater();

    pLastTreeWidgetItem = NULL;

    if (currentInsertionPosition > 0)
    {
        pLastTreeWidgetItem = treeWidgetItems[currentInsertionPosition - 1];
    }

    return true;
}

#define DEFINE_OBJECT_LIST_WIDGET_TYPES(OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE) \
    template class ListItemObject<STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class EditorDialogContents<OBJECT_TYPE, STATE_TYPE>; \
    template class EditorDialog<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class ListItemSeparator<OBJECT_TYPE>; \
    template class ListItemBase<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class ListItem<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class TopSeparatorListItem<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class MiddleSeparatorListItem<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class BottomSeparatorListItem<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class TextDisplay<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \
    template class ObjectListWidget<OBJECT_TYPE, STATE_TYPE, OBJECT_TYPE_ENUM_TYPE>; \

DEFINE_OBJECT_LIST_WIDGET_TYPES(Conversation::Action, Conversation::State, Conversation::ActionType)
DEFINE_OBJECT_LIST_WIDGET_TYPES(FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType)
