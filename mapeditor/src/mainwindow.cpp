#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "tankmapwidget.h"
#include "newwizard.h"
#include <QSignalMapper>
#include <QMessageBox>
#include <QTextCodec>
#ifdef Q_WS_WIN
#  include <windows.h>
#  include "qtwin.h"
#  include <QPainter>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    newCounter(1)
{
    ui->setupUi(this);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    TankMapWidget::setEmptyButton(ui->btnEmpty);
    TankMapWidget::setEagleButton(ui->btnEagle);
    TankMapWidget::setP1Button(ui->btnP1);
    TankMapWidget::setP2Button(ui->btnP2);
    TankMapWidget::setEButton(ui->btnE);
#ifdef Q_WS_WIN
    HANDLE iconSm, iconBig;
    iconSm = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(10000), IMAGE_ICON, 16, 16, NULL);
    iconBig = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(10000), IMAGE_ICON, 32, 32, NULL);
    SendMessage(winId(), WM_SETICON, ICON_SMALL, (LPARAM) iconSm);
    SendMessage(winId(), WM_SETICON, ICON_BIG, (LPARAM) iconBig);
    ui->mainToolBar->setStyleSheet("QToolBar { border:0; }");
    if (QtWin::isCompositionEnabled()) {
        QtWin::extendFrameIntoClientArea(this, 0, ui->mainToolBar->sizeHint().height(), 0, 0);
    }
#else
    QIcon icon(":/images/icon.ico");
    setWindowIcon(icon);
#endif
    ui->actionNowy->setShortcuts(QKeySequence::New);
    ui->actionOtw_rz->setShortcuts(QKeySequence::Open);
    ui->actionZapisz->setShortcuts(QKeySequence::Save);
    ui->actionZamknij->setShortcuts(QKeySequence::Quit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNowy_triggered()
{
    NewWizard wizard;
    if(!wizard.exec()) return;
    QString mapName;
    mapName = (!wizard.getMapName().isEmpty() ? wizard.getMapName() : tr("Map ") + QString::number(newCounter));

    int index;
    TankMapWidget *widget = new TankMapWidget();
    widget->setDrawGrid(ui->actionPoka_siatk->isChecked());
    connect(ui->actionPoka_siatk, SIGNAL(toggled(bool)), widget, SLOT(setDrawGrid(bool)));

    switch(wizard.getTemplateType())
    {
    default:
    case 0:
        break;
    case 1:
        widget->load(":/templates/19x13.tlv");
        break;
    case 2:
        widget->load(":/templates/13x13.tlv");
        break;
    }

    widget->setMapName(mapName);
    index = ui->tabWidget->addTab(widget, mapName);

    ui->tabWidget->setCurrentIndex(index);
    newCounter++;

    TLV1_BonusRect br;
    br = widget->getBonusRect();
    ui->sbX->setValue(br.x);
    ui->sbY->setValue(br.y);
    ui->sbW->setValue(br.w);
    ui->sbH->setValue(br.h);

    TLV1_Point p, e1, e2, e3;
    bool cond;
    p = widget->getEagle();
    cond = (p.x == -1 && p.y == -1);
    ui->btnEagle->setEnabled(cond);
    p = widget->getSpawnP1();
    cond = (p.x == -1 && p.y == -1);
    ui->btnP1->setEnabled(cond);
    p = widget->getSpawnP2();
    cond = (p.x == -1 && p.y == -1);
    ui->btnP2->setEnabled(cond);
    e1 = widget->getSpawnE1();
    e2 = widget->getSpawnE2();
    e3 = widget->getSpawnE3();
    cond = ((e1.x == -1 && e1.y == -1) || (e2.x == -1 && e2.y == -1) || (e3.x == -1 && e3.y == -1));
    ui->btnE->setEnabled(cond);
}

void MainWindow::on_actionOtw_rz_triggered()
{
    int index;
    TankMapWidget *widget = new TankMapWidget();
    widget->setDrawGrid(ui->actionPoka_siatk->isChecked());
    connect(ui->actionPoka_siatk, SIGNAL(toggled(bool)), widget, SLOT(setDrawGrid(bool)));
    if(!widget->load()) return;
    index = ui->tabWidget->addTab(widget, widget->getMapName());

    ui->tabWidget->setCurrentIndex(index);

    TLV1_Point p, e1, e2, e3;
    bool cond;
    p = widget->getEagle();
    cond = (p.x == -1 && p.y == -1);
    ui->btnEagle->setEnabled(cond);
    p = widget->getSpawnP1();
    cond = (p.x == -1 && p.y == -1);
    ui->btnP1->setEnabled(cond);
    p = widget->getSpawnP2();
    cond = (p.x == -1 && p.y == -1);
    ui->btnP2->setEnabled(cond);
    e1 = widget->getSpawnE1();
    e2 = widget->getSpawnE2();
    e3 = widget->getSpawnE3();
    cond = ((e1.x == -1 && e1.y == -1) || (e2.x == -1 && e2.y == -1) || (e3.x == -1 && e3.y == -1));
    ui->btnE->setEnabled(cond);
}

void MainWindow::on_actionZapisz_triggered()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->save();
    }
}

