#ifndef LISTMANIPULATOR_H
#define LISTMANIPULATOR_H

#include "CaseCreator/CaseContent/CaseContent.h"
#include "CaseCreator/UIComponents/MainWindow.h"

#include "NewObjectDialog.h"
#include "ObjectManipulator.h"

#include <QWidget>

#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollArea>

#include <QSplitter>
#include <QVBoxLayout>

#include <QFileDialog>

class ListManipulatorBase : public QWidget
{
    Q_OBJECT

public:
    ListManipulatorBase(QWidget *parent = 0)
        : QWidget(parent)
    {
    }

    virtual void RefreshList() = 0;

    virtual bool Undo() = 0;
    virtual bool Redo() = 0;

    virtual bool GetIsActive() = 0;
    virtual void SetIsActive(bool isActive) = 0;

public slots:
    void ListCurrentRowChangedSlot(int newRowIndex) { return ListCurrentRowChanged(newRowIndex); }
    void ListItemDoubleClickedSlot(QListWidgetItem *pClickedItem) { return ListItemDoubleClicked(pClickedItem); }

protected:
    virtual void ListCurrentRowChanged(int newRowIndex) = 0;
    virtual void ListItemDoubleClicked(QListWidgetItem *pClickedItem) = 0;
};

// TODO: Make a ListManipulator for sub-lists that can have multiple list objects.
template <class T>
class ListManipulator : public ListManipulatorBase
{
public:
    explicit ListManipulator(QWidget *parent = 0)
        : ListManipulatorBase(parent)
        , pParentObject(NULL)
        , pObjectList(NULL)
        , pNewObjectDialog(NULL)
        , isActive(false)
    {
        pObjectManipulator = ObjectManipulator<T>::Create();

        QHBoxLayout *pContentLayout = new QHBoxLayout();
        pContentLayout->addWidget(pObjectManipulator);

        setLayout(pContentLayout);
    }

    ~ListManipulator()
    {
        if (pNewObjectDialog != NULL)
        {
            pNewObjectDialog->deleteLater();
            pNewObjectDialog = NULL;
        }
    }

    void SetParentObject(void *pParentObject)
    {
        this->pParentObject = pParentObject;

        RefreshList();
    }

    void RefreshList()
    {
        PopulateIds();
    }

    void Reset()
    {
        pObjectManipulator->Reset();
    }

    bool Undo()
    {
        return pObjectManipulator->Undo();
    }

    bool Redo()
    {
        return pObjectManipulator->Redo();
    }

    bool GetIsActive()
    {
        return isActive;
    }

    void SetIsActive(bool isActive)
    {
        if (isActive && pObjectList == NULL)
        {
            pObjectList = new QListWidget();
            pObjectList->setSelectionMode(QAbstractItemView::SingleSelection);

            QObject::connect(pObjectList, SIGNAL(currentRowChanged(int)), this, SLOT(ListCurrentRowChangedSlot(int)));
            QObject::connect(pObjectList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(ListItemDoubleClickedSlot(QListWidgetItem*)));

            if (T::GetAllowsNewObjects())
            {
                pAddNewObjectItem = new QListWidgetItem(QString("Double-click to add new ") + T::GetObjectAdditionString());
                QFont newObjectFont;
                newObjectFont.setItalic(true);
                pAddNewObjectItem->setFont(newObjectFont);
                pAddNewObjectItem->setTextColor(QColor::fromRgb(128, 128, 128));
                pObjectList->addItem(pAddNewObjectItem);
            }

            PopulateIds();

            if (T::GetIsMainList())
            {
                MainWindow::GetInstance()->SetMainListWidget(T::GetListTitle(), pObjectList);
            }
            else
            {
                MainWindow::GetInstance()->ClearSubListWidgets();
                MainWindow::GetInstance()->AddSubListWidget(T::GetListTitle(), pObjectList);
            }
        }
        else if (!isActive && pObjectList != NULL)
        {
            pObjectList = NULL;

            if (!T::GetIsMainList())
            {
                MainWindow::GetInstance()->ClearSubListWidgets();
            }
        }

        if (isActive != this->isActive)
        {
            this->isActive = isActive;
            pObjectManipulator->SetIsActive(isActive);
            EnsureRowSelected();
        }
    }

protected:
    void ListCurrentRowChanged(int newRowIndex)
    {
        if (newRowIndex < idList.count() && newRowIndex >= 0)
        {
            pObjectManipulator->Reset();
            pObjectManipulator->Init(CaseContent::GetInstance()->GetById<T>(idList[newRowIndex], pParentObject));
        }
    }

