#include "scanner.h"
#include <exception>
#include <iostream>


TokenType Scanner::reservedLookup(char *s)
{
    for(int i = 0; i < MAXRESERVED; i++)
    {
        if(!strcmp(s, reservedWords[i].str))
            return reservedWords[i].tok;
    }
    return ID;
}

void Scanner::printToken(char *tstring, TokenType ttype)
{
    if(strlen(tstring) <= 0) return;
    std::cout << tstring << '\t';
    switch (ttype) {
    case IF:
    case ELSE:
    case WHILE:
    case FOR :
    case INCLUDE:
    case CIN:
    case COUT:
    case INT:
    case FLOAT:
    case DOUBLE:
    case MAIN:
    case HEADERFILE:
        std::cout<< "保留字" << std::endl;
        break;
    case ID:
        std::cout << "标识符" << std::endl;
        break;
    case INTNUM:
    case FLOATNUM:
        std::cout << "数" << std::endl;
        break;
    case STRING:
        std::cout << "串" << std::endl;
        break;
    default:
        std::cout << "特殊字符" << std::endl;
        break;
    }
}

std::string Scanner::getTokenMessage(const char *tstring, TokenType ttype)
{
    if(strlen(tstring) <= 0) return NULL;
    std::string msg(tstring);
    std::cout << "Now get " << msg << std::endl;

    msg += '\t';
    switch (ttype) {
    case IF:
    case ELSE:
    case WHILE:
    case FOR :
    case INCLUDE:
    case CIN:
    case COUT:
    case INT:
    case FLOAT:
    case DOUBLE:
    case MAIN:
    case HEADERFILE:
        msg += "保留字";
        break;
    case ID:
        msg += "标识符";
        break;
    case INTNUM:
    case FLOATNUM:
        msg += "数";
        break;
    case STRING:
        msg += "串";
        break;
    default:
        msg += "特殊字符";
        break;
    }
    return msg;
}

Scanner::Scanner(const char *fileName)
{
    FILE *fp;
    if((fp=fopen(fileName,"r")) == NULL)
    {
        std::cout << "Error when opening the source file!" << std::endl;
        exit(1);
    }
    else
    {
        this->source = fp;
        std::cout << "Sucessfully open file " << fileName << std::endl;
    }
}

int Scanner::getNextChar(bool echoSource)
{
    if(!(linePos < bufSize))
    {
        lineNo++;
        if(fgets(lineBuf,BUFLEN-1, source))
        {
            if(echoSource) std::cout << "Listening:" <<  lineNo << " - " << lineBuf << std::endl;
            bufSize = strlen(lineBuf);
            linePos = 0;
            return lineBuf[linePos++];
        }
        else
        {
            EOF_flag = true;
            return EOF;
        }
    }
    else return lineBuf[linePos++];
}

void Scanner::ungetNextChar()
{
    if(!EOF_flag) linePos--;
}

void Scanner::resetFilePointer()
{
    if(this->source == NULL)    return;
    rewind(this->source);
    linePos = 0;
    bufSize = 0;
    EOF_flag = false;
    lineNo = 0;
}

TokenType Scanner::getToken(bool traceScan)
{
    int tokenStringIndex = 0;
    TokenType currentToken;
    StateType state = START;

    bool save;
    while(state != DONE)
    {
        int c = getNextChar(false);
        // std::cout << "Scanning " << char(c) << std::endl;
        switch(state)
        {
        case START:
            if(isdigit(c))
            {
                save = true;
                state = INNUM;
            }
            else if (isalpha(c))
            {
                save = true;
                state = INID;
            }
            else if(c=='=')
            {
                save = true;
                state = INASSIGN;  //双等号?
            }
            else if(c == ' ' || c == '\t' || c == '\n')
                save = false;
            else if (c == '/')
            {
                save = false;
                state = INCOMMENT1; //检查是否为注释
            }
            else if(c == '<')
            {
                save = true;
                state = OUTOP; //检查是否为<<
            }
            else if(c == '>')
            {
                save = true;
                state = INOP; // >> ?
            }
            else if(c == '"')
            {
                save = true;
                state = INSTR;
            }
            else
            {
                state = DONE;
                save = true;
                switch (c) {
                case EOF:
                    save = false;
                    currentToken = ENDFILE;
                    break;
                case '=':
                    currentToken = EQ;
                    break;
                case '#':
                    currentToken = HASH;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case INCOMMENT1:
            save = false;
            if(c == '*')
            {
                state = INCOMMENT2;

            }
            else if(c == EOF)
            {
                state = DONE;
                currentToken = ENDFILE;
            }
            else if (c=='/') // 遇到单行注释//
            {
                state = START;
            }
            else
            {
                ungetNextChar();
                save=true;
                currentToken = OVER;
                state = DONE;
            }
            break;
        case INCOMMENT11:
            save = false;
            if(c == '\n')
            {
                state = START; //结束单行注释
            }
            else if(c == EOF)
            {
                state = DONE;
                currentToken = ENDFILE;
            }
            break;
        case INCOMMENT2:
            save = false;
            if(c == '*')
            {
                state = INCOMMENT3;
            }
            else if(c == EOF)
            {
                state = DONE;
                currentToken = ENDFILE;
            }
            // else still INCOMMENT
            break;

        case INCOMMENT3:
            save = false;
            if(c == '/')
            {
                state = START;  //结束注释，回到START
            }
            else
            {
                state = INCOMMENT2;
            }
            break;

        case INOP:
            if(c == '>')
            {
                save = true;
                state = DONE;
                currentToken = INPAREN;
            }
            else
            {
                save = false;
                ungetNextChar();
                currentToken = GT;
                state = DONE;
            }
            break;

        case OUTOP:
            if(c == '<')
            {
                save = true;
                state = DONE;
                currentToken = OUTPAREN;
            }
            else
            {
                save = false;
                ungetNextChar();
                currentToken = LT;
                state = DONE;
            }
            break;

        case INASSIGN:
            state = DONE;
            if(c == '=')
                currentToken = EQ;
            else
            {
                ungetNextChar();
                save = false;
                currentToken = ASSIGN;
            }
            break;

        case INNUM:
            if(c == '.')
            {
                state = INFLOATNUM;
            }
            else if(!isdigit(c))
            {
                ungetNextChar();
                save = false;
                state = DONE;
                currentToken = INTNUM;
            }
            break;

        case INFLOATNUM:
            if(!isdigit(c))
            {
                ungetNextChar();
                state = DONE;
                save = false;
                currentToken = FLOATNUM;
            }
            break;

        case INSTR:
            save = true;
            if(c == '"')
            {
                state = DONE;
                currentToken = STRING;
            }
            break;

        case INID:
            if(!isalpha(c) && !isdigit(c) && c != '_' && c!= '.')
            {
                ungetNextChar();
                save = false;
                state = DONE;
                currentToken = ID;
            }
            break;

        case DONE:
            break;

        default:
            std::cout << "Scanner Bug: state=" << state << std::endl;
            state = DONE;
            currentToken = ERROR;
            break;
        }
        if((save) && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = (char) c;
        if(state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if(currentToken == ID)
                currentToken = reservedLookup(tokenString);
            break;
        }

    }
    if(traceScan)
    {
        printToken(tokenString,currentToken);
    }
    return currentToken;
}

const char* Scanner::getTokenString() const
{
    return tokenString;
}

bool noNeedBlank(TokenType token)
{
    return token >= HASH;

}
