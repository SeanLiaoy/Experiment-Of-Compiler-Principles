#include "nfa.h"
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <map>
#include <set>
using namespace std;


NfaMananger::NfaMananger(const string &re) : reg_exp(re)
{
    memset(graph, WORD_DEFAULT, sizeof(graph));
    this->nfa = getNFA(this->reg_exp);
    this->serializeNFA();
}

/* 正则表达式转NFA递归方法思路：
 * 首先把(..) 看成一个单元素NFA,和a等价
 * 把单个NFA看成一个或数个元素的Union，即 NFA = a[|b|c...]
 * 扫描正则表达式，首先扫描 | 进行拆分递归
 * 逐项建立NFA并连接对于括号进行递归处理
 */
NFA NfaMananger::getNFA(const string &reg)
{
    cout << "In getNFA, the reg is " << reg << endl;
    int len = reg.length();
    vector<int> leapIds = getRegExpLeapIndex(reg);
    // 先递归拆分 '|'
    if(leapIds.size() != 0)
    {
        vector<NFA> nfas;
        string sub_reg;
        int begin = 0;
        for(int i = 0; i < leapIds.size(); i++)
        {
            sub_reg = reg.substr(begin, leapIds[i]-begin);
            begin = leapIds[i] + 1;
            nfas.push_back(getNFA(sub_reg));
        }

        sub_reg = reg.substr(begin, reg.size()-begin);
        nfas.push_back(getNFA(sub_reg));

        NFANode *node1 = new NFANode();
        NFANode *node2 = new NFANode();
        for(NFA nfa : nfas)
        {
            NFAEdge edge1(node1, nfa.start_node, WORD_EPSILON);
            NFAEdge edge2(nfa.end_node, node2, WORD_EPSILON);

            node1->out_edges.push_back(edge1);
            cout << node1->out_edges.size() << endl;
            node2->in_edges.push_back(edge2);
            cout << node2->in_edges.size() << endl;

            nfa.start_node->in_edges.push_back(edge1);
            cout << nfa.start_node->in_edges.size() << endl;
            nfa.end_node->out_edges.push_back(edge2);
            cout << nfa.end_node->out_edges.size() << endl;
        }

        return NFA(node1, node2);
    }

    // 递归拆分括号
    NFA nfa;
    char cur,next;
    for(int i = 0; i < len-1; i++)
    {
        cur = reg[i], next=reg[i+1];
        if(isLetter(reg[i])) // 是待匹配元素
        {
            NFA cell(cur);
            if(next == '*')  // 闭包优先级比较高，先进行闭包再连接
            {
                cell.Star();
                i++;
                cout << "Scanning " << cur << " and next is *" << endl;
            }
            if(nfa.isEmptyNFA())  // 是在reg的开头，初始化当前nfa(浅复制)
                nfa = cell;
            else
                nfa.Joint(cell);    // 否则直接连接
        }
        else if(reg[i] == '(')  // 进行递归
        {
            int matchParamIndex = getMatchRightParmIndex(reg, i);
            if( matchParamIndex == -1)
            {
                cout << "Error when parsing the sub reg expression" << endl;
                exit(-1);
            }
            string subReg = reg.substr(i+1, matchParamIndex-1-i);
            next = reg[matchParamIndex + 1];

            if(next == '*')
            {
                cout << "Dealing *" << endl;
                if(nfa.isEmptyNFA())
                {
                    nfa = getNFA(subReg);
                    nfa.Star();
                    i = matchParamIndex + 2;
                }
                else
                {
                    NFA sub_nfa = getNFA(subReg);
                    sub_nfa.Star();
                    nfa.Joint(sub_nfa);
                    i = matchParamIndex + 1;
                }
            }
            else
            {
                if(nfa.isEmptyNFA())
                    nfa = getNFA(subReg);
                else
                    nfa.Joint(getNFA(subReg));

                i = matchParamIndex + 1;
            }

        }
    }

    // 最后再处理最后一个
    if(isLetter(reg[len-1]))
    {
        if(nfa.isEmptyNFA())
            nfa = NFA(reg[len-1]);
        else
            nfa.Joint(NFA(reg[len-1]));
    }
    cout << "Get NFA " << reg << endl;
    return nfa;
}

