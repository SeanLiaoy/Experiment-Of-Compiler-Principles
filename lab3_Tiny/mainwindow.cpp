#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GLOBALS.H"
#include "UTIL.H"
#include "PARSE.H"
#include "SCAN.H"
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMessageBox>


/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;
FILE * log1;
FILE * log2;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;
int Error = FALSE;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    l1_path = QDir::currentPath() + "/log1.txt";
    l2_path = QDir::currentPath() + "/log2.txt";

    qDebug() << l1_path;
   // char *pgm = "/Users/sean/Workspaces/CppCode/Compiler_lab/lab3_Tiny/test.tiny";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个代码文件", ".", "Code(*.txt *.tiny )");
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

void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_2->clear();
    QString file_path = ui->lineEdit->text();
    const char *pgm = file_path.toStdString().c_str();

    source = fopen(pgm,"rw");
    if(source == NULL)
    {
        fprintf(stderr,"File %s not found\n",pgm);
        exit(1);
    }

    const char *log1_path = l1_path.toStdString().c_str();
    const char *log2_path = l2_path.toStdString().c_str();
    log1 = fopen(log1_path, "wb");
    log2 = fopen(log2_path, "wb");

    fprintf(log1, "Open file %s successfully", pgm);

    listing = log2; /* send listing to screen */
    fprintf(listing,"\nTINY COMPILATION: %s\n",pgm);

//    while(getToken() != ENDFILE);
    TreeNode *syntaxTree;
    syntaxTree = parse();
    if(TraceParse){
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }

    fclose(source);
    fclose(log1);
    fclose(log2);

    file_path = this->l2_path;
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly | QFile::Text))
       return;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        ui->textEdit_2->append(line);
    }

    resetScan();
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