    void ListItemDoubleClicked(QListWidgetItem *pClickedItem)
    {
        if (pClickedItem == pAddNewObjectItem)
        {
            if (pNewObjectDialog == NULL)
            {
                pNewObjectDialog = NewObjectDialog<T>::Create();
            }

            if (pNewObjectDialog->exec() == QDialog::Accepted)
            {
                T *pNewObject = pNewObjectDialog->GetNewObject();
                CaseContent::GetInstance()->Add<T>(pNewObject->GetId(), pNewObject, pParentObject);
                PopulateIds();
                pObjectList->setCurrentRow(idList.indexOf(QRegExp(pNewObject->GetId())));
            }
        }
    }

private:
    // This method compares two strings in a case-insensitive way,
    // and such that numbers are treated as numbers when encountered -
    // i.e., {String1, String2, String3, String10, String 11, String100}
    // is considered the proper order for those strings.
    static bool LessThanWithNumbers(QString s1, QString s2)
    {
        int string1Index = 0;
        int string2Index = 0;

        while (string1Index < s1.length() && string2Index < s2.length())
        {
            QChar c1 = s1.at(string1Index);
            QChar c2 = s2.at(string2Index);

            // If we're at the start of a number in both strings
            // then we should get the number from both strings and compare.
            // Only if it's the same number will we keep going;
            // otherwise we'll return less than or greater than.
            if (c1.isDigit() && c2.isDigit())
            {
                int string1NumberValue = 0;
                int string2NumberValue = 0;

                while (c1.isDigit() && string1Index < s1.length())
                {
                    string1NumberValue = string1NumberValue * 10 + c1.digitValue();
                    string1Index++;

                    if (string1Index < s1.length())
                    {
                        c1 = s1.at(string1Index);
                    }
                }

                while (c2.isDigit() && string2Index < s2.length())
                {
                    string2NumberValue = string2NumberValue * 10 + c2.digitValue();
                    string2Index++;

                    if (string2Index < s2.length())
                    {
                        c2 = s2.at(string2Index);
                    }
                }

                if (string1NumberValue < string2NumberValue)
                {
                    return true;
                }
                else if (string1NumberValue > string2NumberValue)
                {
                    return false;
                }
            }
            // Otherwise, if both characters are letters,
            // we'll make both of them lowercase when checking
            // in order to check in a case insensitive manner.
            else if (c1.isLetter() && c2.isLetter())
            {
                if (c1.toLower() < c2.toLower())
                {
                    return true;
                }
                else if (c1.toLower() > c2.toLower())
                {
                    return false;
                }

                string1Index++;
                string2Index++;
            }
            // If we neither have both digits nor both letters,
            // then the best we can do is just compare the characters'
            // numeric values.
            else
            {
                if (c1 < c2)
                {
                    return true;
                }
                else if (c1 > c2)
                {
                    return false;
                }

                string1Index++;
                string2Index++;
            }
        }

        // If we've reached the end here, that means that
        // we've found no difference between the strings
        // thus far.  Now we need to check where we are in each string.
        // If one string is a subset of another string,
        // then we'll consider the subset to be less than the full string.
        if (string1Index < string2Index)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void PopulateIds()
    {
        if (pObjectList == NULL)
        {
            return;
        }

        idList = CaseContent::GetInstance()->GetIds<T>(pParentObject);
        displayNameList = CaseContent::GetInstance()->GetDisplayNames<T>(pParentObject);

        for (int i = idList.length() - 1; i >= 0; i--)
        {
            T *pItem = CaseContent::GetInstance()->GetById<T>(idList[i]);

            if (!pObjectManipulator->ShouldIncludeItem(pItem))
            {
                idList.removeAt(i);
                displayNameList.removeAt(i);
            }
        }

        if (idList.length() != displayNameList.length())
        {
            throw new MLIException("ID list and display name list must always be the same length.");
        }

        QMap<QString, QString> idToDisplayNameMap;

        for (int i = 0; i < idList.length(); i++)
        {
            idToDisplayNameMap[idList[i]] = displayNameList[i];
        }

        qSort(idList.begin(), idList.end(), ListManipulator<T>::LessThanWithNumbers);

        for (int i = pObjectList->count() - (T::GetAllowsNewObjects() ? 2 : 1); i >= 0; i--)
        {
            pObjectList->takeItem(i);
        }

        QStringList displayNameListSorted;

        for (int i = 0; i < idList.length(); i++)
        {
            displayNameListSorted.push_back(idToDisplayNameMap[idList[i]]);
        }

        displayNameList = displayNameListSorted;
        pObjectList->insertItems(0, displayNameList);
        EnsureRowSelected();
    }

    void *pParentObject;

    QListWidget *pObjectList;
    QListWidgetItem *pAddNewObjectItem;
    ObjectManipulator<T> *pObjectManipulator;
    NewObjectDialog<T> *pNewObjectDialog;

    QStringList idList;
    QStringList displayNameList;

    bool isActive;

private:
    void EnsureRowSelected()
    {
        // If we're active and have populated our list, we'll select the first row.
        if (isActive && pObjectList != NULL && pObjectList->count() > (T::GetAllowsNewObjects() ? 1 : 0))
        {
            pObjectList->setCurrentRow(0);
        }
    }

    bool isMainList;
};

#endif // LISTMANIPULATOR_H
