#define _CRT_SECURE_NO_WARNINGS
#include "Token.h"
#include "syntaxAnalyzer.h"
#include "symbolTable.h"

Token* t;
DataItem* dt;
HashTable* test;
DataItem* check;
int countParams;
char* lastFuncName;
char* typeOfExpr;
int countDim;
int flag;
char* lastArr;
int flag_if;




void parser() {
    parse_PROG();
    char* Fname;
    char* Rtype;

    //recognizes full def func for every pre def//

    for (int i = 0; i < SIZE; i++){
        if (curr_table != NULL) {
            if (curr_table->hashArray[i] != NULL) {
                if (strcmp(curr_table->hashArray[i]->role, "pre-def") == 0) {
                    Fname = curr_table->hashArray[i]->key;
                    Rtype = curr_table->hashArray[i]->type;
                    delete(curr_table, curr_table->hashArray[i]);
                    DataItem* temp = search(curr_table, Fname,"full-def");

                    if (temp != NULL) {
                        if (strcmp(Rtype, temp->type) != 0) {
                            fprintf(yyout_parser_semantic, "error, different return type between func pre-def and full-def, func name : %s \n", Fname);
                        }
                    }
                    else {
                        fprintf(yyout_parser_semantic, "error, there isn't a matching full def func, func name : %s \n", Fname);
                    }
                    
                }
            }

        }
        }
    match(EOF_TOKEN);
    freeHash(curr_table);

}

void parse_PROG()
{
    dt = (DataItem*)malloc(sizeof(DataItem));
    ListDims* dims = (ListDims*)calloc(1, sizeof(ListDims));
    ListParams* params = (ListParams*)calloc(1, sizeof(ListParams));
    dt->lp = params;
    dt->list = dims;
    curr_table = create_table(NULL);
    typeOfExpr = "int";
    flag_if = 0;

    parse_GLOBAL_VARS();

    parse_FUNC_PREDEFS();
    t = back_token();
    t = back_token();
    t = back_token();
    parse_FUNC_FULL_DEFS();
}  /* end of parse_PROG  */

void parse_GLOBAL_VARS()
{

    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        fprintf(yyout_parser, "Rule(GLOBAL_VARS -> VAR_DEC GLOBAL_VARS')\n");
        t = back_token();
        parse_VAR_DEC();
        parse_GLOBAL_VARS_TAG();
        break;
    default:
        parse_GLOBAL_VARS_TAG();

        break;
    }
}

void parse_GLOBAL_VARS_TAG()
{
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW };

    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        fprintf(yyout_parser, "Rule(GLOBAL_VARS' -> VAR_DEC GLOBAL_VARS'|epsilon)\n");
        t = back_token();
        parse_VAR_DEC();

        parse_GLOBAL_VARS_TAG();
        break;

    default:
        fprintf(yyout_parser, "Expected token of type INT_KW, FLOAT_KW at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));

        break;
    }

}

void parse_VAR_DEC() {
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW, ID, opening_curlybraces, IF_KW, RETURN_KW };
    t = next_token();
    dt->role = "var";
    dt->sizeOfDims = 0;
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        dt->type = t->lexeme;
        fprintf(yyout_parser, "Rule(VAR_DEC -> TYPE id VAR_DEC')\n");
        t = back_token();
        parse_TYPE();
        match(ID);
        t = back_token();
        t = next_token();
        dt->key = t->lexeme;
        if (next_token()->kind == opening_parentheses) {
            t=back_token();
            t=back_token();
            t = back_token();
            t = back_token();

            break;

        }
        back_token();
        parse_VAR_DEC_TAG();
        if (searchInScope(curr_table, dt->key)==NULL) {
            insert(curr_table, dt);
           
        }
        else {
            fprintf(yyout_parser_semantic, "error, there are multiple declarations for the same id : %s, line : %d \n",dt->key,t->lineNumber);
            fflush(yyout_parser_semantic);
        }
    
        break;
    default:


        break;
    }
}

