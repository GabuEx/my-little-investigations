#ifndef CASECONTENTLOADER_H
#define CASECONTENTLOADER_H

#include <QString>
#include <QObject>

#include "CaseContentLoaderProgressDialog.h"

class CaseContentLoader : public QObject
{
    Q_OBJECT

public:
    static CaseContentLoader * GetInstance()
    {
        if (pInstance == NULL)
        {
            pInstance = new CaseContentLoader();
        }

        return pInstance;
    }

    explicit CaseContentLoader();
    ~CaseContentLoader();

    void LoadCaseContentFromCaseFile(QString caseFilePath, QString projectFilePath, QString projectFileName, ICaseLoadedCallback *pCallback);

private:
    static CaseContentLoader *pInstance;

    CaseContentLoaderProgressDialog *pProgressDialog;
};

#endif // CASECONTENTLOADER_H
