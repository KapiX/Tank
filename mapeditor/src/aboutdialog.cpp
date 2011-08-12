#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->QtVersion->setText(tr("Qt version ") + QString(qVersion()));
    setFixedSize(width(), height());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
