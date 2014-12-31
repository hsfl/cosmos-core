#include "filepathdialog.h"
#include "ui_filepathdialog.h"

FilepathDialog::FilepathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilepathDialog)
{
    ui->setupUi(this);
}

FilepathDialog::~FilepathDialog()
{
    delete ui;
}

QString FilepathDialog::getFilepath()
{
    return (ui->filepathfield->text());
}
