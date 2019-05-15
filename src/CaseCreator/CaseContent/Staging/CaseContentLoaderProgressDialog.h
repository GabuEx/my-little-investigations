#ifndef CASECONTENTLOADERPROGRESSDIALOG_H
#define CASECONTENTLOADERPROGRESSDIALOG_H

#include "CaseCreator/Utilities/Interfaces.h"

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QFutureWatcher>

class CaseContentLoaderProgressDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CaseContentLoaderProgressDialog(QWidget *parent = 0);
    static void LoadCaseContentFromCaseFileWorker(QString caseFilePath);

    void Run(QString caseFilePath);

signals:

public slots:
    virtual int exec();
    virtual void accept();

    void LoadCaseContentFromCaseFileFinished();

    void HandleProgressStageUpdated(QString currentStage);
    void HandleProgressPercentUpdated(int percentDone);
    void HandleProgressCurrentFileUpdated(QString currentFile);

private:
    QLabel *pStageLabel;
    QProgressBar *pProgressBar;
    QLabel *pCurrentActionLabel;
    int originalCurrentActionLabelWidth;

    QFutureWatcher<void> caseLoadWatcher;
    ICaseLoadedCallback *pCallback;
};

#endif // CASECONTENTLOADERPROGRESSDIALOG_H
