#ifndef GLOBALS_H
#define GLOBALS_H

#define MAXRESERVED 13
#define BUFLEN 256
#define MAXTOKENLEN 500

typedef enum
{
    //程序标识符
    BEGINFILE, ENDFILE, ERROR,

    //头文件
    HEADERFILE,

    // 关键字
    IF, ELSE, WHILE, FOR, INCLUDE, CIN, COUT, INT, FLOAT, DOUBLE, MAIN, RETURN,


    // 多字符标识
    ID, INTNUM, FLOATNUM, STRING,

    // 特殊符号
    /* HASH : #
     * LPREN: (
     * RPREN: )
     * PLUS: +
     * MNIUS: -
     * EQ: ==
     * TIMES: *
     * SEMI: ;
     * GT: >
     * LT: <
     * INPAREN: >>
     * OUTPAREN: <<
     * LBRACES: {
     * RBRACES: }
     * LBRACKET: [
     * RBRACKET: ]
     * OVER: /
     */
    HASH,ASSIGN,EQ, GT, LT, INPAREN, OUTPAREN,PLUS,MINUS,TIMES,LPAREN, RPAREN, SEMI,
    LBRACES,RBRACES,LBRACKET,RBRACKET, OVER
} TokenType;


typedef struct
{
    TokenType tokenval;
    char *stringval;
    int numval;
} TokenRecord;


#endif // GLOBALS_H