void MainWindow::on_actionO_programie_triggered()
{
    AboutDialog *aboutdlg = new AboutDialog(this);
    aboutdlg->exec();
    delete aboutdlg;
}

void MainWindow::on_btnEmpty_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(EMPTY);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);
        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(false);
        ui->chk21->setEnabled(false);
        ui->chk31->setEnabled(false);
        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(false);
        ui->chk22->setEnabled(false);
        ui->chk32->setEnabled(false);
        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);
        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                checkChkbox(x, y, true);
            }
        }
    }
}

void MainWindow::on_btnBrick_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(BRICK);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);

        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(true);
        ui->chk21->setEnabled(true);
        ui->chk31->setEnabled(false);

        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(true);
        ui->chk22->setEnabled(true);
        ui->chk32->setEnabled(false);

        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);

        checkChkbox(1, 0, ui->chk11->isChecked());
        checkChkbox(0, 0, ui->chk11->isChecked());
        checkChkbox(0, 1, ui->chk11->isChecked());

        checkChkbox(2, 0, ui->chk21->isChecked());
        checkChkbox(3, 0, ui->chk21->isChecked());
        checkChkbox(3, 1, ui->chk21->isChecked());

        checkChkbox(1, 3, ui->chk12->isChecked());
        checkChkbox(0, 3, ui->chk12->isChecked());
        checkChkbox(0, 2, ui->chk12->isChecked());

        checkChkbox(2, 3, ui->chk22->isChecked());
        checkChkbox(3, 3, ui->chk22->isChecked());
        checkChkbox(3, 2, ui->chk22->isChecked());
    }
}

void MainWindow::on_btnSteel_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(STEEL);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);

        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(true);
        ui->chk21->setEnabled(true);
        ui->chk31->setEnabled(false);

        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(true);
        ui->chk22->setEnabled(true);
        ui->chk32->setEnabled(false);

        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);

        checkChkbox(1, 0, ui->chk11->isChecked());
        checkChkbox(0, 0, ui->chk11->isChecked());
        checkChkbox(0, 1, ui->chk11->isChecked());

        checkChkbox(2, 0, ui->chk21->isChecked());
        checkChkbox(3, 0, ui->chk21->isChecked());
        checkChkbox(3, 1, ui->chk21->isChecked());

        checkChkbox(1, 3, ui->chk12->isChecked());
        checkChkbox(0, 3, ui->chk12->isChecked());
        checkChkbox(0, 2, ui->chk12->isChecked());

        checkChkbox(2, 3, ui->chk22->isChecked());
        checkChkbox(3, 3, ui->chk22->isChecked());
        checkChkbox(3, 2, ui->chk22->isChecked());
    }
}

void MainWindow::on_btnSea_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(SEA);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);

        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(true);
        ui->chk21->setEnabled(true);
        ui->chk31->setEnabled(false);

        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(true);
        ui->chk22->setEnabled(true);
        ui->chk32->setEnabled(false);

        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);

        checkChkbox(1, 0, ui->chk11->isChecked());
        checkChkbox(0, 0, ui->chk11->isChecked());
        checkChkbox(0, 1, ui->chk11->isChecked());

        checkChkbox(2, 0, ui->chk21->isChecked());
        checkChkbox(3, 0, ui->chk21->isChecked());
        checkChkbox(3, 1, ui->chk21->isChecked());

        checkChkbox(1, 3, ui->chk12->isChecked());
        checkChkbox(0, 3, ui->chk12->isChecked());
        checkChkbox(0, 2, ui->chk12->isChecked());

        checkChkbox(2, 3, ui->chk22->isChecked());
        checkChkbox(3, 3, ui->chk22->isChecked());
        checkChkbox(3, 2, ui->chk22->isChecked());
    }
}

