%option noyywrap
%{
#include "Token.h"
int line=1;
%}

WhitespaceTab			[ \t]+
EXPONENT        		[eE]
NON_ZERO_DIGIT  		[1-9]
DIGIT					[0-9]
int_num					0|[1-9][0-9]*
float_num				(({NON_ZERO_DIGIT}{DIGIT}*|"0")("."){DIGIT}{DIGIT}*{EXPONENT}?("-"|"+")?({NON_ZERO_DIGIT}{DIGIT}*|"0"))
ar_op_mul				"*"
ar_op_add				"+"
assignment_op			"="
rel_op_smaller 			"<"
rel_op_smallerOrEqual	"<="
rel_op_equal			"=="
rel_op_biggerOrEqual	">="
rel_op_bigger			">"
rel_op_notEqual			"!="
INT_KW					"int"
FLOAT_KW				"float"
VOID_KW					"void"
IF_KW					"if"
RETURN_KW				"return"
ID 						[a-z]["_"?A-Za-z0-9][A-Za-z0-9]*
comma					","
colon					":"
semicolon				";"
opening_parentheses		"("
closing_parentheses		")"
opening_brackets		"["
closing_brackets		"]"	
opening_curlybraces		"{"
closing_curlybraces		"}"

Unrecognized_token		.

%x comment1

%%

\n						{line++;}

"/*"  					BEGIN(comment1);
<comment1>[^*\n]*                   	 /*do nothing till ‘*’  */
<comment1>\n    		line++;          /*delete new line in multi-line comment*/
<comment1>"*/"  		BEGIN(0);

{WhitespaceTab} 			{fprintf(yyout, "");}

{int_num} 					{create_and_store_token(int_num,yytext,line);fprintf(yyout,"Line %d : found token of type INT_NUM , lexeme %s\n", line, yytext); return 1;}
{float_num} 				{create_and_store_token(float_num,yytext,line);fprintf(yyout,"Line %d : found token of type FLOAT_NUM , lexeme %s\n", line, yytext); return 1;}
{ar_op_mul} 				{create_and_store_token(ar_op_mul,yytext,line);fprintf(yyout,"Line %d : found token of type ar_op_mul , lexeme %s\n", line, yytext); return 1;}
{ar_op_add} 				{create_and_store_token(ar_op_add,yytext,line);fprintf(yyout,"Line %d : found token of type ar_op_add , lexeme %s\n", line, yytext); return 1;}
{rel_op_smaller} 			{create_and_store_token(rel_op_smaller,yytext,line);fprintf(yyout,"Line %d : found token of type rel_op_smaller , lexeme %s\n", line, yytext); return 1;}
{rel_op_smallerOrEqual} 	{create_and_store_token(rel_op_smallerOrEqual,yytext,line);fprintf(yyout,"Line %d : found token of type rel_op_smallerOrEqual , lexeme %s\n", line, yytext); return 1;}
{rel_op_equal} 				{create_and_store_token(rel_op_equal,yytext,line);fprintf(yyout,"Line %d : found token of type rel_op_equal , lexeme %s\n", line, yytext); return 1;}
{rel_op_biggerOrEqual} 		{create_and_store_token(rel_op_biggerOrEqual,yytext,line);fprintf(yyout,"Line %d : found token of type rel_op_biggerOrEqual , lexeme %s\n", line, yytext); return 1;}
{rel_op_bigger} 			{create_and_store_token(rel_op_bigger,yytext,line);fprintf(yyout,"Line %d : found token of type rel_op_bigger , lexeme %s\n", line, yytext); return 1;}
{rel_op_notEqual} 			{create_and_store_token(rel_op_notEqual,yytext,line);fprintf(yyout,"Line %d : found token of type rel_op_notEqual , lexeme %s\n", line, yytext); return 1;}
{assignment_op} 			{create_and_store_token(assignment_op,yytext,line);fprintf(yyout,"Line %d : found token of type assignment_op , lexeme %s\n", line, yytext); return 1;}
{INT_KW} 					{create_and_store_token(INT_KW,yytext,line);fprintf(yyout,"Line %d : found token of type INT_KW , lexeme %s\n", line, yytext); return 1;}
{FLOAT_KW}					{create_and_store_token(FLOAT_KW,yytext,line);fprintf(yyout,"Line %d : found token of type FLOAT_KW , lexeme %s\n", line, yytext); return 1;}
{VOID_KW} 					{create_and_store_token(VOID_KW,yytext,line);fprintf(yyout,"Line %d : found token of type VOID_KW , lexeme %s\n", line, yytext); return 1;}
{IF_KW} 					{create_and_store_token(IF_KW,yytext,line);fprintf(yyout,"Line %d : found token of type IF_KW , lexeme %s\n", line, yytext); return 1;}
{RETURN_KW} 				{create_and_store_token(RETURN_KW,yytext,line);fprintf(yyout,"Line %d : found token of type RETURN_KW , lexeme %s\n", line, yytext); return 1;}