vector<int> NfaMananger::getRegExpLeapIndex(const string &reg)
{
    vector<int> LeapId;
    cout << "In getRegExpLeapIndex()" << '\t';
    cout << "Scanning " << reg  << endl;
    int len = reg.length();
    for(int i = 0; i < len; i++)
    {
        if(reg[i] == '|')
        {
            LeapId.push_back(i);
        }
        else if(reg[i] == '(')
        {
            int nextIndex = getMatchRightParmIndex(reg,i);
            if(nextIndex == -1)
            {
                cout << "Error when scanning the expression" << endl;
                exit(-1);
            }
            i = nextIndex + 1;
        }
    }
    return LeapId;
}

int NfaMananger::getMatchRightParmIndex(const string &reg, int leftParamIndex)
{
    int nRightParam = 0;
    for(int i = leftParamIndex+1; i < reg.length(); i++)
    {
        if(reg[i] == ')')
        {
            if(nRightParam == 0)
                return i;
            else
                nRightParam--;
        }
        else if(reg[i] == '(')
        {
            nRightParam++;
        }
    }

    return -1;
}

bool NfaMananger::isLetter(char ch)
{
    return isdigit(ch)||isalpha(ch);
}

void NfaMananger::serializeNFA() // 给建立好的NFA图编号,并转化为DFA
{
    if(nfa.isEmptyNFA())    return;
    cout << "In serializeNFA()" << endl;

    nfa.start_node->state = START;
    nfa.end_node->state = END;

    NFANode *begin = nfa.start_node;
    int id = 1;
    begin->id = id++;

    DFANode dfa_node;
    dfa_node.node.insert(begin->id);

    // bfs
    queue<NFANode *> Q, QT;  // QT: 新的DFA结点  Q: 当前DFA结点的epsilon闭包
    QT.push(begin);

    bool done[NODE_MAX_NUM];  // 防止死循环(如闭包时)
    memset(done,0,sizeof(done));
    while(!QT.empty())
    {
        NFANode *node_from = QT.front();
        QT.pop();
        DFANode *dfa_node = dfa.crateNewNode(node_from->id);
        done[node_from->id] = true;
        Q.push(node_from);
        cout << "开始寻找" << node_from->id <<"的Epsilon闭包" << endl;

        bool done1[NODE_MAX_NUM];
        memset(done1,0,sizeof(done1));
        while(!Q.empty())
        {
            NFANode *node = Q.front();
            Q.pop();
            done1[node->id] = true;

            for(NFAEdge edge : node->out_edges)
            {
                if(edge.end_node->id == ID_DEFAULT)
                {
                    edge.end_node->id = id++;
                }
                graph[edge.start_node->id][edge.end_node->id] = edge.word;
                cout << edge.start_node->id << "--[" << edge.word << "]-->" << edge.end_node->id << endl;
                if(edge.word == WORD_EPSILON)  // epsilon边，加入当前epsilon闭包
                {
                    dfa_node->node.insert(edge.end_node->id);
                    if(!done1[edge.end_node->id])
                        Q.push(edge.end_node);
                }
                else  // 非epsilon边，加入新闭包队列
                {
                    word_list.insert(edge.word);


                    if(!done[edge.end_node->id])
                    {
                        cout << "发现" << edge.end_node->id << "可以作为新的点集合" << endl;
                        done[edge.end_node->id] = true;
                        QT.push(edge.end_node); // 这时不可以直接加DFA边，因为这个结点还没创建好
                    }
                }
            }
        }

        if(dfa_node->includeID(nfa.endID()))
            dfa_node->state = END;
    }

    cout << "NFA Start id:" << nfa.start_node->id  << "\t" << "End id:" << nfa.end_node->id << endl;
    nfa.nodeNums = id-1;

    cout << "Before deal DFA, DFA size is:" << dfa.graph.size() << endl;
    dealDFA();
    cout << "Now mnimize DFA size is: "  << finalMiniDFA.graph.size() << endl;

    // 测试结果
    int dfa_set_id = 0;
    for(DFANode *node : finalMiniDFA.graph)
    {
        cout  << "ID Set " << ++dfa_set_id << ":\t";
        for(int id : node->node)
        {

            cout << id << ' ';
        }
        if(node->state == END)  cout << "END state";
        cout << endl;
    }

    // 测试NFA图的建立情况
//    for(int i = 1; i <= nfa.nodeNums; i++)
//    {
//        for(int j = 1; j <= nfa.nodeNums; j++)
//        {
//            cout << i << "--[" << graph[i][j] << "]--" << j << endl;
//        }
//    }

}

