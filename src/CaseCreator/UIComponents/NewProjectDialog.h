#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QString>

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    QString GetProjectDirectoryPath()
    {
        return projectDirectoryPath;
    }

    QString GetProjectDirectoryFileName()
    {
        return projectName + ".mliproj";
    }

public slots:
    virtual int exec()
    {
        InitFields();
        return QDialog::exec();
    }

    virtual void accept()
    {
        if (ValidateFields())
        {
            QDialog::accept();
        }
        else
        {
            QApplication::beep();
        }
    }

    void RetrieveBaseProjectDirectoryPath();
    void DoValidateProjectName()
    {
        ValidateProjectName();
    }

protected:
    void InitFields();
    bool ValidateFields();

private:
    bool ValidateProjectName();
    void SetProjectDirectoryPath();

    QLabel *pProjectNamePromptLabel;
    QLineEdit *pProjectNameLineEdit;
    QLabel *pProjectNameErrorLabel;

    QLabel *pBaseProjectDirectoryPathPromptLabel;
    QLabel *pBaseProjectDirectoryPathLabel;
    QLabel *pBaseProjectDirectoryPathErrorLabel;

    QString projectName;
    QString baseProjectDirectoryPath;

    QString projectDirectoryPath;
};

#endif // NEWPROJECTDIALOG_H
