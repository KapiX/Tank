#ifndef NEWWIZARD_H
#define NEWWIZARD_H

#include <QWizard>

namespace Ui {
    class NewWizard;
}

class NewWizard : public QWizard
{
    Q_OBJECT

public:
    explicit NewWizard(QWidget *parent = 0);
    ~NewWizard();

    QString getMapName() const;
    int getTemplateType() const;

private:
    Ui::NewWizard *ui;
};
#endif // NEWWIZARD_H
