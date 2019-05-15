#include "CaseContentLoaderProgressDialog.h"

#include "CaseContentLoadingStager.h"

#include <QVBoxLayout>
#include <QProgressBar>
#include <QtConcurrent/QtConcurrent>

CaseContentLoaderProgressDialog::CaseContentLoaderProgressDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint)
{
    QObject::connect(&caseLoadWatcher, SIGNAL(finished()), this, SLOT(LoadCaseContentFromCaseFileFinished()));

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    pStageLabel = new QLabel();
    pStageLabel->setAlignment(Qt::AlignCenter);
    pMainLayout->addWidget(pStageLabel);

    pProgressBar = new QProgressBar();
    pProgressBar->setMinimum(0);
    pProgressBar->setMaximum(100);
    pProgressBar->setValue(0);
    pMainLayout->addWidget(pProgressBar);

    pCurrentActionLabel = new QLabel();
    pCurrentActionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    originalCurrentActionLabelWidth = -1;

    pMainLayout->addWidget(pCurrentActionLabel);

    setLayout(pMainLayout);
    setWindowTitle("Creating project...");

    setFixedSize(600, 100);
}

void CaseContentLoaderProgressDialog::Run(QString caseFilePath)
{
    QFuture<void> future = QtConcurrent::run(CaseContentLoaderProgressDialog::LoadCaseContentFromCaseFileWorker, caseFilePath);
    caseLoadWatcher.setFuture(future);

    exec();
}

void CaseContentLoaderProgressDialog::LoadCaseContentFromCaseFileWorker(QString caseFilePath)
{
    CaseContentLoadingStager::GetCurrent()->LoadFromCaseFile(caseFilePath);
}

void CaseContentLoaderProgressDialog::LoadCaseContentFromCaseFileFinished()
{
    accept();
}

int CaseContentLoaderProgressDialog::exec()
{
    QObject::connect(CaseContentLoadingStager::GetCurrent(), SIGNAL(ProgressStageUpdated(QString)), this, SLOT(HandleProgressStageUpdated(QString)), Qt::QueuedConnection);
    QObject::connect(CaseContentLoadingStager::GetCurrent(), SIGNAL(ProgressPercentUpdated(int)), this, SLOT(HandleProgressPercentUpdated(int)), Qt::QueuedConnection);
    QObject::connect(CaseContentLoadingStager::GetCurrent(), SIGNAL(ProgressCurrentFileUpdated(QString)), this, SLOT(HandleProgressCurrentFileUpdated(QString)), Qt::QueuedConnection);
    return QDialog::exec();
}

void CaseContentLoaderProgressDialog::accept()
{
    QDialog::accept();
    QObject::disconnect(CaseContentLoadingStager::GetCurrent(), SIGNAL(ProgressStageUpdated(QString)), this, SLOT(HandleProgressStageUpdated(QString)));
    QObject::disconnect(CaseContentLoadingStager::GetCurrent(), SIGNAL(ProgressPercentUpdated(int)), this, SLOT(HandleProgressPercentUpdated(int)));
    QObject::disconnect(CaseContentLoadingStager::GetCurrent(), SIGNAL(ProgressCurrentFileUpdated(QString)), this, SLOT(HandleProgressCurrentFileUpdated(QString)));
}

void CaseContentLoaderProgressDialog::HandleProgressStageUpdated(QString currentStage)
{
    pStageLabel->setText(currentStage);
    pStageLabel->update();
}

void CaseContentLoaderProgressDialog::HandleProgressPercentUpdated(int percentDone)
{
    pProgressBar->setValue(percentDone);
    pProgressBar->update();
}

void CaseContentLoaderProgressDialog::HandleProgressCurrentFileUpdated(QString currentFile)
{
    QFontMetrics currentActionLabelFontMetric(pCurrentActionLabel->font());

    if (originalCurrentActionLabelWidth < 0)
    {
        originalCurrentActionLabelWidth = pCurrentActionLabel->width();
    }

    pCurrentActionLabel->setText("Writing " + currentActionLabelFontMetric.elidedText(currentFile, Qt::ElideLeft, originalCurrentActionLabelWidth - currentActionLabelFontMetric.width("Writing ")));
    pCurrentActionLabel->update();
}
