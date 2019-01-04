#ifndef LL1NODE_H
#define LL1NODE_H
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <iostream>
using namespace std;


class LL1Node
{
public:
    string left;
    vector<vector<string>> right;
    LL1Node(const string &str)
    {
        left = str;
        right.clear();
    }

    void print()
    {
        cout << left << "==>";
        for(size_t i = 0; i < right[0].size(); i++)
        {
            cout << ' ' << right[0][i];
        }

        for(size_t i = 1; i < right.size(); i++)
        {
            cout << " |";
            for(size_t j = 0; j < right[i].size(); j++)
            {
                cout << ' ' << right[i][j];
            }
        }
        cout << endl;
    }

    void insert(const vector<string> &nodes)
    {
        right.push_back(nodes);
    }

    bool inclueEpsilon()
    {
        for(size_t i = 0; i < right.size(); i++)
        {
            if(right[i].size() == 1 && right[i][0] == "#")
                return true;
        }
        return false;
    }
    string toString() const
    {
        string msg = this->left + "-->";
        for(string str: this->right[0])
        {
            msg += str;
            msg += " ";
        }
        for(auto it = right.begin()+1; it != right.end(); it++)
        {
            msg += "|";
            for(auto it1 = it->begin(); it1 != it->end(); it1++)
            {
                msg += *it1;
                msg += " ";
            }
        }
        return msg;
    }

};

#endif // LL1NODE_H
