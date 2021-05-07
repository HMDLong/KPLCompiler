/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/*************************************************/

void skipBlank() {
  // TODO
  while(currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

void skipComment() {
  // TODO
  int state = 0;
  while(currentChar != EOF && state<2){
    switch(charCodes[currentChar]){
      case CHAR_TIMES:
        state = 1;
        break;
      case CHAR_RPAR:
        if(state == 1) state = 2;
        else state = 0;
        break;
      default:
        state = 0;
        break;
    }
    readChar();
  }
  if(state != 2){
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }
}

Token* readIdentKeyword(void) {
  // TODO
  Token* token;
  token = makeToken(TK_NONE, lineNo, colNo);
  int index = 0;
  char* buff = (char*)malloc(MAX_IDENT_LEN+1);

  while(currentChar != EOF && 
        (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT || currentChar == 95)){
    if(index >= MAX_IDENT_LEN){
      while(currentChar != EOF && 
            (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT || currentChar == 95)){
              readChar();
            }
      error(ERR_IDENTTOOLONG, lineNo, colNo);
      return token;
    }
    buff[index++] = currentChar;
    readChar();
  }
  buff[index] = '\0';
  token->tokenType = checkKeyword(buff) == TK_NONE ? TK_IDENT : checkKeyword(buff);
  strcpy(token->string, buff);
  free(buff);
  return token;
}

Token* readNumber(void) {
  // TODO
  Token* token;
  token = makeToken(TK_NONE, lineNo, colNo);
  char* str = (char*)malloc(MAX_IDENT_LEN+1);
  int index = 0;
  int isFloat = 0;
  while(currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD)){
    if(index >= MAX_IDENT_LEN)
      error(ERR_FLOATTOOLONG, token->lineNo, token->colNo);
    if(charCodes[currentChar] == CHAR_PERIOD) {
      if(isFloat > 1) error(ERR_INVALIDFLOAT, token->lineNo, token->colNo);
      isFloat++;
    }
    str[index++] = currentChar;
    readChar();
  }
  str[index] = '\0';
  strcpy(token->string, str);
  if(isFloat == 0){
    token->value = atoi(token->string);
    token->tokenType = TK_NUMBER;
    return token;
  }
  token->tokenType = TK_FLOAT;
  return token;
}

Token* readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
    
  token->string[0] = currentChar;
  token->string[1] = '\0';

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}

Token* readString(void){
  Token* token = makeToken(TK_NONE, lineNo, colNo);
  char* str = (char*)malloc(MAX_IDENT_LEN+1);
  int index = 0;
  readChar();
  while(currentChar != EOF && charCodes[currentChar] != CHAR_DOUBLEQUOTE){
    if(index >= MAX_IDENT_LEN){
      while(currentChar != EOF && charCodes[currentChar] != CHAR_DOUBLEQUOTE)
        readChar();
      error(ERR_INVALIDSTRING, lineNo, colNo);
      return token;
    }
    str[index++] = currentChar;
    readChar();
  }
  str[index] = '\0';
  token->tokenType = TK_STRING;
  strcpy(token->string, str);
  free(str);
  readChar();
  return token;
}

Token* readPointFloat(){
  Token* token = makeToken(TK_NONE, lineNo, colNo-1);
  char* str = (char*)malloc(MAX_IDENT_LEN+1);
  str[0] = '0';
  str[1] = '.';
  str[2] = currentChar;
  int index = 3;
  readChar();
  while(currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD)){
    if(index >= MAX_IDENT_LEN){
      while(currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD))
        readChar();
      error(ERR_FLOATTOOLONG, lineNo, colNo);
      return token;
    }
    str[index++] = currentChar;
    readChar();
  }
  str[index] = '\0';
  strcpy(token->string, str);
  token->tokenType = TK_FLOAT;
  free(str);
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_MINUS: 
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_LT:
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) token = makeToken(SB_LE, lineNo, colNo-1);
    else token = makeToken(SB_LT, lineNo, colNo-1);
    readChar();
    return token;
  case CHAR_GT:
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) token = makeToken(SB_GE, lineNo, colNo-1);
    else token = makeToken(SB_GT, lineNo, colNo-1);
    return token;
  case CHAR_EXCLAIMATION:
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token = makeToken(SB_NEQ, lineNo, colNo-1);
      readChar();
    }
    else error(ERR_INVALIDSYMBOL, lineNo, colNo-1);
    return token;
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA: 
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERIOD:
    readChar();
    if(charCodes[currentChar] == CHAR_RPAR) {
      token = makeToken(SB_RSEL, lineNo, colNo-1);
      readChar();
    }
    else {
      if(charCodes[currentChar] == CHAR_DIGIT){
        token = readPointFloat();
      }
      else token = makeToken(SB_PERIOD, lineNo, colNo-1);
    }
    return token;
  case CHAR_COLON:
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token = makeToken(SB_ASSIGN, lineNo, colNo-1);
      readChar();
    }
    else token = makeToken(SB_COLON, lineNo, colNo-1);
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  case CHAR_DOUBLEQUOTE:
    return readString();
  case CHAR_LPAR:
    readChar();
    if(charCodes[currentChar] == CHAR_PERIOD){
      token = makeToken(SB_LSEL, lineNo, colNo-1);
      readChar();
    }
    else{
      if(charCodes[currentChar] == CHAR_TIMES) { 
        skipComment(); 
        return getToken();
      }
      else token = makeToken(SB_LPAR, lineNo, colNo-1);
    }
    return token;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_PERCENT:
    token = makeToken(SB_PERCENT, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}

Token* getValidToken(void) {
  Token *token = getToken();
  while (token->tokenType == TK_NONE) {
    free(token);
    token = getToken();
  }
  return token;
}

/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;

  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_STRING: printf("TK_STRING(\'%s\')\n", token->string); break;
  case TK_FLOAT: printf("TK_FLOAT(%s)\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;

  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_FLOAT: printf("KW_FLOAT\n"); break;
  case KW_STRING: printf("KW_STRING\n"); break;

  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;

  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;

  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_PERCENT: printf("SB_PERCENT\n"); break;

  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}
