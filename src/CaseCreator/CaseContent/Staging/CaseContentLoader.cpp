#include "CaseContentLoader.h"

#include "CaseContentLoadingStager.h"

#include "../CaseContent.h"
#include "CaseCreator/Utilities/Interfaces.h"

#include <QMessageBox>

CaseContentLoader *CaseContentLoader::pInstance = NULL;

CaseContentLoader::CaseContentLoader()
    : QObject(NULL)
{
    pProgressDialog = new CaseContentLoaderProgressDialog();
}

CaseContentLoader::~CaseContentLoader()
{
    delete pProgressDialog;
    pProgressDialog = NULL;
}

void CaseContentLoader::LoadCaseContentFromCaseFile(QString caseFilePath, QString projectFilePath, QString projectFileName, ICaseLoadedCallback *pCallback)
{
    CaseContentLoadingStager::BeginStaging(projectFilePath, projectFileName);

    pProgressDialog->Run(caseFilePath);

    QMessageBox finishedMessageBox(QMessageBox::Information, "Done creating project!", "Finished creating case project file and extracting files.", QMessageBox::Ok);
    finishedMessageBox.exec();

    CaseContent::ReplaceInstanceWithStager();
    CaseContentLoadingStager::EndStaging();

    CaseContent::GetInstance()->SaveToProjectFile();

    if (pCallback != NULL)
    {
        pCallback->CaseLoaded();
    }
}
