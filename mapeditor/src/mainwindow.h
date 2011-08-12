#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tankmapwidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNowy_triggered();
    void on_actionOtw_rz_triggered();
    void on_actionZapisz_triggered();
    void on_actionO_programie_triggered();
    void on_btnEmpty_clicked();
    void on_btnBrick_clicked();
    void on_btnSteel_clicked();
    void on_btnSea_clicked();
    void on_btnJungle_clicked();
    void on_btnIce_clicked();
    void on_btnEagle_clicked();
    void on_btnEdge_clicked();
    void on_btnP1_clicked();
    void on_btnP2_clicked();
    void on_btnE_clicked();
    void on_chk11_toggled(bool checked);
    void on_chk21_toggled(bool checked);
    void on_chk12_toggled(bool checked);
    void on_chk22_toggled(bool checked);

    void on_sbX_valueChanged(int i);
    void on_sbY_valueChanged(int i);
    void on_sbW_valueChanged(int i);
    void on_sbH_valueChanged(int i);

    void on_mapName_textEdited(const QString &text);

    void on_tabWidget_currentChanged();

    void closeTab(int tab);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;

    int newCounter;

    void checkChkbox(int x, int y, bool b, bool setPart = true);
};

#endif // MAINWINDOW_H