{ID} 						{create_and_store_token(ID,yytext,line);fprintf(yyout,"Line %d : found token of type ID , lexeme %s\n", line, yytext); return 1;}
{comma} 					{create_and_store_token(comma,yytext,line);fprintf(yyout,"Line %d : found token of type comma , lexeme %s\n", line, yytext); return 1;}
{colon} 					{create_and_store_token(colon,yytext,line);fprintf(yyout,"Line %d : found token of type colon , lexeme %s\n", line, yytext); return 1;}
{semicolon} 				{create_and_store_token(semicolon,yytext,line);fprintf(yyout,"Line %d : found token of type semicolon , lexeme %s\n", line, yytext); return 1;}
{opening_parentheses} 		{create_and_store_token(opening_parentheses,yytext,line);fprintf(yyout,"Line %d : found token of type opening_parentheses , lexeme %s\n", line, yytext); return 1;}
{closing_parentheses} 		{create_and_store_token(closing_parentheses,yytext,line);fprintf(yyout,"Line %d : found token of type closing_parentheses , lexeme %s\n", line, yytext); return 1;}
{opening_brackets} 			{create_and_store_token(opening_brackets,yytext,line);fprintf(yyout,"Line %d : found token of type opening_brackets , lexeme %s\n", line, yytext); return 1;}
{closing_brackets} 			{create_and_store_token(closing_brackets,yytext,line);fprintf(yyout,"Line %d : found token of type closing_brackets , lexeme %s\n", line, yytext); return 1;}
{opening_curlybraces} 		{create_and_store_token(opening_curlybraces,yytext,line);fprintf(yyout,"Line %d : found token of type opening_curlybraces , lexeme %s\n", line, yytext); return 1;}
{closing_curlybraces} 		{create_and_store_token(closing_curlybraces,yytext,line);fprintf(yyout,"Line %d : found token of type closing_curlybraces , lexeme %s\n", line, yytext); return 1;}
<<EOF>> 					{create_and_store_token(EOF_TOKEN,yytext,line);fprintf(yyout,"Line %d : found token of type EOF_TOKEN , lexeme %s\n", line, yytext); return 0;}
{Unrecognized_token} 		{fprintf(yyout,"Line %d : found token of type Unrecognized , lexeme %s\n", line, yytext); return 1;}

%%

#include <string.h>
#include <stdio.h>
#include "syntaxAnalyzer.h"

void main()
{
    yyin = fopen("C:\\temp\\test1.txt", "r");
    yyout = fopen("C:\\temp\\test1_316670173_312349822_204589261_lex.txt", "w");
    yyout_parser = fopen("C:\\temp\\test1_316670173_312349822_204589261_syntactic.txt", "w");
    while (yylex() != 0) {};
    backToFirst();
    parser();

    yyrestart(yyin);
    yyrestart(yyout);
    yyrestart(yyout_parser);
    line = 1;

    emptyCreateAndStore();

    yyin = fopen("C:\\temp\\test2.txt", "r");
    if (yyin == NULL)
        printf("Can't open file\n");
    yyout = fopen("C:\\temp\\test2_316670173_312349822_204589261_lex.txt", "w");
    if (yyout == NULL)
        printf("Can't open file\n");
    yyout_parser = fopen("C:\\temp\\test2_316670173_312349822_204589261_syntactic.txt", "w");
    if (yyout_parser == NULL)
        printf("Can't open file\n");

    while (yylex() != 0) {};
    backToFirst();
    parser();

    fclose(yyin);
    fclose(yyout);
    fclose(yyout_parser);
}