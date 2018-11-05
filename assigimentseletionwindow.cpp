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