vector<int> NfaMananger::findDFANodeConnections(int id)
{
    vector<int> V;
    for(int i = 1; i <= nfa.nodeNums; i++)
    {
        if(graph[id][i] != WORD_DEFAULT && graph[id][i] != WORD_EPSILON)
        {
            V.push_back(i);
        }
    }
    return V;
}

void NfaMananger::dealDFA()
{
    // 建立DFA边
    for(int i = 0; i < dfa.graph.size(); i++)
    {
        DFANode *dnode = dfa.graph[i];
        map<char, vector<DFANode*> > M;  // 处理相同字符连接不同node的情况，如正则表达式为 a|ab
        for(int id : dnode->node) // 当前集合中所有结点
        {
            vector<int> V = findDFANodeConnections(id);
            for(int j : V) // 转换结点id
            {
                DFANode *node = dfa.getNodes(j)[0];  // 一般来说转换边结点编号对应的结点是唯一的，即不会被多个集合包含
                char word = graph[id][j];
                DFAEdge edge(word, node);
                dnode->insertEdge(edge);
                M[word].push_back(node);
            }
        }
        map<char, vector<DFANode*>>::iterator it;
        for(it = M.begin(); it != M.end(); it++)
        {
            vector<DFANode *> nodes = it->second;
            int sz = nodes.size();

            if(sz >= 2)
            {
                for(int i = 1; i < sz; i++)
                {
                    nodes[0]->unionNode(nodes[i]);
                    dfa.delNode(nodes[i]);
                }
            }
        }
    }
    dfa.word_list.insert(this->word_list.begin(),this->word_list.end());
    dfa.printResult();

//    minimizeDFA();
//    cout << "最小化完成，结果如下" << endl;
//    for(int i = 0; i < miniDFA.size(); i++)
//    {
//        cout << "状态"  << i  << ":";
//        for(DFANode* n : miniDFA[i])
//        {
//            cout << n->toString();
//        }
//        cout << endl;
//    }
//    cout << "下面对最小化DFA转换表进行后处理" << endl;
//    dealMiniDFA();
//    cout << "处理完毕，Final minimize DFA:" << endl;
//    finalMiniDFA.printResult();
//    cout << "转换DFA完毕" << endl;
}

inline bool contains(const vector<DFANode*> &s,const DFANode* node)
{
    for(DFANode* n : s)
    {
        if(n==node)
            return true;
    }
    return false;
}

void NfaMananger::minimizeDFA()
{
    vector<DFANode*> s1,s2;   // s1: 非终态集合， s2: 终态集合
    for(DFANode *node : dfa.graph)
    {
        if(node->state == END)
        {
            s2.push_back(node);
        }
        else
        {
            s1.push_back(node);
        }
    }

    cout << "非终态结点s1：";
    for(DFANode *n : s1)    cout << n->toString() << '\t';
    cout << endl;
    cout << "终态结点s2: ";
    for(DFANode *n : s2) cout << n->toString() << '\t';
    cout << endl;

    queue<vector<DFANode*> > states;  // 等待划分的状态集合队列
    states.push(s1);
    states.push(s2);

    //int node_nums = dfa.graph.size();

    // 先拆分非终态集合
    while(!states.empty())
    {
        vector<DFANode*> s = states.front();
        states.pop();
        if(s.empty())   continue;
        int sz = s.size();
        if(sz == 1)
        {
            miniDFA.push_back(s);
            continue;
        }
        vector<vector<DFANode*> > sub_states;

        DFANode *node = s[0];
        bool ok = true;
        cout << "以" << node->toString() << "为标准进行划分" << endl;
        for(int i = 1; i < sz; i++)
        {
            // 两个结点的转换结果不一样，则需要进行拆分
            if(DFA::equals(s[i], node,this->word_list) == false)
            {
                cout << s[i]->toString() << "与" << node->toString() << "转换表不同" << endl;
                // 将与第i个结点转换结果相同的结点都拿出来(包括i)
                vector<DFANode*> cur_states;
                for(vector<DFANode*>::iterator it = s.begin()+1; it != s.end();)
                {
                    if(DFA::equals((*it), s[i], this->word_list) )
                    {
                        cout << (*it)->toString() << "与" << s[i]->toString() << "转换表一致" << endl;
                        cur_states.push_back(*it);  // 加入新集合
                        it = s.erase(it);  // 在s中删除该结点, 更新it为原it的下一个位置
                    }
                    else
                    {
                        ++it;
                    }
                }
                sub_states.push_back(cur_states);
                // 因为有删除s中的元素，要更新一下s的大小
                sz = s.size();
                ok = false;
            }
        }
        if(ok)
        {
            cout << "该状态集合转换结果完全相同，无需进行划分" << endl;
            miniDFA.push_back(s);
        }
        else
        {
            sub_states.push_back(s);
            cout << "加入" << sub_states.size() << "个新状态" << endl;
            for(vector<DFANode*> v : sub_states)
            {
                states.push(v);
            }
        }
        cout << "等待划分队列大小：" << states.size() << endl;
    }

    dealMiniDFA(); // 进行后处理
}


