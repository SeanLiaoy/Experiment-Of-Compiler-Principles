#ifndef SCANNER_H
#define SCANNER_H

#include "globals.h"
#include <cstdio>
#include <string>

static struct{
    const char *str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {
        {"if",IF},
        {"else",ELSE},
        {"while",WHILE},
        {"for",FOR},
        {"cin",CIN},
        {"cout",COUT},
        {"include",INCLUDE},
        {"float",FLOAT},
        {"double",DOUBLE},
        {"int", INT},
        {"main", MAIN},
        {"iostream.h",HEADERFILE},
        {"return", RETURN}
};

class Scanner
{
private:
    FILE *source = NULL;   //Source File Name
    char lineBuf[BUFLEN];  // Holds the current line
    int linePos = 0;  //current position of the LineBuf
    int bufSize = 0;
    int EOF_flag = false;
    int lineNo = 0;
    char tokenString[MAXTOKENLEN];

    enum StateType {START, INASSIGN, INCOMMENT1,
                    INCOMMENT11,INCOMMENT2, INCOMMENT3,
                    INNUM, INFLOATNUM, INID, INSTR,
                    DONE, INOP, OUTOP};

    static TokenType reservedLookup(char *s);
    static void printToken(char *tstring, TokenType ttype);


public:
    Scanner(const char *fileName);

    int getNextChar(bool echoSource = false);
    void ungetNextChar();
    void resetFilePointer();
    TokenType getToken(bool traceScan = false);
    const char* getTokenString() const;

    static std::string getTokenMessage(const char *tstring, TokenType ttype);
};

bool noNeedBlank(TokenType token);

#endif // SCANNER_H
