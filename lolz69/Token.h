#pragma once

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE* yyin, * yyout;


typedef enum eTOKENS
{
	ID,
	int_num,
	float_num,
	ar_op_mul,
	ar_op_add,
	assignment_op,
	rel_op_smaller,
	rel_op_smallerOrEqual,
	rel_op_equal,
	rel_op_biggerOrEqual,
	rel_op_bigger,
	rel_op_notEqual,
	INT_KW,
	FLOAT_KW,
	VOID_KW,
	IF_KW,
	RETURN_KW,
	comma,
	colon,
	semicolon,
	opening_parentheses,
	closing_parentheses,
	opening_brackets,
	closing_brackets,
	opening_curlybraces,
	closing_curlybraces,
	EOF_TOKEN,
	Unrecognized_token

	/*
		Examples of tokens, add/change according to your needs.

		Alternative way to define tokens:
		#define TOKEN_INTEGER 1
		#define TOKEN_IF 2
		...........
	*/
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char* lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token* tokensArray;
	struct Node* prev;
	struct Node* next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token* next_token();
Token* back_token();
const char* getTokenName(enum eTOKENS token);

#endif