void MainWindow::on_btnJungle_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(JUNGLE);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);

        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(true);
        ui->chk21->setEnabled(true);
        ui->chk31->setEnabled(false);

        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(true);
        ui->chk22->setEnabled(true);
        ui->chk32->setEnabled(false);

        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);

        checkChkbox(1, 0, ui->chk11->isChecked());
        checkChkbox(0, 0, ui->chk11->isChecked());
        checkChkbox(0, 1, ui->chk11->isChecked());

        checkChkbox(2, 0, ui->chk21->isChecked());
        checkChkbox(3, 0, ui->chk21->isChecked());
        checkChkbox(3, 1, ui->chk21->isChecked());

        checkChkbox(1, 3, ui->chk12->isChecked());
        checkChkbox(0, 3, ui->chk12->isChecked());
        checkChkbox(0, 2, ui->chk12->isChecked());

        checkChkbox(2, 3, ui->chk22->isChecked());
        checkChkbox(3, 3, ui->chk22->isChecked());
        checkChkbox(3, 2, ui->chk22->isChecked());
    }
}

void MainWindow::on_btnIce_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(ICE);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);

        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(true);
        ui->chk21->setEnabled(true);
        ui->chk31->setEnabled(false);

        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(true);
        ui->chk22->setEnabled(true);
        ui->chk32->setEnabled(false);

        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);

        checkChkbox(1, 0, ui->chk11->isChecked());
        checkChkbox(0, 0, ui->chk11->isChecked());
        checkChkbox(0, 1, ui->chk11->isChecked());

        checkChkbox(2, 0, ui->chk21->isChecked());
        checkChkbox(3, 0, ui->chk21->isChecked());
        checkChkbox(3, 1, ui->chk21->isChecked());

        checkChkbox(1, 3, ui->chk12->isChecked());
        checkChkbox(0, 3, ui->chk12->isChecked());
        checkChkbox(0, 2, ui->chk12->isChecked());

        checkChkbox(2, 3, ui->chk22->isChecked());
        checkChkbox(3, 3, ui->chk22->isChecked());
        checkChkbox(3, 2, ui->chk22->isChecked());
    }
}

void MainWindow::on_btnEagle_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(EAGLE);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);
        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(false);
        ui->chk21->setEnabled(false);
        ui->chk31->setEnabled(false);
        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(false);
        ui->chk22->setEnabled(false);
        ui->chk32->setEnabled(false);
        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);
        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                checkChkbox(x, y, true);
            }
        }
    }
}

void MainWindow::on_btnEdge_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(EDGE);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);

        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(true);
        ui->chk21->setEnabled(true);
        ui->chk31->setEnabled(false);

        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(true);
        ui->chk22->setEnabled(true);
        ui->chk32->setEnabled(false);

        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);

        checkChkbox(1, 0, ui->chk11->isChecked());
        checkChkbox(0, 0, ui->chk11->isChecked());
        checkChkbox(0, 1, ui->chk11->isChecked());

        checkChkbox(2, 0, ui->chk21->isChecked());
        checkChkbox(3, 0, ui->chk21->isChecked());
        checkChkbox(3, 1, ui->chk21->isChecked());

        checkChkbox(1, 3, ui->chk12->isChecked());
        checkChkbox(0, 3, ui->chk12->isChecked());
        checkChkbox(0, 2, ui->chk12->isChecked());

        checkChkbox(2, 3, ui->chk22->isChecked());
        checkChkbox(3, 3, ui->chk22->isChecked());
        checkChkbox(3, 2, ui->chk22->isChecked());
    }
}

void MainWindow::on_btnP1_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(P1);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);
        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(false);
        ui->chk21->setEnabled(false);
        ui->chk31->setEnabled(false);
        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(false);
        ui->chk22->setEnabled(false);
        ui->chk32->setEnabled(false);
        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);
        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                checkChkbox(x, y, true);
            }
        }
    }
}

void MainWindow::on_btnP2_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(P2);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);
        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(false);
        ui->chk21->setEnabled(false);
        ui->chk31->setEnabled(false);
        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(false);
        ui->chk22->setEnabled(false);
        ui->chk32->setEnabled(false);
        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);
        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                checkChkbox(x, y, true);
            }
        }
    }
}

void MainWindow::on_btnE_clicked()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        widget->changeTool(E);
        ui->chk00->setEnabled(false);
        ui->chk10->setEnabled(false);
        ui->chk20->setEnabled(false);
        ui->chk30->setEnabled(false);
        ui->chk01->setEnabled(false);
        ui->chk11->setEnabled(false);
        ui->chk21->setEnabled(false);
        ui->chk31->setEnabled(false);
        ui->chk02->setEnabled(false);
        ui->chk12->setEnabled(false);
        ui->chk22->setEnabled(false);
        ui->chk32->setEnabled(false);
        ui->chk03->setEnabled(false);
        ui->chk13->setEnabled(false);
        ui->chk23->setEnabled(false);
        ui->chk33->setEnabled(false);
        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                checkChkbox(x, y, true);
            }
        }
    }
}

