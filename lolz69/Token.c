#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/


Node* currentNode = NULL;
int currentIndex = 0;


#define TOKEN_ARRAY_SIZE 1000



/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{
	int length = strlen(lexeme) + 1;

	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray =
			(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray =
				(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}
	//printf(yyout, "Line % d : found token of type assignment_op, lexeme: %s\n", numOfLine, lexeme);
	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char) * length);
#ifdef _WIN32
	strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
#else
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
#endif	



}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token* back_token() {
	if (currentNode == NULL) {
		return NULL;
	}

	if (currentNode == 0) {
		if (currentNode->prev == NULL) {
			currentIndex--;
			return NULL;
		}
		currentNode = currentNode->prev;
		currentIndex = TOKEN_ARRAY_SIZE - 1;
	}
	else
		currentIndex--;
	return &(currentNode->tokensArray[currentIndex]);
}



/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token):
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token* next_token()
{
	if (currentNode == NULL) {
		return NULL;
	}

	if (currentIndex == TOKEN_ARRAY_SIZE - 1) {
		if (currentNode->next == NULL) {
			return NULL;
		}
		currentNode = currentNode->next;
		currentIndex = 0;
	}
	else
		currentIndex++;
	return &currentNode->tokensArray[currentIndex];
}

void backToFirst() {
	while (currentNode->prev != NULL)currentNode = currentNode->prev;
	currentIndex = -1;
}


const char* getTokenName(enum eTOKENS token) {
	switch (token)
	{

	case	ID: return "ID";
	case	int_num:return "int_num";
	case	float_num: return "float_num";
	case	ar_op_mul: return "ar_op_mul";
	case	ar_op_add: return "ar_op_add";
	case	assignment_op: return "assigntment_op";
	case	rel_op_smaller: return "rel_op_smaller";
	case	rel_op_smallerOrEqual: return "rel_op_smallerOrEqual";
	case	rel_op_equal: return "rel_op_equal";
	case	rel_op_biggerOrEqual: return "rel_op_biggerOrEqual";
	case	rel_op_bigger: return "rel_op_bigger";
	case	rel_op_notEqual: return "rel_op_notEqual";
	case	INT_KW: return "INT_KW";
	case	FLOAT_KW: return "FLOAT_KW";
	case	VOID_KW: return "VOID_KW";
	case	IF_KW: return "IF_KW";
	case	RETURN_KW: return "RETURN_KW";
	case	comma: return "comma";
	case	colon: return "colon";
	case	semicolon: return "semicolon";
	case	opening_parentheses: return "opening_partheses";
	case	closing_parentheses: return "closing_parentheses";
	case	opening_brackets: return "opening_brackets";
	case	closing_brackets: return "closing_brackets";
	case	opening_curlybraces: return "opening_curlybraces";
	case	closing_curlybraces: return "closing_curlybraces";
	case	EOF_TOKEN: return "EOF_TOKEN";
	case	Unrecognized_token: return "Unrecognized_token";
	default: return "Not found";
	}
}

void emptyCreateAndStore()
{
	currentNode = NULL;
	currentIndex = 0;
}
