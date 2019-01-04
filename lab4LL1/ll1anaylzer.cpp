#include "ll1analyzer.h"
#include <fstream>
#include <map>
#include <stack>

string& trim(string &s)
{
    if(s.empty())
        return s;
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

vector<string> splitString(string str, const string &spliter)
{
    string::size_type pos1,pos2;
    pos1 = str.find(spliter);
    pos2 = 0;
    vector<string> results;
    while(pos1 != string::npos)
    {
        string res = str.substr(pos2, pos1-pos2);
        if(res != " ")
            results.push_back(trim(res));
        pos2 = pos1 + spliter.size();
        pos1 = str.find(spliter, pos2);
    }
    if(pos2 != str.length())
    {
        string res = str.substr(pos2);
        if(res != " ")
            results.push_back(trim(res));
    }
    return results;
}



Analyzer::Analyzer(string file_path)
{
    log.open(this->logFileName.c_str());
    if(!log.is_open())
    {
        cerr << "打开日志文件失败！";
        exit(1);
    }
    ifstream input(file_path.c_str());
    string line;
    if(input.is_open() == false)
    {
        cout << "Open file error!" << endl;
        exit(1);
    }
    while(getline(input, line))
    {
        if(line.size() <= 3) continue;
        log << "Scanning...>>>>\t" << line << '\n';
        size_t splitIndex = line.find("-->");
        if(splitIndex == string::npos)
        {
            cerr << "Error when input the LL1 grammer";
            exit(1);
        }

        string startNode = line.substr(0, splitIndex);
        string toNodeStr = line.substr(splitIndex+3);
        vector<string> toNodes = splitString(toNodeStr, "|");

        LL1Node node(startNode);

        for(string str:toNodes)
        {
            vector<string> nodes = splitString(str, " ");
            node.insert(nodes);
        }

        grammers.push_back(node);
    }
    input.close();
    startWordIndex = -1;
    initWords();
}

void Analyzer::printGrammers()
{
    for(size_t i = 0; i < grammers.size(); i++)
    {
        log <<  i+1 << " :::>>> ";
        const string &left = grammers[i].left;
        const vector<vector<string>> &right = grammers[i].right;

        log << left << "==>";
        for(size_t i = 0; i < right[0].size(); i++)
        {
            log << ' ' << right[0][i];
        }

        for(size_t i = 1; i < right.size(); i++)
        {
            log << " |";
            for(size_t j = 0; j < right[i].size(); j++)
            {
                log << ' ' << right[i][j];
            }
        }
        log << '\n';
    }
    log << "非终结符号如下:" << endl;
    for(string word : nonFinalWord)
    {
        log << word << ' ';
    }
    log << '\n';
    log << "终结符号如下:" << '\n';
    for(string word: finalWord)
    {
        log << word << ' ';
    }
    log << '\n';
    log << "非终结符号个数:" << nonFinalWord.size() << '\n';
    log << "终结符号个数:" << finalWord.size() << '\n';
}

int Analyzer::getNonFinalWordIndex(string word)
{
    for(size_t i = 0; i < grammers.size(); i++)
    {
        if(word == grammers[i].left)
            return i;
    }
    return -1;
}

void Analyzer::initWords()
{
    map<string,int> wordMap;
    this->nonFinalWord.clear();
    this->finalWord.clear();
    for(LL1Node node : grammers)
    {
        wordMap[node.left] = 1;
        this->nonFinalWord.push_back(node.left);
    }
    for(LL1Node node : grammers)
    {
        for(vector<string> words : node.right)
        {
            for(string word : words)
            {
                if(wordMap.count(word) == 0)
                {
                    this->finalWord.push_back(word);
                    wordMap[word] = 1;
                }
            }
        }
    }
}

void Analyzer::dealGrammers()
{
    int sizeBefore = grammers.size();
    for(size_t i = 0; i < grammers.size(); i++)
    {
        for(size_t j = 0; j < i; j++)
        {
            /*
             * 例子：S -> Qc|c
             *      Q -> R  b|b
             *      R -> Sa|a
             *  设 i : Q -> Rb|b
             *     j : R -> Sa|a
             * 
             */

            vector< vector<string> > cont;
            vector< vector<string> > &right1 = grammers[i].right;
            vector< vector<string> > &right2 = grammers[j].right;
            string check = grammers[j].left;
            vector<vector<string>>::iterator it1 = right1.begin();
            vector<vector<string>>::iterator it2 = right2.begin();
            for(; it1 != right1.end(); it1++)
            {
                if(it1->at(0) == check)  // i的右边第一个元素与j的非终结符号相同，代入
                {
                    // 将 j 代入 i
                    // 例如 此时 *it1 = {R,b}
                    //       而  right2 = {{S,a},{a}}
                    //   代入后  为 {S,a,b}、{a,b}
                    vector<string> other;
                    for(size_t k = 1; k < (*it1).size(); k++)   // 将i中R之后的符号代入
                    {
                        other.push_back((*it1)[k]);
                    }
                    // 此时 other = {b}

                    for(; it2 != right2.end(); it2++)
                    {
                        vector<string> after = *it2;
                        after.insert(after.end(),other.begin(),other.end());
                        cont.push_back(after);  // {S, a, b} 、 {a, b}
                    }
                }
            }
            
            size_t nn = right1.size();
            while(nn--)
            {
                if(right1.begin()->at(0) == check)
                {
                    right1.erase(right1.begin());
                }
                else
                {
                    cont.push_back(*right1.begin());
                    right1.erase(right1.begin());
                }
            }
            for( size_t i = 0; i < cont.size(); i++)
            {
                right1.push_back(cont[i]);
            }
        }
    }

    this->printGrammers();
    int sizeAfter = grammers.size();
    if(sizeAfter > sizeBefore)
        startWordIndex = grammers.size()-1;
    else
        startWordIndex = 0;

    // 下面消除直接左递归
    for(size_t i = 0; i < grammers.size(); i++)
    {
        string check = grammers[i].left;
        vector<vector<string>> &temp = grammers[i].right;
        vector<vector<string>>::iterator it = temp.begin();
        string tt = check + "'";
        bool flag = true;
        for(; it != temp.end(); it++)
        {
            if(it->at(0) == check)  // 左 = 右边第一个，发现直接左递归
            {
                grammers.push_back(LL1Node(tt));
                flag = false;
                break;
            }
        }
        if(flag)    continue;
        vector<vector<string>> cont;
        vector<vector<string>> &ss = grammers[grammers.size()-1].right;
        vector<string> eplision;
        eplision.push_back("#");
        ss.push_back(eplision);
        while(!temp.empty())
        {
            if(temp.begin()->at(0) == check)  // 发现左递归元素
            {
                vector<string> vt;
                vt.assign(temp.begin()->begin()+1, temp.begin()->end());
                vt.push_back(tt);
                ss.push_back(vt);
            }
            else  // 否则，应该修改原语法
            {
                vector<string> vt;
                vt.assign(temp.begin()->begin(),temp.begin()->end());
                vt.push_back(tt);
                cont.push_back(vt);
            }
            temp.erase(temp.begin());
        }
        for(size_t i = 0; i < cont.size(); i++)
        {
            temp.push_back(cont[i]);
        }
    }

    initWords();  // 加入了新的结点，要重新init
}

void Analyzer::dealLeftCommonGrammers()
{
    for(size_t i = 0; i < grammers.size(); i++)
    {
        vector<LL1Node> afterDeal = dealCommonGrammer(grammers[i]);
        if(afterDeal.empty())   continue;
        grammers.erase(grammers.begin() + i);
        grammers.insert(grammers.end(), afterDeal.begin(), afterDeal.end());
    }
    initWords();
}

void Analyzer::simplify()
{
    // 设置开始结点，然后进行遍历，把遍历不到的结点统统删除
    if(startWordIndex == -1)
    {
        log << "尚未进行正常去除左递归" << '\n';
        exit(-1);
    }
    memset(used, 0, sizeof(used));
    int x = startWordIndex;
    dfs(x);
    vector<LL1Node> res;
    for(size_t i = 0; i < grammers.size(); i++)
    {
        if(used[i])
            res.push_back(grammers[i]);
    }
    grammers.clear();
    grammers = res;
    initWords();
}

void Analyzer::dfsFindFirst(int i)
{
    if(used[i])     return;
    used[i] = true;
    string &left = grammers[i].left;
    vector< vector<string> >  &right = grammers[i].right;
    vector<vector<string>>::iterator it = right.begin();
    for(; it!= right.end(); it++)
    {
        for(size_t j = 0; j < it->size(); j++)
        {
            string word = it->at(j);
            if(isFinalWord(word) == false)  // 非终结符号，则应该把 word的first集合加入到it中
            {
                // 走你
                // dfs(?);
                int y = getNonFinalWordIndex(word);
                string &tleft = grammers[y].left;
                dfsFindFirst(y);   // 先取得y的first集合
                set<string> &temp = first[tleft];
                set<string>::iterator it1 = temp.begin();
                bool flag = true;
                for(; it1!= temp.end(); it1++)
                {
                    if(*it1 == "#")   // 存在eplision
                    {
                        flag = false;
                    }
                    else first[left].insert(*it1);
                }
                if(flag)
                {
                    break;
                }
                else
                {
                    /*  这一步主要是考虑如下文法规则
                     * 1 :::>>> A==> B C c | g D B
                     * 2 :::>>> B==> b C D E | #
                     * 3 :::>>> C==> D a B | c a
                     * 4 :::>>> D==> d D | #
                     * 5 :::>>> E==> g A f | c
                     *
                     * First(B) = {b, #}  包含epsilon
                     * First(A) = {a, b, c, d, g} 不能包含epsilon
                     */
                    if( j == it->size() - 1)    first[left].insert("#");
                }

                // 如果该非终结符号的first集合不存在eplision，则应该跳出当前段
                // 如果存在，则不能break,要继续将下一个终结符号加入到当前的first集合
            }
            else
            {
                first[left].insert(it->at(j));
                break;
            }
        }
    }
}


void Analyzer::makeFirst()
{
    memset(used, 0, sizeof(used));
    for(size_t i = 0; i < grammers.size(); i++)
        dfsFindFirst(i);  //

    map<string, set<string>>::iterator it = first.begin();
    for(; it!=first.end(); it++)
    {
        log << "FIRST(" << it->first << ")={";
        set<string> &temp = it->second;
        set<string>::iterator it1 = temp.begin();
        bool flag = false;
        while(it1!= temp.end())
        {
            if(flag)    log << ",";
            log << *it1;
            flag = true;
            it1++;
        }
        log << "}" << '\n';

    }
}

void Analyzer::makeFollow()
{
    follow[grammers[startWordIndex].left].insert("$");
    while(true)
    {
        bool go = false;
        for(size_t i = 0; i < grammers.size(); i++)
        {
            string &left = grammers[i].left;
            vector<vector<string>> &right = grammers[i].right;
            vector<vector<string>>::iterator it = right.begin();
            while(it != right.end())
            {

                bool flag = true;  // 是否可能为该段最后一个符号
                vector<string> &words = *it;
                for(int j = it->size()-1; j >= 0; j--)
                {
                    string word = words[j];
                    if(isFinalWord(words[j]) == false)  // 如果当前符号是非终结符号，则应把当前符号的follow集合加入到前一个
                    {
                        // A -> aBc
                        // 如果B可能是最后一个符号(例如c是非终结符且c.includeEpsilon),则应该把follow(A) 加到 follow(B) 中
                        // 否则，应该把first(c)-{epsilon} 加到 follow(B)中
                        int x = getNonFinalWordIndex(word);
                        if(flag)  // 当前符号j 可能是最后一个
                        {
                            size_t sizeBefore = follow[word].size();
                            append(left, it->at(j));    // B可能是最后一个, 把 A的follow集合加入到B中
                            if(grammers[x].inclueEpsilon() == false)
                                flag = false;
                            size_t sizeAfter = follow[word].size();
                            if(sizeAfter > sizeBefore)    go = true;   // 还在变化，要继续
                        }
                        for(size_t k = j + 1; k < it->size(); k++)
                        {
                            string nextWord = words[k];
                            if(isFinalWord(nextWord) == false)
                            {
                                set<string> &from = first[nextWord];
                                set<string> &to = follow[word];
                                set<string>::iterator it1 = from.begin();
                                int sizeBefore = follow[word].size();
                                for(; it1 != from.end(); it1++)
                                {
                                    if(*it1 != "#")
                                        to.insert(*it1);
                                }
                                int sizeAfter = follow[word].size();
                                if(sizeAfter > sizeBefore)  go = true;
                                if(grammers[getNonFinalWordIndex(nextWord)].inclueEpsilon() == false)
                                    break;
                            }
                            else
                            {
                                int sizeBefore = follow[word].size();
                                follow[word].insert(nextWord);
                                int sizeAfter = follow[word].size();
                                if(sizeAfter > sizeBefore) go = true;
                                break;
                            }
                        }

                    }
                    else
                    {
                        flag = false;
                    }
                }

                it++;
            }
        }
        if(go == false)   // 没有变化，跳出循环
            break;
    }
    map<string, set<string>>::iterator it = follow.begin();
    for(; it != follow.end(); it++)
    {
        log << "FOLLOW(" << it->first << ")={";
        set<string> &temp = it->second;
        bool flag = false;
        for(set<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
        {
            if(flag) log << ",";
            log << *it1;
            flag = true;
        }
        log << "}" << endl;
    }
}

void Analyzer::makeTable()
{
    map<string, vector<string>> temp;
    vector<string> &letter = this->letter;
    map<string, bool> vis;
    for(size_t i = 0; i < grammers.size(); i++)
    {
        vector<vector<string>> &right = grammers[i].right;
        vector<vector<string>>::iterator it = right.begin();
        while(it != right.end())
        {
            for(int j = 0; j < it->size(); j++)
            {
                if(isFinalWord(it->at(j)) == false)     continue;
                if(vis.count(it->at(j)))    continue;
                vis[it->at(j)] = true;
                letter.push_back(it->at(j));
            }
            it++;
        }
    }
    letter.push_back("$");

    for(size_t i = 0; i < grammers.size(); i++)
    {
        temp.clear();
        string &left = grammers[i].left;
        vector<vector<string>> &right = grammers[i].right;
        for(vector<vector<string>>::iterator it = right.begin(); it != right.end(); it++)
        {
            for(size_t j = 0; j < letter.size(); j++)
            {
                if(checkFirst(*it, letter[j]))
                {
                    temp[letter[j]] = *it;
                }
                if(letter[j]=="$" && it->at(0) == "#" && checkFollow(left, letter[j]) )
                {
                    temp[letter[j]] = *it;
                }
            }
        }
        table.push_back(temp);
    }
}

void Analyzer::dfs(int i)
{
    if(used[i])     return;
    used[i] = true;
    vector< vector<string> >  &right = grammers[i].right;
    vector<vector<string>>::iterator it = right.begin();
    for(; it!= right.end(); it++)
    {
        for(size_t j = 0; j < it->size(); j++)
        {
            string word = it->at(j);
            if(isFinalWord(word) == false)  // 非终结符号，则应该把 word的first集合加入到it中
            {
                // 走你
                // dfs(?);
                int y = getNonFinalWordIndex(word);
                dfs(y);   // 先取得y的first集合
            }
        }
    }
}

// abc | ade| ede | acf
vector<LL1Node> Analyzer::dealCommonGrammer(LL1Node &node)
{
    vector<LL1Node> afterDeal;
    for(size_t i = 0; i < node.right.size(); i++)
    {
        string iFrist = node.right[i][0];
        vector< vector<string> > temp;
        vector< vector<string> > noCommonTemp;
        bool noCommon = true;
        for(size_t j = i+1; j < node.right.size(); j++)
        {
            string jFirst = node.right[j][0];
            if(iFrist == jFirst)
            {
                noCommon = false;
                vector<string> noCommonLefts;
                if(node.right[j].begin() + 1 == node.right[j].end())    continue;
                noCommonLefts.assign(node.right[j].begin()+1, node.right[j].end()); // 保存提取左公因子之后剩余的部分
                temp.push_back(noCommonLefts);
            }
            else
            {
                noCommonTemp.push_back(node.right[j]);
            }
        }
        if(noCommon == false)
        {
            vector<string> noCommonLefts;
            noCommonLefts.assign(node.right[i].begin()+1, node.right[i].end()); // 保存提取左公因子之后剩余的部分
            temp.push_back(noCommonLefts);

            string newNodeName = node.left + "'";  // 生成新结点
            LL1Node newNode(newNodeName);
            newNode.right = temp;


            vector<string> link;
            link.push_back(iFrist);
            link.push_back(newNodeName);
            noCommonTemp.insert(noCommonTemp.begin(), link);

            LL1Node oriNode(node.left);
            oriNode.right = noCommonTemp;

            afterDeal.push_back(oriNode);
            afterDeal.push_back(newNode);
            break;
        }
    }
    return afterDeal;
}

void Analyzer::append(const string &str1, const string &str2)
{
    set<string> &from = follow[str1];
    set<string> &to = follow[str2];
    set<string>::iterator it = from.begin();
    for( ; it!=from.end(); it++)
    {
        to.insert(*it);
    }
}

bool Analyzer::isFinalWord(string word)
{
    return find(finalWord.begin(),finalWord.end(), word) != finalWord.end();
}

bool Analyzer::isWord(string word)
{
    return find(finalWord.begin(),finalWord.end(), word) != finalWord.end() ||
            find(nonFinalWord.begin(),nonFinalWord.end(), word) != nonFinalWord.end();
}

bool Analyzer::checkFirst(const vector<string> &text, const string &str)
{
    for(size_t i = 0; i < text.size(); i++)
    {
        bool hasEmpty = false;
        if(isFinalWord(text[i]))
        {
            if(text[i] != str)  return false;
            else return true;
        }
        else
        {
            string temp = text[i];
            set<string> &dic = first[temp];
            for(set<string>::iterator it = dic.begin(); it!=dic.end(); it++)
            {
                if(*it == "$")  hasEmpty = true;
                if(*it == str)  return true;
            }
            if(!hasEmpty) break;
        }
    }
    return false;
}

bool Analyzer::checkFollow(const string &text, const string &str)
{
    set<string> &dic = follow[text];
    return find(dic.begin(), dic.end(), str) != dic.end();
}


void Analyzer::printStep(int step, stack<string> stk, vector<string> src, string wf, int x)
{
    cout << step << '\t';

    analyzerLog1.push_back(to_string(step));
    string out="";
    while(stk.empty() == false)
    {
        out = stk.top()+ " " + out;
        stk.pop();
    }
    cout << out << '\t';
    analyzerLog2.push_back(out);
    out = "";
    for(int  i = x; i < src.size(); i++)
    {
        out += " "+ src[i];
    }
    analyzerLog3.push_back(out);
    cout << out << '\t';
    analyzerLog4.push_back(wf);
    cout << wf << endl;
}


void Analyzer::analyzer(vector<string> &text)
{
    text.push_back("$");
    stack<string> stk;
    stk.push("$");
    string startStr = grammers[startWordIndex].left;
    stk.push(startStr);  // 开始
    int steps = 1;
    int idx = 0;
    while(!stk.empty())
    {
        string u = stk.top();
        vector<string> tmp;
        stk.pop();
        string msg = "match";

        if(isFinalWord(u) || u == "$")
        {
            if(idx == text.size()-1 && u == "#");
            else if(text[idx] == u)  idx++;
        }
        else
        {
            int x = getNonFinalWordIndex(u);
            string word = text[idx];
            tmp.insert(tmp.end(),table[x][word].begin(),table[x][word].end());

            for(int i = tmp.size()-1; i>=0; i--)
            {
                stk.push(tmp[i]);
            }
            msg = "";
            msg += u+"->";
            for(string str : tmp)
            {
                msg += " "+ str;
            }
        }
        printStep(steps++, stk, text, msg, idx);
//        if(idx == text.size()) break;
    }
}

void Analyzer::analyzer(string &sentence)
{
    vector<string> text = splitSentence(sentence);
    for(string str:text)
    {
        cout << str << endl;
    }
    analyzer(text);
}

vector<string> Analyzer::splitSentence(string &sentence)
{
    vector<string> vt;
    int beginIndex = 0;
    for(int end = 0; end < sentence.length(); end++)
    {
        for(int j = beginIndex; j <= end; j++)
        {
            string w = sentence.substr(j, end-j+1);
            if(isWord(w))
            {
                vt.push_back(w);
                beginIndex = end+1;
            }
        }
    }
    return vt;
}