void MainWindow::on_chk11_toggled(bool checked)
{
    checkChkbox(1, 1, checked);
    checkChkbox(1, 0, checked);
    checkChkbox(0, 0, checked);
    checkChkbox(0, 1, checked);
}

void MainWindow::on_chk21_toggled(bool checked)
{
    checkChkbox(2, 1, checked);
    checkChkbox(2, 0, checked);
    checkChkbox(3, 0, checked);
    checkChkbox(3, 1, checked);
}

void MainWindow::on_chk12_toggled(bool checked)
{
    checkChkbox(1, 2, checked);
    checkChkbox(1, 3, checked);
    checkChkbox(0, 3, checked);
    checkChkbox(0, 2, checked);
}

void MainWindow::on_chk22_toggled(bool checked)
{
    checkChkbox(2, 2, checked);
    checkChkbox(2, 3, checked);
    checkChkbox(3, 3, checked);
    checkChkbox(3, 2, checked);
}

void MainWindow::on_sbX_valueChanged(int i)
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        TLV1_BonusRect br;
        br = widget->getBonusRect();
        widget->setBonusRect(i, br.y, br.w, br.h);
    }
}

void MainWindow::on_sbY_valueChanged(int i)
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        TLV1_BonusRect br;
        br = widget->getBonusRect();
        widget->setBonusRect(br.x, i, br.w, br.h);
    }
}

void MainWindow::on_sbW_valueChanged(int i)
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        TLV1_BonusRect br;
        br = widget->getBonusRect();
        widget->setBonusRect(br.x, br.y, i, br.h);
    }
}

void MainWindow::on_sbH_valueChanged(int i)
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        TLV1_BonusRect br;
        br = widget->getBonusRect();
        widget->setBonusRect(br.x, br.y, br.w, i);
    }
}

void MainWindow::on_mapName_textEdited(const QString &text)
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), text);
        widget->setMapName(text);
    }
}

void MainWindow::on_tabWidget_currentChanged()
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget != 0)
    {
        TLV1_BonusRect br;
        br = widget->getBonusRect();
        ui->sbX->setValue(br.x);
        ui->sbY->setValue(br.y);
        ui->sbW->setValue(br.w);
        ui->sbH->setValue(br.h);

        TLV1_Point p, e1, e2, e3;
        bool cond;
        p = widget->getEagle();
        cond = (p.x == -1 && p.y == -1);
        ui->btnEagle->setEnabled(cond);
        p = widget->getSpawnP1();
        cond = (p.x == -1 && p.y == -1);
        ui->btnP1->setEnabled(cond);
        p = widget->getSpawnP2();
        cond = (p.x == -1 && p.y == -1);
        ui->btnP2->setEnabled(cond);
        e1 = widget->getSpawnE1();
        e2 = widget->getSpawnE2();
        e3 = widget->getSpawnE3();
        cond = ((e1.x == -1 && e1.y == -1) || (e2.x == -1 && e2.y == -1) || (e3.x == -1 && e3.y == -1));
        ui->btnE->setEnabled(cond);

        ui->mapName->setText(widget->getMapName());
        ui->actionPoka_siatk->setChecked(widget->getDrawGrid());
        switch(widget->getSelectedTool())
        {
        case EMPTY:
            ui->btnEmpty->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);
            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(false);
            ui->chk21->setEnabled(false);
            ui->chk31->setEnabled(false);
            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(false);
            ui->chk22->setEnabled(false);
            ui->chk32->setEnabled(false);
            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case BRICK:
            ui->btnBrick->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);

            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(true);
            ui->chk21->setEnabled(true);
            ui->chk31->setEnabled(false);

            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(true);
            ui->chk22->setEnabled(true);
            ui->chk32->setEnabled(false);

            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case STEEL:
            ui->btnSteel->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);

            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(true);
            ui->chk21->setEnabled(true);
            ui->chk31->setEnabled(false);

            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(true);
            ui->chk22->setEnabled(true);
            ui->chk32->setEnabled(false);

            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case SEA:
            ui->btnSea->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);

            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(true);
            ui->chk21->setEnabled(true);
            ui->chk31->setEnabled(false);

            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(true);
            ui->chk22->setEnabled(true);
            ui->chk32->setEnabled(false);

            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case JUNGLE:
            ui->btnJungle->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);

            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(true);
            ui->chk21->setEnabled(true);
            ui->chk31->setEnabled(false);

            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(true);
            ui->chk22->setEnabled(true);
            ui->chk32->setEnabled(false);

            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case ICE:
            ui->btnIce->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);

            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(true);
            ui->chk21->setEnabled(true);
            ui->chk31->setEnabled(false);

            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(true);
            ui->chk22->setEnabled(true);
            ui->chk32->setEnabled(false);

            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case EAGLE:
            ui->btnEagle->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);
            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(false);
            ui->chk21->setEnabled(false);
            ui->chk31->setEnabled(false);
            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(false);
            ui->chk22->setEnabled(false);
            ui->chk32->setEnabled(false);
            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case EDGE:
            ui->btnEdge->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);

            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(true);
            ui->chk21->setEnabled(true);
            ui->chk31->setEnabled(false);

            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(true);
            ui->chk22->setEnabled(true);
            ui->chk32->setEnabled(false);

            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case P1:
            ui->btnP1->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);
            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(false);
            ui->chk21->setEnabled(false);
            ui->chk31->setEnabled(false);
            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(false);
            ui->chk22->setEnabled(false);
            ui->chk32->setEnabled(false);
            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case P2:
            ui->btnP2->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);
            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(false);
            ui->chk21->setEnabled(false);
            ui->chk31->setEnabled(false);
            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(false);
            ui->chk22->setEnabled(false);
            ui->chk32->setEnabled(false);
            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        case E:
            ui->btnE->setChecked(true);
            ui->chk00->setEnabled(false);
            ui->chk10->setEnabled(false);
            ui->chk20->setEnabled(false);
            ui->chk30->setEnabled(false);
            ui->chk01->setEnabled(false);
            ui->chk11->setEnabled(false);
            ui->chk21->setEnabled(false);
            ui->chk31->setEnabled(false);
            ui->chk02->setEnabled(false);
            ui->chk12->setEnabled(false);
            ui->chk22->setEnabled(false);
            ui->chk32->setEnabled(false);
            ui->chk03->setEnabled(false);
            ui->chk13->setEnabled(false);
            ui->chk23->setEnabled(false);
            ui->chk33->setEnabled(false);
            break;
        }

        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                checkChkbox(x, y, widget->getPart(x, y), false);
            }
        }
    }
}

