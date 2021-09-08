/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

#define MAX_IDENT_LEN 15
#define MAX_STRING_LEN 250
#define KEYWORDS_COUNT 28

typedef enum {
  TK_NONE, TK_IDENT, TK_EOF,
  TK_NUMBER, TK_CHAR, TK_STRING, TK_DOUBLE,
  
  KW_PROGRAM, KW_CONST, KW_TYPE, KW_VAR,
  KW_INTEGER, KW_CHAR, KW_ARRAY,
  // mo rong
  KW_STRING, KW_DOUBLE, KW_BREAK, KW_SWITCH, KW_CASE, KW_DEFAULT, KW_SUM, KW_RETURN,
  KW_OF, 
  KW_FUNCTION, KW_PROCEDURE,
  KW_BEGIN, KW_END, KW_CALL,
  KW_IF, KW_THEN, KW_ELSE,
  KW_WHILE, KW_DO, KW_FOR, KW_TO,

  SB_SEMICOLON, SB_COLON, SB_PERIOD, SB_COMMA,
  SB_ASSIGN, SB_EQ, SB_NEQ, SB_LT, SB_LE, SB_GT, SB_GE,
  SB_PLUS, SB_MINUS, SB_TIMES, SB_SLASH,
  SB_LPAR, SB_RPAR, SB_LSEL, SB_RSEL,
  // mo rong
  SB_POWER, SB_FACTORIAL
} TokenType; 

typedef struct {
  char* string; // mo rong
  int lineNo, colNo;
  TokenType tokenType;
  double value; // mo rong
} Token;

TokenType checkKeyword(char *string);
Token* makeToken(TokenType tokenType, int lineNo, int colNo);
char *tokenToString(TokenType tokenType);


#endif