void parse_VAR_DEC_TAG() {
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW, ID, opening_curlybraces, IF_KW, RETURN_KW };
    t = next_token();
    switch (t->kind) {
    case semicolon:
        fprintf(yyout_parser, "Rule(VAR_DEC' -> ;|[DIM_SIZES];)\n");
        break;
    case opening_brackets:
        dt->list = (ListDims*)calloc(1, sizeof(ListDims));
        dt->sizeOfDims = 0;
        fprintf(yyout_parser, "Rule(VAR_DEC' -> ;|[DIM_SIZES];)\n");
        parse_DIM_SIZES();

        if (match(closing_brackets))
            match(semicolon);
        break;

    default:


        break;
    }
}


void parse_TYPE() {
    int Follow[] = { ID };
    t = next_token();
    switch (t->kind) {
    case INT_KW:

    case FLOAT_KW:
        fprintf(yyout_parser, "Rule(TYPE -> int | float)\n");
        break;
    default:


        break;
    }
}


void parse_DIM_SIZES() {
    int Follow[] = { closing_brackets };
    t = next_token();
    int numb;
    switch (t->kind) {
    case int_num:
        numb = atoi(t->lexeme);
        addDims(numb, dt);
        fprintf(yyout_parser, "Rule(DIM_SIZES -> int_num DIM_SIZES')\n"); 
            parse_DIM_SIZES_TAG();

        break;
    default:
    

        break;
    }
}

void parse_DIM_SIZES_TAG() {
    int Follow[] = { closing_brackets };
    t = next_token();
    switch (t->kind) {
    case comma:

        fprintf(yyout_parser, "Rule(DIM_SIZES' -> epsilon|, DIM_SIZES)\n");
        parse_DIM_SIZES();
        break;
    case closing_brackets:
        fprintf(yyout_parser, "Rule(DIM_SIZES' -> epsilon|, DIM_SIZES)\n");
        t = back_token();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type comma at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}

void parse_FUNC_PREDEFS() {
    dt->role = "pre-def";
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
    case VOID_KW:
        fprintf(yyout_parser, "Rule(FUNC_PREDEFS -> FUNC_PROTOTYPE FUNC_PREDEFS')\n");
        t = back_token();
        parse_FUNC_PROTOTYPE();
       
        if (match(semicolon)) {
            curr_table = freeHash(curr_table);
            insert(curr_table, dt);
            
            parse_FUNC_PREDEFS_TAG();
        }
        else {
           
            t = back_token();
            if (match(opening_curlybraces)) {
                curr_table = freeHash(curr_table);
                dt->role = "full-def";
                while (t->kind != opening_parentheses)
                    t = back_token();
            }
        }
        

        break;
    default:

        break;
    }
}

void parse_FUNC_PREDEFS_TAG() {
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW };

    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
    case VOID_KW:
        fprintf(yyout_parser, "Rule(FUNC_PREDEFS' -> FUNC_PROTOTYPE FUNC_PREDEFS'|epsilon)\n");
        t = back_token();
        parse_FUNC_PROTOTYPE();
        if (match(semicolon)) {
            curr_table = freeHash(curr_table);
            insert(curr_table, dt);
           
            parse_FUNC_PREDEFS_TAG();
        } 
        else {
            t = back_token();
            if (match(opening_curlybraces)) {
                dt->role = "full-def";
                curr_table = freeHash(curr_table);
                while (t->kind != opening_parentheses)
                    t = back_token();
                    
               
            }
        }

        break;
    default:
        fprintf(yyout_parser, "Expected token of type INT_KW, FLOAT_KW, VOID_KW at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }

}

void parse_FUNC_PROTOTYPE() {
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW, opening_curlybraces };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
    case VOID_KW:
        fprintf(yyout_parser, "Rule(FUNC_PROTOTYPE -> RETURNED_TYPE id (PARAMS))\n");
        t = back_token();
        parse_RETURNED_TYPE();
        match(ID);
        t = back_token();
        t = next_token();
        dt->key = t->lexeme;
        lastFuncName = dt->key;

        if (match(opening_parentheses)) {
  
      
            curr_table = create_table(curr_table);
             
            t = back_token();
        }
        else {
            t = back_token();
        }
        
               
        if (!match(opening_parentheses))
            t = back_token();

        parse_PARAMS();
        if (!match(closing_parentheses))
            t = back_token();
        break;
    default:

        break;
    }
}

