/* Thompson way to generate NFA/DFA from reg expression
 * Author: SeanLiao
 * Date: 2018.11.1
 *
 */

#ifndef NFA_H
#define NFA_H
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <map>

const int NODE_MAX_NUM = 200;
const int ID_DEFAULT = -1;
const char WORD_DEFAULT = ' ';
const char WORD_EPSILON = '#';

enum State{
  START,
    END,
    NONE
};

struct NFAEdge;

struct NFANode
{
    int id;
    State state;

    std::vector<NFAEdge> in_edges;
    std::vector<NFAEdge> out_edges;

    NFANode(int n, State s) : id(n), state(s) {}
    NFANode() : id(ID_DEFAULT), state(NONE) {}
};

struct NFAEdge
{
   NFANode *start_node, *end_node;
   char word;  // 接受的字符

   NFAEdge(NFANode *n1, NFANode *n2, char w):start_node(n1), end_node(n2),word(w) {}
};


class NFA
{
public:

    NFANode *start_node, *end_node;
    int nodeNums;

    NFA() : start_node(NULL), end_node(NULL), nodeNums(2) {}
    NFA(char c, char id1, char id2);  // 构造一个识别c的NFA
    NFA(char c); // 构造一个标号未定的识别c的NFA
    NFA(NFANode *st, NFANode *ed) : start_node(st), end_node(ed) {}

    void operator=(const NFA &nfa);  //浅复制

    void buildCell(char c);
    void Unite(const NFA &nfa);  // a | b
    void Joint(const NFA &nfa);  // ab
    void Star();  // a*

    bool isEmptyNFA() const;
    bool isEndID(int id) const;

    int endID() const;
    int startID() const;
};


struct DFANode;

struct DFAEdge
{
    char word;
    DFANode *next;

    DFAEdge(char w, DFANode *n) : word(w), next(n) {}
};

struct DFANode
{
    std::set<int> node;
    std::vector<DFAEdge> edges;

    std::string miniName;
    State state{NONE};

    bool includeID(int id);
    void insertID(int id);
    void insertEdge(DFAEdge edge);
    void unionNode(DFANode *n);

    DFANode *transform(char word); // 该结点接受word字符后的转化为的结点
    std::string toString();
};

class DFA
{
public:
    std::vector<DFANode*> graph;
    std::vector<DFANode*> getNodes(int id);
    DFANode *miniStartNode;
    std::vector<DFANode*> miniEndNodes;
    std::set<char> word_list;

    DFANode* crateNewNode(int id);

    static bool equals(DFANode *node1, DFANode *node2, std::set<char> words);

    bool isEndNode(DFANode *node) const;
    void delNode(DFANode *node);    // 从当前DFA中删除某个结点
    void printResult(); // 打印当前DFA结果到控制台
    void getCurrentDfaNodesCode(DFANode *n, std::vector<std::string> &lines, int tabsnum);

    void getCCode(DFANode *n, std::vector<std::string> &lines, int tabsnum);
};

class NfaMananger
{
private:
    std::vector<int> findDFANodeConnections(int id);


public:
    std::string reg_exp;

    NFA nfa;
    DFA dfa;
    std::vector<std::vector<DFANode*> > miniDFA;
    DFA finalMiniDFA;
    char graph[NODE_MAX_NUM][NODE_MAX_NUM];

    std::string codes;  // 最小DFA转换成的代码
    std::set<char> word_list;

    NfaMananger(const std::string &re);

    static NFA getNFA(const std::string &post_reg);
    static std::vector<int> getRegExpLeapIndex(const std::string &reg);
    static int getMatchRightParmIndex(const std::string &reg, int leftParamIndex);
    static bool isLetter(char ch);

    void serializeNFA();  // 给NFA的结点编号并建立初始DFA结点
    void dealDFA();  // 初始DFA图后处理
    void minimizeDFA();     // 最小化DFA(划分法）
    void dealMiniDFA();     // 最小DFA图后处理
    void miniDfaToCodes();  // 将最小DFA图转换为代码

    void miniDfaToC();  //将最小DFA图转换为C语言代码
};


#endif // NFA_H