inline bool differentEdge(vector<DFAEdge> edges, DFAEdge e)
{
    for(DFAEdge edge : edges)
    {
        if(edge.word == e.word && edge.next == e.next)
            return false;
    }
    return true;
}


// 划分法处理最小化DFA后处理
void NfaMananger::dealMiniDFA()
{
    int state_nums = miniDFA.size();
    cout << "最小DFA状态数：" <<  state_nums << endl;
    for(int i = 0; i < state_nums; i++)
    {
        vector<DFANode*> cur_state = miniDFA[i];
        vector<DFAEdge> cur_edges;
        for(DFANode* node : cur_state)  // 对当前行中的所有DFA结点
        {

            for(DFAEdge edge : node->edges) // 对其所有边
            {
                for(int j = 0; j < state_nums; j++)  // 查看这条边是到哪个新状态的
                {
                    if(contains(miniDFA[j], edge.next))
                    {
                        cout << "添加 状态"<<i<<"--[" << edge.word << "]-->" <<"状态" << j << endl;
                        DFAEdge e(edge.word, miniDFA[j][0]);
                        if(differentEdge(cur_edges, e))   cur_edges.push_back(e);
                        // 为什么下标0？因为把每一行状态都合并到第一个元素
                        finalMiniDFA.word_list.insert(edge.word);
                    }
                }
            }
        }

        for(int j = 1; j < cur_state.size(); j++)
        {
            cur_state[0]->unionNode(cur_state[j]);
            if(cur_state[j]->state == END)  cur_state[0]->state = END;
        }
        cur_state[0]->edges.clear();
        cur_state[0]->edges.assign(cur_edges.begin(), cur_edges.end());  // 这个会不会出问题？ 参考vector的=号操作
        finalMiniDFA.graph.push_back(cur_state[0]);  // 加到最终结果
    }

    char name = 'A';
    for(int i = 0; i < state_nums; i++)
    {
        this->finalMiniDFA.graph[i]->miniName = name + i;
        if(finalMiniDFA.graph[i]->includeID(1))
            finalMiniDFA.miniStartNode = finalMiniDFA.graph[i];
        if(finalMiniDFA.graph[i]->state == END)
            finalMiniDFA.miniEndNodes.push_back(finalMiniDFA.graph[i]);

    }
}

inline string getTabTexts(int n)
{
    string texts = "";
    for(int i = 0; i < n; i++)
    {
        texts += "    ";
    }
    return texts;
}


void DFA::getCurrentDfaNodesCode(DFANode *n, vector<string> &lines, int tabsnum)
{
    lines.push_back(getTabTexts(tabsnum) + "{now in state " + n->miniName + "}");
    int sz = n->edges.size();
    if(sz > 0)
    {
        vector<DFAEdge> goNextEdges;
        vector<DFAEdge> selfEdges;
        bool firstLine = true;
        for(DFAEdge e : n->edges)
        {
            if(e.next == n)
            {
                selfEdges.push_back(e);
            }
            else
            {
                goNextEdges.push_back(e);
            }
        }
        if(selfEdges.size() > 0)
        {
            string value = "";
            value.push_back(selfEdges[0].word);
            for(int i = 1; i < selfEdges.size(); i++)
            {
                value += " or ";
                value.push_back(selfEdges[i].word);
            }
            string line1 = getTabTexts(tabsnum) + "while the next character is " + value + " do advance the input; {stay in state " + n->miniName+"}";
            string line2 = getTabTexts(tabsnum) + "end while";
            lines.push_back(line1);
            lines.push_back(line2);
            firstLine = false;
        }
        if(goNextEdges.size() > 0)
        {
            for(DFAEdge e : goNextEdges)
            {
                string line1 = getTabTexts(tabsnum) + "if the next character is " + e.word + " do advance the input;";
                string line2 = getTabTexts(tabsnum) + "end if;";
                if(!firstLine) line1 = "else " + line1;
                lines.push_back(line1);
                getCurrentDfaNodesCode(e.next,lines, tabsnum+1);
                lines.push_back(line2);
                firstLine = false;
            }
            lines.push_back(getTabTexts(tabsnum) + "else {error}");
        }

    }
    if(isEndNode(n)) lines.push_back(getTabTexts(tabsnum) + "accept;");
}