void parse_FUNC_FULL_DEFS() {
    dt->role = "full-def";
    int Follow[] = { EOF_TOKEN };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
    case VOID_KW:
        fprintf(yyout_parser, "Rule(FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS')\n");
        t = back_token();
        parse_FUNC_WITH_BODY();

        parse_FUNC_FULL_DEFS_TAG();
        break;
    default:

        break;
    }
}

void parse_FUNC_FULL_DEFS_TAG() {
    int Follow[] = { EOF_TOKEN,closing_curlybraces };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
    case VOID_KW:
        fprintf(yyout_parser, "Rule(FUNC_FULL_DEFS' -> FUNC_FULL_DEFS|epsilon)\n");
        t = back_token();
        parse_FUNC_FULL_DEFS();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type INT_KW, FLOAT_KW, VOID_KW at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        t = next_token();
        if (t->kind == closing_curlybraces)
            parse_FUNC_FULL_DEFS();
        break;
    }
}

void parse_FUNC_WITH_BODY() {
    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW, EOF_TOKEN };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
    case VOID_KW:
        fprintf(yyout_parser, "Rule(FUNC_WITH_BODY -> FUNC_PROTOTYPE COMP_STMT)\n");
        t = back_token();
        parse_FUNC_PROTOTYPE();

        //insert func name into global symbol table//

        HashTable* temp = curr_table;
        while (temp->father_table != NULL) {
            temp = temp->father_table;
        }
        insert(temp, dt);
      
        parse_COMP_STMT();

        
        break;
    default:

        break;
    }
}

void parse_RETURNED_TYPE()
{
    int Follow[] = { ID };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        dt->type = t->lexeme;
        fprintf(yyout_parser, "Rule(RETURNED_TYPE -> TYPE|void)\n");
        t = back_token();
        parse_TYPE();
        break;

    case VOID_KW:

        break;

    default:

        break;
    }
}

