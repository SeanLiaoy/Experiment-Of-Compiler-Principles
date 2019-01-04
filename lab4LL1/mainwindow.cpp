#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("LL(1) 分析器  作者：廖思源");
    filePath = QDir::currentPath() + "/test.txt";
    this->analyzer = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()  // 点击打开
{
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个文法规则文件", ".", "Code(*.txt *.tiny )");

//    QString file_path = "/Users/sean/Workspaces/CppCode/Compiler_lab/lab4LL1/test.txt";

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
    file.close();
}

void MainWindow::on_pushButton_2_clicked()  // 点击处理
{
   std::string analyzerFilePath = ui->lineEdit->text().toStdString();
   analyzer = new Analyzer(analyzerFilePath);

   analyzer->log << "-------------原本语法如下---------------" << '\n';
   analyzer->printGrammers();
   analyzer->dealGrammers();
   analyzer->log << "-------------去除左递归后---------------" << '\n';
   analyzer->printGrammers();
   analyzer->simplify();
   analyzer->log << "-------------去除多余项后---------------" << '\n';
   analyzer->printGrammers();
   analyzer->dealLeftCommonGrammers();
   analyzer->log << "-------------去除左公因子后---------------" << '\n';
   analyzer->printGrammers();
   analyzer->makeFirst();
   analyzer->makeFollow();
   analyzer->makeTable();

   QTextEdit *textEdit = this->ui->textEdit_2;
   QString logPath = QString::fromStdString(analyzer->logFileName);
   QFile file(logPath);
   if(!file.open(QFile::ReadOnly | QFile::Text))
      return;
   QTextStream in(&file);
   while(!in.atEnd())
   {
       QString line = in.readLine();
       textEdit->append(line);
   }
   file.close();

   QTableWidget *table = ui->tableWidget;
   int rowCount = analyzer->nonFinalWord.size();
   int colCount = analyzer->finalWord.size();
   table->setRowCount(rowCount+1);
   table->setColumnCount(colCount+1);
   table->setItem(0, 0, new QTableWidgetItem("表头"));
   for(int i = 1; i < colCount+1; i++)
   {
       QString item = QString::fromStdString(analyzer->letter[i-1]);
       qDebug() << item;
       table->setItem(0, i, new QTableWidgetItem(item));
   }
   for(int j =0; j < rowCount; j++)
   {
       QString item = QString::fromStdString(analyzer->grammers[j].left);
       qDebug() << item;
       table->setItem(j+1, 0, new QTableWidgetItem(item));

       for(int k = 0; k < colCount; k++)
       {
           if(analyzer->table[j].count(analyzer->letter[k]))
           {
               vector<string> vs = analyzer->table[j][analyzer->letter[k]];
               string str = vs[0];

               for(size_t l = 1; l < vs.size(); l++)  str = str +  "\n" + vs[l];
               QString item = QString::fromStdString(str);
               table->setItem(j+1, k+1, new QTableWidgetItem(item));

           }

       }
   }

   QTextEdit *textEdit1 = this->ui->textEdit_7;
   for(LL1Node node : this->analyzer->grammers)
   {
       textEdit1->append(QString::fromStdString(node.toString()));
   }

}

void MainWindow::on_pushButton_4_clicked()  // 点击保存
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

void MainWindow::on_pushButton_3_clicked()
{
    string sentence = ui->lineEdit_2->text().toStdString();
    cout << sentence << endl;
    if(sentence.length() <= 1)  return;
    analyzer->analyzer(sentence);
    vector<string> &log1 = analyzer->analyzerLog1;
    vector<string> &log2 = analyzer->analyzerLog2;
    vector<string> &log3 = analyzer->analyzerLog3;
    vector<string> &log4 = analyzer->analyzerLog4;

    QTextEdit *text1 = this->ui->textEdit_3;
    QTextEdit *text2 = this->ui->textEdit_4;
    QTextEdit *text3 = this->ui->textEdit_5;
    QTextEdit *text4 = this->ui->textEdit_6;

    for(size_t i = 0; i < log1.size(); i++)
    {
        text1->append(QString::fromStdString(log1[i]));
        text2->append(QString::fromStdString(log2[i]));
        text3->append(QString::fromStdString(log3[i]));
        text4->append(QString::fromStdString(log4[i]));
    }

}
