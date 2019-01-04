#include "assigimentseletionwindow.h"
#include "ui_assigimentseletionwindow.h"

AssigimentSeletionWindow::AssigimentSeletionWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AssigimentSeletionWindow)
{
    ui->setupUi(this);
}

AssigimentSeletionWindow::~AssigimentSeletionWindow()
{
    delete ui;
}

void AssigimentSeletionWindow::on_pushButton_clicked()
{
    MainWindow *w = new MainWindow(this);
    w->show();
}

void AssigimentSeletionWindow::on_pushButton_2_clicked()
{
    RegDialog *d = new RegDialog(this);
    d->show();
}