void parse_PARAMS()
{
    dt->lp = (ListParams*)calloc(1, sizeof(ListParams));

    int Follow[] = { closing_parentheses };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:

        fprintf(yyout_parser, "Rule(PARAMS->PARAM_LIST | epsilon)\n");
        t = back_token();
        parse_PARAM_LIST();
        
        break;


    default: fprintf(yyout_parser, "Expected token of type INT_KW, FLOAT_KW, closing_parentheses at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_PARAM_LIST()
{
    int Follow[] = { closing_parentheses };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        fprintf(yyout_parser, "Rule(PARAM_LIST->PARAM PARAM_LIst')\n");
        t = back_token();
     
        parse_PARAM();

        parse_PARAM_LIST_TAG();
        break;
    default:

        break;
    }
}
void parse_PARAM_LIST_TAG()
{
    int Follow[] = { closing_parentheses };
    t = next_token();
    switch (t->kind) {
    case comma:
        fprintf(yyout_parser, "Rule(PARAM_LIST'->,PARAM PARAM_LIST')\n");
        parse_PARAM();

        parse_PARAM_LIST_TAG();
        break;

    default:
        fprintf(yyout_parser, "Expected token of type comma at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_PARAM()
{
    DataItem* zz;
    
    zz=(DataItem*)malloc(sizeof(DataItem));
    int Follow[] = { closing_parentheses, comma };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        zz->type = t->lexeme;
        t = next_token();
        zz->key = t->lexeme;
        zz->role = "var";
        t = back_token();
        addParams(t->lexeme, dt);
        fprintf(yyout_parser, "Rule(PARAM->TYPE id PARAM')\n"); 
        t = back_token();
        parse_TYPE();
        
        if (!match(ID))
            t = back_token();

        //inserts func params and checks param declaration//

        if (strcmp(dt->role, "full-def") == 0) {
            if (searchInScope(curr_table, zz->key) == NULL)
            {
              

                insert(curr_table, zz);

            }
            else {
                fprintf(yyout_parser_semantic, "error, multiple declaration with the same name in parameter list. variable name : %s, line number : %d \n", zz->key, t->lineNumber);
            }
        }
        
        parse_PARAM_TAG();

        break;
    default:

        break;
    }
}
void parse_PARAM_TAG()
{
    int Follow[] = { closing_parentheses, comma };
    t = next_token();
    switch (t->kind) {
    case opening_brackets:
        fprintf(yyout_parser, "Rule(PARAM'->epsilon | [DIM_SIZES])\n");
        parse_DIM_SIZES();
        if (!match(closing_brackets))
            t = back_token();
        break;

    default:
        fprintf(yyout_parser, "Expected token of type opening_brackets at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));

        break;
    }
}
void parse_COMP_STMT()
{

    int Follow[] = { INT_KW, FLOAT_KW, VOID_KW, EOF_TOKEN };
    t = next_token();
    switch (t->kind) {
    case opening_curlybraces:
        curr_table = create_table(curr_table);

        fprintf(yyout_parser, "Rule(COMP_STMT->{VAR_DEC_LIST STMT_LIST})\n");
        parse_VAR_DEC_LIST();
        parse_STMT_LIST();
        if (!match(closing_curlybraces))
            t = back_token();
        else {
            curr_table = freeHash(curr_table);
            if(flag_if==0){
            curr_table = freeHash(curr_table);
            }
        }
        break;
    case FLOAT_KW:
    case INT_KW:
    case ID:
    case IF_KW:
    case RETURN_KW:
        t = back_token();
        match(opening_curlybraces);
        t = back_token();
        fprintf(yyout_parser, "Rule(COMP_STMT->{VAR_DEC_LIST STMT_LIST})\n");
        parse_VAR_DEC_LIST();
        parse_STMT_LIST();
        if (!match(closing_curlybraces))
            t = back_token();
        else
            //closes func body and scope//

            curr_table=freeHash(curr_table);
        break;
    default:

        break;
    }
}
void parse_VAR_DEC_LIST()
{
    int Follow[] = { ID, opening_curlybraces, IF_KW, RETURN_KW };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        fprintf(yyout_parser, "Rule(VAR_DEC_LIST->VAR_DEC VAR_DEC_LIST')\n");
        t = back_token();
        parse_VAR_DEC();
        parse_VAR_DEC_LIST_TAG();
        break;
    default:

        t = back_token();
        break;
    }
}
void parse_VAR_DEC_LIST_TAG()
{
    int Follow[] = { ID, opening_curlybraces, IF_KW, RETURN_KW };
    t = next_token();
    switch (t->kind) {
    case INT_KW:
    case FLOAT_KW:
        fprintf(yyout_parser, "Rule(VAR_DEC_LIST-> VAR_DEC VAR_DEC_LIST' | epsilon)\n");
        t = back_token();
        parse_VAR_DEC();

        parse_VAR_DEC_LIST_TAG();
        break;

    default:
        fprintf(yyout_parser, "Expected token of type INT_KW, FLOAT_KW at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_STMT_LIST()
{
    int Follow[] = { closing_curlybraces };
    t = next_token();
    switch (t->kind) {
    case ID:
    case opening_curlybraces:
    case IF_KW:
    case RETURN_KW:
        fprintf(yyout_parser, "Rule(STMT_LIST -> STMT STMT_LIST')\n");
        t = back_token();
        parse_STMT();
        parse_STMT_LIST_TAG();
        break;
    default:

        break;
    }
}
void parse_STMT_LIST_TAG()
{
    int Follow[] = { closing_curlybraces };
    t = next_token();
    switch (t->kind) {
    case semicolon:
        fprintf(yyout_parser, "Rule(STMT_LIST' -> ;STMT STMT_LIST' | epsilon)\n");
        parse_STMT();

        parse_STMT_LIST_TAG();
        break;
    case ID:
    case opening_curlybraces:
    case IF_KW:
    case RETURN_KW:
        t = back_token();
        match(semicolon);
        t = back_token();
        parse_STMT();
        parse_STMT_LIST_TAG();
    default:
        fprintf(yyout_parser, "Expected token of type semicolon at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        t = back_token();
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_STMT()
{
    char* tempCheckName;
    int Follow[] = { closing_curlybraces, semicolon };
    t = next_token();
    switch (t->kind) {
    case ID:
        fprintf(yyout_parser, "Rule(STMT -> VAR=EXPR | COMP_STMT | IF_STMT | CALL| RETURN_STMT)\n");
        t = next_token();
        if (t->kind == opening_parentheses) {
            t = back_token();
            t = back_token();
            parse_CALL();
            break;
        }
        else {
            t = back_token();
            t = back_token();
            parse_VAR();
             t = back_token();
             t = next_token();
             tempCheckName = t->lexeme;
            if (!match(assignment_op)) {
                t = back_token();
            }
            else {
                
                
                           //checking errors for arrays*/

                if (check != NULL) {
                    if (strcmp(check->role, "var") != 0) {
                        fprintf(yyout_parser_semantic, "error, trying to assign to function. name of func - %s. line number %d\n", check->key, t->lineNumber);
                    }
                    if (check->sizeOfDims > 0) {
                        fprintf(yyout_parser_semantic, "error, trying to assign to an array. name of array - %s. line number %d\n", check->key, t->lineNumber);

                    }

                }
            }
            
            //checking assignment types//

            parse_EXPR();
            check = search(curr_table, tempCheckName, NULL);
            if (check != NULL) {
                if (strcmp(typeOfExpr, "float") == 0 && strcmp(check->type, "int") == 0) {
                    fprintf(yyout_parser_semantic, "error, assigning float type expression to int type variable. variable: - %s. line number %d\n", check->key, t->lineNumber);
                }
                else if (strcmp(typeOfExpr, "undefined") == 0) {
                    fprintf(yyout_parser_semantic, "error, assigning undefined type expression to variable. variable: - %s. line number %d\n", check->key, t->lineNumber);
                }
            }
            typeOfExpr = "int";

            break;
        }
        break;
    case opening_curlybraces:
        fprintf(yyout_parser, "Rule(STMT -> VAR=EXPR | COMP_STMT | IF_STMT | CALL| RETURN_STMT)\n");
        t = back_token();
        parse_COMP_STMT();
        break;
    case IF_KW:
        fprintf(yyout_parser, "Rule(STMT -> VAR=EXPR | COMP_STMT | IF_STMT | CALL| RETURN_STMT)\n");
        t = back_token();
        parse_IF_STMT();
        break;
    case RETURN_KW:
        fprintf(yyout_parser, "Rule(STMT -> VAR=EXPR | COMP_STMT | IF_STMT | CALL| RETURN_STMT)\n");

        t = back_token();
        parse_RETURN_STMT();
        break;
    default:

        break;
    }
}
void parse_IF_STMT()
{
    int Follow[] = { closing_curlybraces, semicolon };
    t = next_token();
    switch (t->kind) {
    case IF_KW:
        fprintf(yyout_parser, "Rule(IF_STMT -> if(CONDITION) STMT)\n");
        if (!match(opening_parentheses))
            t = back_token();
        parse_CONDITION();
        if (!match(closing_parentheses))
            t = back_token();
        flag_if = 1;
        parse_STMT();
        flag_if = 0;
        break;
    default:

        break;
    }
}
void parse_CALL()
{
    char* nameOfFuncCall;
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets, ar_op_add, ar_op_mul,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind)
    {
    case ID:
        fprintf(yyout_parser, "Rule(CALL -> id(ARGS))\n");
        if (!match(opening_parentheses))
            t = back_token();
        countParams = 1;
        nameOfFuncCall = t->lexeme;
        parse_ARGS();
        
        if (!match(closing_parentheses)) 
            t = back_token();
        else {
t = back_token();
t = back_token();
if (t->kind == opening_parentheses)
countParams = 0;
t = next_token();
t = next_token();
        }
        //checking uniformity of params//
        if (search(curr_table, nameOfFuncCall, NULL) != NULL) {


            if ((countParams-1) != search(curr_table, nameOfFuncCall, NULL)->sizeOfLp)
                fprintf(yyout_parser_semantic, "error, number of function call parameters and definition parameters mismatch. call name: %s. line number: %d \n", nameOfFuncCall, t->lineNumber);
        }


        break;
    default:

        break;
    }
}
void parse_ARGS()
{
    int Follow[] = { closing_parentheses };
    t = next_token();
    switch (t->kind) {
    case ID:
    case int_num:
    case float_num:
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(ARGS -> ARG_LIST|epsilon)\n");
        t = back_token();
        parse_ARG_LIST();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type id, int_num, float_num, opening_parentheses, at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_ARG_LIST()
{
    int Follow[] = { closing_parentheses };
    t = next_token();
    switch (t->kind) {
    case ID:
    case int_num:
    case float_num:
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(ARG_LIST -> EXPR ARG_LIST')\n");
        t = back_token();
        parse_EXPR();
        parse_ARG_LIST_TAG();
        break;
    default:

        break;
    }
}
void parse_ARG_LIST_TAG()
{
    int Follow[] = { closing_parentheses };
    t = next_token();
    countParams++;
    switch (t->kind) {
    case comma:
        fprintf(yyout_parser, "Rule(ARG_LIST' -> , EXPR ARG_LIST'|epsilon)\n");
        parse_EXPR();
        t = back_token();
        parse_ARG_LIST_TAG();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type comma at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_RETURN_STMT()
{
    int Follow[] = { semicolon, closing_curlybraces };
    t = next_token();
    switch (t->kind) {
    case RETURN_KW:
        fprintf(yyout_parser, "Rule(RETURN_STMT -> return RETURN_STMT')\n");
        parse_RETURN_STMT_TAG();
        break;
    default:

        break;
    }
}
void parse_RETURN_STMT_TAG()
{
    int Follow[] = { semicolon, closing_curlybraces };
    t = next_token();
    switch (t->kind) {
    case ID:
        check = search(curr_table, t->lexeme, NULL);
        DataItem* check1;
        check1 = search(curr_table, lastFuncName, NULL);

        //checking return types//
 
            if (check != NULL) {
                if (strcmp(check->type,check1->type)!=0)
                {
                    fprintf(yyout_parser_semantic, "error, type of return value doesnt match function return type, line number : %d \n",t->lineNumber);
                }
            }
            else {
                fprintf(yyout_parser_semantic, "error, there is no matching id, line number - %d \n", t->lineNumber);
            }
          
    case int_num:
    case float_num:
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(RETURN_STMT' -> epsilon|EXPR)\n");
        t = back_token();
        parse_EXPR();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type id, int_num, float_num, opening_parentheses, at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_VAR()
{
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets, ar_op_add, ar_op_mul, assignment_op,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case ID:
        check = search(curr_table, t->lexeme, NULL);

        //checking declaration of id's//
        if (check == NULL) {
            fprintf(yyout_parser_semantic, "error, declaration missing for id : %s, line number : %d \n", t->lexeme,t->lineNumber);
            fflush(yyout_parser_semantic);
            typeOfExpr = "undefined";
        }
        else {
            if (strcmp(typeOfExpr, "int") == 0) {
                typeOfExpr = check->type;
            }
        }
        fprintf(yyout_parser, "Rule(VAR -> id VAR')\n");
        parse_VAR_TAG();
        break;
    default:

        break;
    }
}
void parse_VAR_TAG()
{
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets, ar_op_add, ar_op_mul, assignment_op,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case opening_brackets:
        countDim = 0;
        flag = 0;
        t = back_token();
        lastArr = t->lexeme;
        t = next_token();
        fprintf(yyout_parser, "Rule(VAR' -> epsilon|[EXPR_LIST])\n");
        parse_EXPR_LIST();
        countDim = 0;
        if (!match(closing_brackets))
            t = back_token();
        break;

    default:
        fprintf(yyout_parser, "Expected token of type opening_brackets at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}
void parse_EXPR_LIST()
{
    ListDims* listHead;
    DataItem* arrItem;
    int value;
    char* lex;
    int Follow[] = { closing_brackets };
    t = next_token();
    char* id;
    switch (t->kind) {
    case ID:
        countDim++;
        id = t->lexeme;
        fprintf(yyout_parser, "Rule(EXPR_LIST -> EXPR EXPR_LIST')\n");
        t = back_token();
        parse_EXPR();
         if ((strcmp(typeOfExpr, "int") != 0||strcmp(search(curr_table,id, NULL)->type, "int") != 0) && flag==0) {
            fprintf(yyout_parser_semantic, "error, type of array expression is not int. expression id : % s, line number % d \n", id, t->lineNumber);

            flag = 1;
        }
         
        parse_EXPR_LIST_TAG();
        break;
    case int_num:
        lex = t->lexeme;
      

        fprintf(yyout_parser, "Rule(EXPR_LIST -> EXPR EXPR_LIST')\n");
        t = back_token();
        parse_EXPR();
       
        //checking array consists of int values//
        if (strcmp(typeOfExpr, "int") != 0 && flag==0) {
            fprintf(yyout_parser_semantic, "error, type of array expression is not int. expression id : % s, line number % d, \n", t->lexeme, t->lineNumber);
            flag = 1;
        }
        else
        {
            
            value = atoi(lex);
            arrItem=search(curr_table,lastArr, NULL);
            if (arrItem->sizeOfDims>0) {
                listHead = arrItem->list->head;

                for (int i = 0; i < countDim; i++) {
                    listHead = listHead->next;
                }

                //checking array dimensions//
                if (listHead != NULL) {
                    if (listHead->dim < value) {
                        fprintf(yyout_parser_semantic, "error, value %d exceeds the dimension size of the arr : '%s' line number : %d \n", value, check->key, t->lineNumber);
                    }
                }
                countDim++;
            }
        }
        parse_EXPR_LIST_TAG();
        break;

        //expression checking//

    case float_num:
        countDim++;
        fprintf(yyout_parser_semantic, "error, type of array expression is float. expression id : %s, line number %d,\n", t->lexeme, t->lineNumber);
        fprintf(yyout_parser, "Rule(EXPR_LIST -> EXPR EXPR_LIST')\n");
        t = back_token();
        parse_EXPR();
        parse_EXPR_LIST_TAG();
        break;
    case opening_parentheses:
        countDim++;
        fprintf(yyout_parser, "Rule(EXPR_LIST -> EXPR EXPR_LIST')\n");
        t = back_token();
        parse_EXPR();
        if (strcmp(typeOfExpr, "int") != 0) {
            fprintf(yyout_parser_semantic, "error, type of array expression is not int. expression id : % s, line number % d, \n", t->lexeme, t->lineNumber);
        }
        parse_EXPR_LIST_TAG();
        break;
    default:

        break;
    }
}
void parse_EXPR_LIST_TAG()
{
    int Follow[] = { closing_brackets };
    t = next_token();
    switch (t->kind) {
    case comma:
        fprintf(yyout_parser, "Rule(EXPR_LIST' -> , EXPR EXPR_LIST'|epsilon)\n");
        parse_EXPR();
        t = back_token();
        parse_EXPR_LIST();
        t = back_token();
        parse_EXPR_LIST_TAG();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type comma at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}

void parse_CONDITION() {
    int Follow[] = { closing_parentheses };
    t = next_token();
    switch (t->kind) {
    case ID:
    case int_num:
    case float_num:
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(CONDITION -> EXPR rel_op EXPR)\n");
        t = back_token();
        parse_EXPR();
        switch (t->kind) {
        case rel_op_bigger:  match(rel_op_bigger); break;
        case rel_op_smaller:  match(rel_op_smaller); break;
        case rel_op_smallerOrEqual:  match(rel_op_smallerOrEqual); break;
        case rel_op_equal: match(rel_op_equal); break;
        case rel_op_biggerOrEqual: match(rel_op_biggerOrEqual); break;
        case rel_op_notEqual: match(rel_op_notEqual); break;
        }

        parse_EXPR();
        break;
    default:

        break;
    }
}

void parse_EXPR() {
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case ID:
    case int_num:
    case float_num:
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(EXPR -> TERM EXPR')\n");
        t = back_token();
        parse_TERM();
        parse_EXPR_TAG();
        break;

    default:

        break;
    }
}

void parse_EXPR_TAG() {
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case ar_op_add:
        fprintf(yyout_parser, "Rule(EXPR' -> + TERM EXPR'|epsilon)\n");
        t = back_token();
        if (!match(ar_op_add))
            t = back_token();
        parse_TERM();
        parse_EXPR_TAG();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type ar_op_add at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}

void parse_TERM() {
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets, ar_op_add,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case ID:
    case int_num:
    case float_num:
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(TERM -> FACTOR TERM')\n");
        t = back_token();
        parse_FACTOR();
        parse_TERM_TAG();
        break;
    default:
        break;
    }
}

void parse_TERM_TAG() {
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets, ar_op_add,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case ar_op_mul:
        fprintf(yyout_parser, "Rule(TERM' -> *FACTOR TERM'|epsilon)\n");
        t = back_token();
        if (!match(ar_op_mul))
            t = back_token();
        parse_FACTOR();
        parse_TERM_TAG();
        break;
    default:
        fprintf(yyout_parser, "Expected token of type ar_op_mul at line: %d,\n Actual token of type %s, lexeme: %s\n", t->lineNumber, getTokenName(t->kind), t->lexeme);
        recovery(&Follow, sizeof(Follow) / sizeof(Follow[0]));
        break;
    }
}

void parse_FACTOR()
{
    int Follow[] = { semicolon, closing_curlybraces, comma, closing_parentheses, closing_brackets, ar_op_add, ar_op_mul,
        rel_op_bigger, rel_op_biggerOrEqual, rel_op_smaller, rel_op_equal, rel_op_smallerOrEqual, rel_op_notEqual };
    t = next_token();
    switch (t->kind) {
    case ID:
        fprintf(yyout_parser, "Rule(FACTOR -> VAR|CALL|int_num|float_num|(EXPR))\n");
        t = next_token();
        if (t->kind == opening_parentheses)
        {
            t = back_token();
            t = back_token();
            parse_CALL();
        }
        else {
            t = back_token();
            t = back_token();
            parse_VAR();
        }
        break;
    case int_num:
        fprintf(yyout_parser, "Rule(FACTOR -> VAR|CALL|int_num|float_num|(EXPR))\n");
        break;
    case float_num:
        fprintf(yyout_parser, "Rule(FACTOR -> VAR|CALL|int_num|float_num|(EXPR))\n");
        break;
    case opening_parentheses:
        fprintf(yyout_parser, "Rule(FACTOR -> VAR|CALL|int_num|float_num|(EXPR))\n");
        parse_EXPR();
        if (!match(closing_parentheses))
            t = back_token();
        break;
    default:

        break;
    }
}

int match(int tokenToMatch) {
    Token* currentToken = next_token();

    if (currentToken->kind != tokenToMatch) {
        fprintf(yyout_parser, "Match: Expected token: '%s' at line: %d,\n", getTokenName(tokenToMatch), currentToken->lineNumber);
        fprintf(yyout_parser, "Actual token: '%s', lexeme: '%s' .\n", getTokenName(currentToken->kind), currentToken->lexeme);
        return 0;
    }
    return 1;
}

void recovery(int* follow, int sizeoffollow)
{
    Token* token = back_token();
    int isFollow = 0;
    int i;

    while (!isFollow)
    {
        token = next_token();
        if (token->kind == EOF_TOKEN)
        {
            isFollow = 1;
            back_token();
            break;
        }
        else
        {
            for (i = 0; i < sizeoffollow; i++)
            {
                if (follow[i] == token->kind)
                {
                    isFollow = 1;
                    back_token();
                    break;
                }
            }
        }
    }
}









