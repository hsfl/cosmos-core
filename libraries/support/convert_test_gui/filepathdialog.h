#ifndef FILEPATHDIALOG_H
#define FILEPATHDIALOG_H

#include <QDialog>

namespace Ui {
    class FilepathDialog;
}

class FilepathDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilepathDialog(QWidget *parent = 0);
    ~FilepathDialog();

    QString getFilepath();


private:
    Ui::FilepathDialog *ui;
};

#endif // FILEPATHDIALOG_H
