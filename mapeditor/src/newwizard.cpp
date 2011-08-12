#include "newwizard.h"
#include "ui_newwizard.h"

NewWizard::NewWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::NewWizard)
{
    ui->setupUi(this);
}

NewWizard::~NewWizard()
{
    delete ui;
}

QString NewWizard::getMapName() const
{
    return ui->mapName->text();
}

int NewWizard::getTemplateType() const
{
    if(ui->modern->isChecked())
    {
        return 1;
    }
    else if(ui->classic->isChecked())
    {
        return 2;
    }
    else
    {
        return 0;
    }
}
