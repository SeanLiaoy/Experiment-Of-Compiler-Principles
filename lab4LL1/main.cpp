#include "mainwindow.h"
#include <QApplication>
#include "ll1analyzer.h"



// 参考1： https://blog.csdn.net/qq_24451605/article/details/50087683
// 参考2： https://blog.csdn.net/tangyuanzong/article/details/78796078
// 参考3:  https://blog.csdn.net/nk_test/article/details/51476663


void testFunc()
{
    string filePath = "/Users/sean/Workspaces/CppCode/Compiler_lab/lab4LL1/test.txt";
    Analyzer analyzer(filePath);

    analyzer.log << "-------------原本语法如下---------------" << '\n';
    analyzer.printGrammers();
    analyzer.dealGrammers();
    analyzer.log << "-------------去除左递归后---------------" << '\n';
    analyzer.printGrammers();
    analyzer.simplify();
    analyzer.log << "-------------去除多余项后---------------" << '\n';
    analyzer.printGrammers();
    analyzer.dealLeftCommonGrammers();
    analyzer.log << "-------------去除左公因子后---------------" << '\n';
    analyzer.printGrammers();
    analyzer.makeFirst();
    analyzer.makeFollow();
    analyzer.makeTable();

    vector<string> tokens;
    tokens.push_back("if");
    tokens.push_back("(");
    tokens.push_back("0");
    tokens.push_back(")");
    tokens.push_back("if");
    tokens.push_back("(");
    tokens.push_back("1");
    tokens.push_back(")");
    tokens.push_back("other");
    tokens.push_back("else");
    tokens.push_back("other");
    analyzer.analyzer(tokens);
    string sentence = "if(0) if(1) other else other";

    analyzer.analyzer(sentence);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

//    testFunc();
//    return 0;
}
