#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include "scanner.h"
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Code(*.txt *.cpp *.mycpp *.h)");
    ui->lineEdit->setText(file_path);

    ui->textEdit->clear();
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text))
       return;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        ui->textEdit->append(line);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_2->clear();
    QString file_path = ui->lineEdit->text();
    scanner = new Scanner(file_path.toStdString().c_str());
    TokenType tokenType = BEGINFILE;
    while(tokenType != ENDFILE)
    {
        tokenType = scanner->getToken(true);
        const char *token = scanner->getTokenString();
        if(strlen(token) == 0) continue;
        QString msg = QString::fromStdString(scanner->getTokenMessage(token, tokenType));
        ui->textEdit_2->append(msg);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    scanner->resetFilePointer();
    TokenType tokenType = scanner->getToken();
    const char *token = scanner->getTokenString();
    if(tokenType != ERROR && tokenType != ENDFILE && strlen(token) != 0)
    {
        ui->textEdit->clear();
        std::string str = token;
        QString tokenStr = QString::fromStdString(str);
        TokenType curToken;
        bool addBlankNext = false;
        while((curToken = scanner->getToken()) != ENDFILE)
        {
            //特殊字符前后无需空格
            QString curTokenStr = scanner->getTokenString();
            if(noNeedBlank(curToken)) //是特殊字符
            {
                addBlankNext = false;
                tokenStr += curTokenStr;
            }
            else //是标识符或保留字
            {
                //if(addBlankNext == false) // int a=3;int b=4;
                qDebug() << curTokenStr;
                if(addBlankNext)
                {
                    curTokenStr = ' ' + curTokenStr;
                    std::cout << "添加空格！";
                }
                addBlankNext = true;
                tokenStr += curTokenStr;
            }
        }
        qDebug() << tokenStr;
        this->ui->textEdit->append(tokenStr);
    }
    else
    {
        return;
    }

}

void MainWindow::on_pushButton_4_clicked()
{
    QString save_path = ui->lineEdit->text();
    if(save_path.isEmpty())
    {
        QMessageBox::information(this,"Error","Please open a source first!");
        return;
    }
    QFile *file = new QFile;
    file->setFileName(save_path);
    if(file->open(QIODevice::WriteOnly))
    {
        QTextStream out(file);
        out << ui->textEdit->toPlainText();
        file->close();
        delete file;
        QMessageBox::information(this,"Done", "Sucessfully save file!");
    }
    else
    {
        QMessageBox::information(this,"Error", "Fail to save file!");
        delete file;
        return;
    }
}
