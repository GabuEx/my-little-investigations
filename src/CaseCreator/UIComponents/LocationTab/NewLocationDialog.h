#ifndef NEWLOCATIONWINDOW_H
#define NEWLOCATIONWINDOW_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

#include <QString>

class Location;

class NewLocationDialog : public NewObjectDialog<Location>
{
    Q_OBJECT

public:
    explicit NewLocationDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    Location * GetNewObject();

public slots:
    void RetrieveBackgroundFilePath();

protected:
    void InitFields();
    bool ValidateFields();

private:
    bool ValidateId();
    bool ValidateBackgroundFilePath(QString backgroundFilePath);

    QLabel *pIdPromptLabel;
    QLineEdit *pIdLineEdit;
    QLabel *pIdErrorLabel;

    QLabel *pBackgroundFilePathPromptLabel;
    QLabel *pBackgroundFilePathLabel;
    QLabel *pBackgroundFilePathErrorLabel;

    QString id;
    QString backgroundFilePath;
};

#endif // NEWLOCATIONWINDOW_H
