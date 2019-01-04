#include "mainwindow.h"
#include "scanner.h"
#include <QApplication>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include "nfa.h"
#include "regdialog.h"
#include "assigimentseletionwindow.h"
using namespace std;


void test()
{
//    reg expression: a(a*|bcd(e*|a))*|b|c
//    Right result: 16 18
//    string reg = "a(a*|bcd(e*|a))*|b";
//    NFA nfa = NfaMananger::getNFA(reg);

    NfaMananger mynfa("ab(c|d)*x*");
    cout << mynfa.nfa.nodeNums << endl;
    cout << "Test done!" << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    test();
//    RegDialog *regWindow = new RegDialog();
//    regWindow->show();
    AssigimentSeletionWindow w;
    w.show();
//    return 0;
    return a.exec();
}
