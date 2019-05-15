#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

#include <QListWidget>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>

#include "CaseCreator/Utilities/Interfaces.h"

class ListManipulatorBase;

class ListWidgetHolder : public QWidget
{
    Q_OBJECT

public:
    explicit ListWidgetHolder(const QString &title, QListWidget *pListWidget)
    {
        QGridLayout *pLayout = new QGridLayout();
        pLayout->setRowStretch(0, 0);
        pLayout->setRowStretch(1, 1);

        pLayout->addWidget(new QLabel(title), 0, 0);
        pLayout->addWidget(pListWidget, 1, 0);

        pLayout->setContentsMargins(0, 0, 0, 0);
        setLayout(pLayout);
    }
};

class MainWindow : public QMainWindow, public ICaseLoadedCallback
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void CaseLoaded();

    static MainWindow * GetInstance()
    {
        return pSingleton;
    }

    void SetMainListWidget(const QString &title, QListWidget *pListWidget);
    void AddSubListWidget(const QString &title, QListWidget *pListWidget);
    void ClearSubListWidgets();

public slots:
    void OnTabWidgetCurrentChanged(int newTabIndex);

protected:
    bool eventFilter(QObject *pSender, QEvent *pEvent);

private slots:
    void CreateNewProject();
    void OpenProject();
    void SaveProject();
    void CreateProjectFromCaseFile();
    void CompileProjectToCaseFile();

private:
    static MainWindow *pSingleton;

    void RefreshLists();

    QMenu *pFileMenu;

    QHBoxLayout *pMainListWidgetLayout;
    ListWidgetHolder *pMainListWidgetHolder;
    QVBoxLayout *pSubListWidgetsLayout;
    QList<ListWidgetHolder *> subListWidgetHolders;

    QTabWidget *pTabWidget;
    QList<ListManipulatorBase *> listManipulators;

    int currentlyActiveTabIndex;
};

#endif // MAINWINDOW_H
