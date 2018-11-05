#include "regdialog.h"
#include "ui_regdialog.h"
#include "nfa.h"
#include <QString>
#include <string>
#include <iostream>

using namespace std;
RegDialog::RegDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("XLEX 作者：SeanLiao");
}

RegDialog::~RegDialog()
{
    delete ui;
}

void RegDialog::on_pushButton_clicked()
{
    QString text=ui->lineEdit->text();
    string regExp = text.toStdString();
    NfaMananger manager(regExp);


    // 显示nfa
    QTableWidget *nfaTable = ui->tableWidget;
    int nfaSize = manager.nfa.nodeNums;
    nfaTable->setRowCount(nfaSize);
    nfaTable->setColumnCount(nfaSize);
    for(int i = 0; i < nfaSize; i++)
    {
        for(int j = 0; j < nfaSize; j++)
        {
            string ch = "";
            ch += manager.graph[i+1][j+1];
            if(ch == " ")   ch = "(None)";
            QString item = QString::fromStdString(ch);
            nfaTable->setItem(i,j, new QTableWidgetItem(item));
        }
    }

    QLabel *begin_label1 = ui->beginNodesId;
    QLabel *end_label1 = ui->endNodesId;
    QLabel *begin_label2 = ui->beginNodesId_2;
    QLabel *end_label2 = ui->endNodesId_2;
    QLabel *begin_label3 = ui->beginNodesId_3;
    QLabel *end_label3 = ui->endNodesId_3;
    if(nfaSize>0)
    {

        begin_label1->setText(QString::fromStdString(to_string(manager.nfa.startID())));
        end_label1->setText(QString::fromStdString(to_string(manager.nfa.endID())));
    }

    QTableWidget *dfaTable = ui->tableWidget_2;
    int colCount = manager.dfa.word_list.size();
    int rowCount = manager.dfa.graph.size();
    dfaTable->setRowCount(rowCount);
    dfaTable->setColumnCount(colCount);

    QStringList colNames;
    QStringList rowNames;
    for(int j = 0; j < rowCount; j++)
    {
        string node = manager.dfa.graph[j]->toString();
        rowNames.append(QString::fromStdString(node));
    }

    int i = 0;
    for(char ch : manager.dfa.word_list)
    {
        string col = "";
        col += ch;
        colNames.push_back(QString::fromStdString(col));
        for(int j = 0; j < rowCount; j++)
        {
            string transformResult;
            DFANode* node = manager.dfa.graph[j]->transform(ch);
            if(node == NULL)
            {
                transformResult = "(None)";
            }
            else
            {
                transformResult = node->toString();
            }
            dfaTable->setItem(j,i,new QTableWidgetItem(QString::fromStdString(transformResult)));
        }
        i++;
    }
    dfaTable->setHorizontalHeaderLabels(colNames);
    dfaTable->setVerticalHeaderLabels(rowNames);

    string startNodeText = "";
    string endNodeText = "";
    for(DFANode *n : manager.dfa.graph)
    {
        if(n->includeID(1))
            startNodeText = n->toString();
        if(n->state == END)
        {
            endNodeText += " " + n->toString();
        }
    }
    begin_label2->setText(QString::fromStdString(startNodeText));
    end_label2->setText(QString::fromStdString(endNodeText));

    QTableWidget *miniDfaTable = ui->tableWidget_3;
    manager.minimizeDFA();
    rowCount = manager.finalMiniDFA.graph.size();
    miniDfaTable->setRowCount(rowCount);
    miniDfaTable->setColumnCount(colCount);

    rowNames.clear();
    for(int j = 0; j < rowCount; j++)
    {
        string node = manager.finalMiniDFA.graph[j]->miniName;
        rowNames.append(QString::fromStdString(node));
    }
    i = 0;
    for(char ch : manager.dfa.word_list)
    {
        for(int j = 0; j < rowCount; j++)
        {
            string transformResult;
            DFANode *node = manager.finalMiniDFA.graph[j]->transform(ch);
            if(node == NULL)
            {
                transformResult = "(None)";
            }
            else
            {
                transformResult = node->miniName;
            }
            miniDfaTable->setItem(j, i, new QTableWidgetItem(QString::fromStdString(transformResult)));
        }
        i++;
    }
    miniDfaTable->setHorizontalHeaderLabels(colNames);
    miniDfaTable->setVerticalHeaderLabels(rowNames);

    startNodeText = "";
    endNodeText = "";
    for(DFANode *n : manager.finalMiniDFA.miniEndNodes)
    {
        endNodeText += " " + n->miniName;
    }
    startNodeText = manager.finalMiniDFA.miniStartNode->miniName;

    begin_label3->setText(QString::fromStdString(startNodeText));
    end_label3->setText(QString::fromStdString(endNodeText));


    manager.miniDfaToCodes();
    QTextEdit *codeText = ui->textEdit;
    codeText->setText(QString::fromStdString(manager.codes));
}
