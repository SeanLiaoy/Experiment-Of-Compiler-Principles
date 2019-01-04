#ifndef LL1ANALYZER_H
#define LL1ANALYZER_H

#include "ll1node.h"
#include <map>
#include <fstream>
#include <stack>

const int MAX_WORD_COUNT = 600;

class Analyzer
{
private:
    bool used[MAX_WORD_COUNT];

    int getNonFinalWordIndex(string word);
    void dfs(int i);
    void dfsFindFirst(int i);

    vector<LL1Node> dealCommonGrammer(LL1Node &node);
    void append(const string &str1, const string &str2);

    void printStep(int step, stack<string> stk, vector<string> src, string wf, int x);

public:
    int startWordIndex;
    string logFileName = "log.txt";
    ofstream log;
    vector<LL1Node> grammers;
    vector<string> finalWord;  // 终结符号
    vector<string> nonFinalWord; // 非终结符号
    vector<string> letter;

    map<string, set<string>> first;
    map<string, set<string>> follow;
    vector< map<string, vector<string> > > table;

    Analyzer(string file_path);

    void initWords();  // 初始化终结符号和非终结符号

    // 参考：https://blog.csdn.net/qq_24451605/article/details/50075467
    void dealGrammers();  // 消除左递归
    void dealLeftCommonGrammers();  // 提取左因子问题
    void simplify();    // 化简


    void makeFirst();
    void makeFollow();
    void makeTable();

    bool isFinalWord(string word);
    bool isWord(string word);

    bool checkFirst(const vector<string> &text,const string &str);
    bool checkFollow(const string &text,const string &str);


    vector<string> analyzerLog1, analyzerLog2, analyzerLog3, analyzerLog4;


    void analyzer(vector<string> &text);
    void analyzer(string &sentence);
    vector<string> splitSentence(string &sentence);

    void printGrammers();
};


vector<string> splitString(string str, const string &spliter);
string& trim(string &s);


#endif // LL1ANALYZER_H