void DFA::getCCode(DFANode *n, std::vector<string> &lines, int tabsnum)
{
    int sz = n->edges.size();
    if(sz > 0)
    {
        vector<DFAEdge> goNextEdges;
        vector<DFAEdge> selfEdges;
        bool firstLine = true;
        lines.push_back(getTabTexts(tabsnum) + "char ch = getChar();");
        for(DFAEdge e : n->edges)
        {
            if(e.next == n)
            {
                selfEdges.push_back(e);
            }
            else
            {
                goNextEdges.push_back(e);
            }
        }
        if(selfEdges.size() > 0)
        {
            string value = "ch==";
            value.push_back(selfEdges[0].word);
            for(size_t i = 1; i < selfEdges.size(); i++)
            {
                value += " || ch==";
                value.push_back(selfEdges[i].word);
            }
            string line1 = getTabTexts(tabsnum) + "while( " + value + ")";
            string line2 = getTabTexts(tabsnum) + "{";
            string line3 = getTabTexts(tabsnum+1) + "Input(ch);";
            string line4 = getTabTexts(tabsnum+1) + "ch=getChar();";
            string line5 = getTabTexts(tabsnum) + "}";
            lines.push_back(line1);
            lines.push_back(line2);
            lines.push_back(line3);
            lines.push_back(line4);
            lines.push_back(line5);
        }
        if(goNextEdges.size() > 0)
        {
            for(DFAEdge e : goNextEdges)
            {
                string line1 = getTabTexts(tabsnum) + "if(ch==" + e.word + ")";
                string line2 = getTabTexts(tabsnum) + "{";
                string line3 = getTabTexts(tabsnum+1) + "Input(ch);";
                string line4 = getTabTexts(tabsnum) + "}";
                if(!firstLine) line1 = "else " + line1;
                lines.push_back(line1);
                lines.push_back(line2);
                lines.push_back(line3);
                getCCode(e.next,lines, tabsnum+1);
                lines.push_back(line4);
                firstLine = false;
            }
            lines.push_back(getTabTexts(tabsnum) + "else {cout << \"error!\";  exit(1);}");
        }

    }
    if(isEndNode(n)) lines.push_back(getTabTexts(tabsnum) + "Done();");
}


void NfaMananger::miniDfaToCodes()
{
    codes = "";
    vector<string> lines;

    DFANode *node = this->finalMiniDFA.miniStartNode;
    lines.push_back("{start in state " + node->miniName + "}");
    finalMiniDFA.getCurrentDfaNodesCode(node, lines, 0);

    for(string line : lines)
    {
        codes += line;
        codes += '\n';
        cout << line << endl;
    }

}

void NfaMananger::miniDfaToC()
{
    codes = "";
    vector<string> lines;

    DFANode *node = this->finalMiniDFA.miniStartNode;
    finalMiniDFA.getCCode(node, lines, 0);

    for(string line : lines)
    {
        codes += line;
        codes += '\n';
        cout << line << endl;
    }
}

NFA::NFA(char c, char id1, char id2)
{
    start_node = new NFANode(id1, NONE);
    end_node = new NFANode(id2, NONE);
    NFAEdge edge(start_node, end_node, c);
    start_node->out_edges.push_back(edge);
}

NFA::NFA(char c)
{
    buildCell(c);
}

void NFA::operator=(const NFA &nfa)
{
    this->start_node = nfa.start_node;
    this->end_node = nfa.end_node;
}

void NFA::buildCell(char c)
{
    start_node = new NFANode();
    end_node = new NFANode();
    NFAEdge edge(start_node, end_node, c);
    start_node->out_edges.push_back(edge);
}

void NFA::Unite(const NFA &nfa) // this | nfa
{
    /*  创建新的头尾结点，进行连接
     *  三分支的处理？交给读取程序来判断
     *  注意： 　WORD_EPSILON 表示eplision转换, ID_DEFAULT表示标号未定
     */

    NFANode *node1 = new NFANode();
    NFANode *node2 = new NFANode();

    NFAEdge edge11(node1, nfa.start_node, WORD_EPSILON);
    NFAEdge edge12(node1, this->start_node, WORD_EPSILON);
    NFAEdge edge21(nfa.end_node, node2, WORD_EPSILON);
    NFAEdge edge22(this->end_node, node2, WORD_EPSILON);

    node1->out_edges.push_back(edge11);
    node1->out_edges.push_back(edge12);
    node2->in_edges.push_back(edge21);
    node2->in_edges.push_back(edge22);

    this->start_node = node1;
    this->end_node = node2;
}