void MainWindow::closeTab(int tab)
{
    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->widget(tab);
    if (widget)
    {
        if(widget->getChanged())
        {
            QMessageBox::StandardButton result =
                    QMessageBox::warning(
                        this,
                        tr("Unsaved changes"),
                        ui->tabWidget->tabText(tab) + tr(" contains unsaved changes. Do you want to save it now?"),
                        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                        QMessageBox::Save
                    );
            switch(result)
            {
            case QMessageBox::Save:
                widget->save();
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
                return;
            }
        }
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(widget));
        delete widget;
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
#ifdef Q_WS_WIN
    QPainter painter(this);
    painter.setBackground(QBrush(palette().color(QPalette::Window)));
    painter.eraseRect(QRect(0, ui->mainToolBar->sizeHint().height(), width(), height()));
#endif
}

void MainWindow::checkChkbox(int x, int y, bool b, bool setPart)
{
    if((x > 3 && x < 0) || (y > 3 && y < 0)) return;

    TankMapWidget *widget = (TankMapWidget *) ui->tabWidget->currentWidget();
    if(widget == 0) return;

    switch(x)
    {
    case 0:
        switch(y)
        {
        case 0:
            ui->chk00->setChecked(b);
            break;
        case 1:
            ui->chk01->setChecked(b);
            break;
        case 2:
            ui->chk02->setChecked(b);
            break;
        case 3:
            ui->chk03->setChecked(b);
            break;
        }
        break;
    case 1:
        switch(y)
        {
        case 0:
            ui->chk10->setChecked(b);
            break;
        case 1:
            ui->chk11->setChecked(b);
            break;
        case 2:
            ui->chk12->setChecked(b);
            break;
        case 3:
            ui->chk13->setChecked(b);
            break;
        }
        break;
    case 2:
        switch(y)
        {
        case 0:
            ui->chk20->setChecked(b);
            break;
        case 1:
            ui->chk21->setChecked(b);
            break;
        case 2:
            ui->chk22->setChecked(b);
            break;
        case 3:
            ui->chk23->setChecked(b);
            break;
        }
        break;
    case 3:
        switch(y)
        {
        case 0:
            ui->chk30->setChecked(b);
            break;
        case 1:
            ui->chk31->setChecked(b);
            break;
        case 2:
            ui->chk32->setChecked(b);
            break;
        case 3:
            ui->chk33->setChecked(b);
            break;
        }
        break;
    }
    if(setPart) widget->setPart(x, y, b);
}
