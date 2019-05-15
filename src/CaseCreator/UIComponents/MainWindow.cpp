#include "MainWindow.h"

#include "TemplateHelpers/ListManipulator.h"
#include "CaseCreator/CaseContent/Staging/CaseContentLoader.h"
#include "CaseCreator/CaseContent/Location.h"
#include "CaseCreator/CaseContent/Character.h"
#include "CaseCreator/CaseContent/Encounter.h"
#include "CaseCreator/UIComponents/NewProjectDialog.h"
#include "MLIException.h"

#include <QApplication>
#include <QMenuBar>
#include <QPushButton>
#include <QTabWidget>
#include <QStandardPaths>
#include <QKeyEvent>

#include <QFontDatabase>

MainWindow *MainWindow::pSingleton = NULL;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , pMainListWidgetLayout(NULL)
    , pMainListWidgetHolder(NULL)
    , pSubListWidgetsLayout(NULL)
{
    if (pSingleton == NULL)
    {
        pSingleton = this;
    }
    else
    {
        throw new MLIException("There should only ever be one MainWindow in the application.");
    }

    QFontDatabase::addApplicationFont("C:/Users/Luke/Documents/My Little Investigations/Case Creator Projects/Case 1/Fonts/CelestiaMediumRedux1.5.ttf");
    QFontDatabase::addApplicationFont("C:/Users/Luke/Documents/My Little Investigations/Case Creator Projects/Case 1/Fonts/FayesMousewriting_quotemapped.ttf");

    currentlyActiveTabIndex = -1;

    QWidget *pListObjectWidget = new QWidget();
    QGridLayout *pListObjectLayout = new QGridLayout();

    pListObjectLayout->setRowStretch(0, 1);
    pListObjectLayout->setRowStretch(1, 0);

    pMainListWidgetLayout = new QHBoxLayout();
    pSubListWidgetsLayout = new QVBoxLayout();

    pListObjectLayout->addLayout(pMainListWidgetLayout, 0, 0);
    pListObjectLayout->addLayout(pSubListWidgetsLayout, 1, 0);

    pListObjectWidget->setLayout(pListObjectLayout);

    pTabWidget = new QTabWidget();
    pTabWidget->setUsesScrollButtons(true);

    ListManipulator<Location> *pLocationManipulator = new ListManipulator<Location>();
    ListManipulator<Character> *pCharacterManipulator = new ListManipulator<Character>();
    ListManipulator<Encounter> *pEncounterManipulator = new ListManipulator<Encounter>();

    listManipulators.push_back(pLocationManipulator);
    listManipulators.push_back(pCharacterManipulator);
    listManipulators.push_back(pEncounterManipulator);

    QObject::connect(pTabWidget, SIGNAL(currentChanged(int)), this, SLOT(OnTabWidgetCurrentChanged(int)));

    pTabWidget->addTab(pLocationManipulator, tr("Locations"));
    pTabWidget->addTab(pCharacterManipulator, tr("Characters"));
    pTabWidget->addTab(pEncounterManipulator, tr("Encounters"));

    QSplitter *pContentSplitter = new QSplitter();
    pContentSplitter->addWidget(pListObjectWidget);
    pContentSplitter->addWidget(pTabWidget);
    pContentSplitter->setStretchFactor(0, 0);
    pContentSplitter->setStretchFactor(1, 1);

    setCentralWidget(pContentSplitter);
    setWindowTitle(tr("My Little Investigations Case Creator"));

    pFileMenu = menuBar()->addMenu(tr("&File"));

    QAction *pNewProjectFile = new QAction(tr("&New project"), this);
    connect(pNewProjectFile, SIGNAL(triggered()), this, SLOT(CreateNewProject()));
    pFileMenu->addAction(pNewProjectFile);

    QAction *pOpenProjectFile = new QAction(tr("&Open project..."), this);
    connect(pOpenProjectFile, SIGNAL(triggered()), this, SLOT(OpenProject()));
    pFileMenu->addAction(pOpenProjectFile);

    QAction *pSaveProjectFile = new QAction(tr("&Save project"), this);
    connect(pSaveProjectFile, SIGNAL(triggered()), this, SLOT(SaveProject()));
    pFileMenu->addAction(pSaveProjectFile);

    pFileMenu->addSeparator();

    QAction *pCreateProjectFromCaseFileAction = new QAction(tr("&Create project from case file..."), this);
    connect(pCreateProjectFromCaseFileAction, SIGNAL(triggered()), this, SLOT(CreateProjectFromCaseFile()));
    pFileMenu->addAction(pCreateProjectFromCaseFileAction);

    QAction *pCompileProjectToCaseFileAction = new QAction(tr("Co&mpile project to case file"), this);
    connect(pCompileProjectToCaseFileAction, SIGNAL(triggered()), this, SLOT(CompileProjectToCaseFile()));
    pFileMenu->addAction(pCompileProjectToCaseFileAction);
}

