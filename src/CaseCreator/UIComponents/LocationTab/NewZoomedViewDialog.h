#ifndef NEWZOOMEDVIEWDIALOG_H
#define NEWZOOMEDVIEWDIALOG_H

#include "../TemplateHelpers/NewObjectDialog.h"

#include <QLabel>
#include <QLineEdit>

#include <QString>

class ZoomedView;

class NewZoomedViewDialog : public NewObjectDialog<ZoomedView>
{
    Q_OBJECT

public:
    explicit NewZoomedViewDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    ZoomedView * GetNewObject();

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

#endif // NEWZOOMEDVIEWDIALOG_H