void NFA::Joint(const NFA &nfa)
{
    NFAEdge e(this->end_node, nfa.start_node, WORD_EPSILON);
    this->end_node->out_edges.push_back(e);
    nfa.start_node->in_edges.push_back(e);
    this->end_node = nfa.end_node;
}

void NFA::Star()
{
    NFANode *node1 = new NFANode();
    NFANode *node2 = new NFANode();

    NFAEdge edge11(node1, this->start_node, WORD_EPSILON);
    NFAEdge edge12(node1, node2, WORD_EPSILON);

    NFAEdge eCircle(this->end_node, this->start_node, WORD_EPSILON);
    NFAEdge tailEdge(this->end_node, node2, WORD_EPSILON);

    node1->out_edges.push_back(edge11);
    node1->out_edges.push_back(edge12);

    node2->in_edges.push_back(edge12);
    node2->in_edges.push_back(tailEdge);


    this->end_node->out_edges.push_back(tailEdge);
    this->end_node->out_edges.push_back(eCircle);

    this->start_node->in_edges.push_back(eCircle);
    this->start_node->in_edges.push_back(edge11);

    start_node = node1;
    end_node = node2;
}

bool NFA::isEmptyNFA() const
{
    return this->start_node == NULL;
}

bool NFA::isEndID(int id) const
{
    return end_node->id == id;
}

int NFA::startID() const
{
    return this->start_node->id;
}

int NFA::endID() const
{
    return this->end_node->id;
}


bool DFANode::includeID(int id)
{
    if(node.find(id) != node.end())
        return true;
    return false;
}

void DFANode::insertID(int id)
{
    node.insert(id);
}

void DFANode::insertEdge(DFAEdge edge)
{
    edges.push_back(edge);
}

void DFANode::unionNode(DFANode *n)
{

    this->node.insert(n->node.begin(), n->node.end());
    this->edges.insert(this->edges.end(),n->edges.begin(), n->edges.end());

}

DFANode *DFANode::transform(char word)
{
    for(DFAEdge edge:edges)
    {
        if(edge.word == word)
            return edge.next;
    }
    return NULL;
}

string DFANode::toString()
{
    string name = "{";
    set<int>::iterator it = node.begin();
    name += std::to_string(*it);
    it++;
    while(it != node.end())
    {
        name += ",";
        name += std::to_string(*it);
        it++;
    }
    name += "}";
    return name;
}

vector<DFANode*> DFA::getNodes(int id)
{
    vector<DFANode*> nodes;
    for(DFANode *node:graph)
    {
        if(node->includeID(id))
            nodes.push_back(node);
    }
    return nodes;
}

DFANode* DFA::crateNewNode(int id)
{
    DFANode *node = new DFANode();
    node->node.insert(id);
    graph.push_back(node);
    return node;
}

bool DFA::equals(DFANode *node1, DFANode *node2, set<char>words)  // 判断两个DFA结点的转化是否等价
{
    if(node1 == node2)  return true;
    for(char ch : words)
    {
        if(node1->transform(ch) != node2->transform(ch))
            return false;
    }
    return true;
}

bool DFA::isEndNode(DFANode *node) const
{
    for(DFANode *n : this->miniEndNodes)
    {
        if(node == n)   return true;
    }
    return false;
}

void DFA::delNode(DFANode *node)
{
    vector<DFANode *>::iterator it;
    vector<DFAEdge>::iterator jt;
    for(it = graph.begin(); it!=graph.end(); )
    {
        if(*it == node)
        {
            it = graph.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for(it = graph.begin(); it != graph.end(); ++it)
    {
        for(jt = (*it)->edges.begin(); jt != (*it)->edges.end();)
        {
            if((*jt).next == node)
            {
                jt = (*it)->edges.erase(jt);
            }
            else
            {
                ++jt;
            }
        }
    }
}

void DFA::printResult()
{
    for(DFANode *node : graph)
    {
        for(DFAEdge edge : node->edges)
        {
            cout << node->toString() << "--[" << edge.word << "]-->" << edge.next->toString() << endl;
        }
    }
}