void MainWindow::CaseLoaded()
{
    RefreshLists();
}

void MainWindow::SetMainListWidget(const QString &title, QListWidget *pListWidget)
{
    if (pMainListWidgetHolder != NULL)
    {
        pMainListWidgetLayout->removeWidget(pMainListWidgetHolder);
        delete pMainListWidgetHolder;
        pMainListWidgetHolder = NULL;
    }

    pMainListWidgetHolder = new ListWidgetHolder(title, pListWidget);
    pMainListWidgetLayout->addWidget(pMainListWidgetHolder);

    ClearSubListWidgets();
}

void MainWindow::AddSubListWidget(const QString &title, QListWidget *pListWidget)
{
    ListWidgetHolder *pSubListWidgetHolder = new ListWidgetHolder(title, pListWidget);
    pSubListWidgetsLayout->addWidget(pSubListWidgetHolder);
    subListWidgetHolders.append(pSubListWidgetHolder);
}

void MainWindow::ClearSubListWidgets()
{
    for (ListWidgetHolder *pSubListWidgetHolder : subListWidgetHolders)
    {
        pSubListWidgetsLayout->removeWidget(pSubListWidgetHolder);
        delete pSubListWidgetHolder;
    }

    subListWidgetHolders.clear();
}

void MainWindow::OnTabWidgetCurrentChanged(int newTabIndex)
{
    if (currentlyActiveTabIndex != newTabIndex)
    {
        if (currentlyActiveTabIndex >= 0)
        {
            listManipulators[currentlyActiveTabIndex]->SetIsActive(false);
        }

        listManipulators[newTabIndex]->SetIsActive(true);

        currentlyActiveTabIndex = newTabIndex;
    }
}

bool MainWindow::eventFilter(QObject *pObject, QEvent *pEvent)
{
    bool handled = false;

    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent *pKeyEvent = static_cast<QKeyEvent *>(pEvent);

        if (pKeyEvent->modifiers().testFlag(Qt::ControlModifier))
        {
            switch (pKeyEvent->key())
            {
            case Qt::Key_Z:
                if (pTabWidget->currentIndex() >= 0)
                {
                    handled = listManipulators[pTabWidget->currentIndex()]->Undo();
                }
                break;

            case Qt::Key_Y:
                if (pTabWidget->currentIndex() >= 0)
                {
                    handled = listManipulators[pTabWidget->currentIndex()]->Redo();
                }
                break;
            }
        }
    }

    if (handled)
    {
        return true;
    }
    else
    {
        return QMainWindow::eventFilter(pObject, pEvent);
    }
}


void MainWindow::CreateNewProject()
{
    CaseContent::ReplaceInstance();
    RefreshLists();
}

void MainWindow::OpenProject()
{
    QString startDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QString("/My Little Investigations/Case Creator Projects");
    QString projectFilePath = QFileDialog::getOpenFileName(this, tr("Select project file"), startDirectory, tr("MLI case creator project files (*.mliproj)"));

    if (projectFilePath.length() > 0)
    {
        CaseContent::ReplaceInstanceFromProjectFile(projectFilePath);
    }

    RefreshLists();
}

void MainWindow::SaveProject()
{
    CaseContent::GetInstance()->SaveToProjectFile();
}

void MainWindow::CreateProjectFromCaseFile()
{
    QString caseFilePath = QFileDialog::getOpenFileName(this, tr("Select case file"), QString(), tr("MLI case files (*.mlicase)"));

    if (caseFilePath.length() > 0)
    {
        NewProjectDialog newProjectDialog;

        if (newProjectDialog.exec() == QDialog::Accepted)
        {
            CaseContentLoader::GetInstance()->LoadCaseContentFromCaseFile(caseFilePath, newProjectDialog.GetProjectDirectoryPath(), newProjectDialog.GetProjectDirectoryFileName(), this);
        }
    }
}

void MainWindow::CompileProjectToCaseFile()
{
    CaseContent::GetInstance()->CompileToCaseFile("CompiledCase/Case.zip");
}

void MainWindow::RefreshLists()
{
    for (ListManipulatorBase *pManipulator : listManipulators)
    {
        pManipulator->RefreshList();
    }
}
