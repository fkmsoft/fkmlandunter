%{
#include <stdio.h>

#include "server.h"

void  yyerror(const char *str)
{
    fprintf(stderr, "error: %s\n", str);
}

int yywrap()
{
    return 1;
}

%}

%token TOKLOGIN TOKPLAY TOKLOGOUT TOKMSG NEWLINE

%union
{
    int num;
    char *str;
}

%token <num> NUMBER
%token <str> WORD

%type<str> string

%%
commands:
        | commands command NEWLINE
        ;

command:
       login | play | msg | logout
       ;

login:
     TOKLOGIN WORD
     {
        printf("Login %s\n", $2);
     }
     ;

play:
    TOKPLAY NUMBER
    {
        printf("Play %d\n", $2);
    }
    ;

msg:
   TOKMSG string
   {
        printf("Message %s\n", $2);
   }
   ;

logout:
      TOKLOGOUT string
      {
        printf("Logout %s\n", $2);
      }
      ;

string:
      /* empty: */
      {
        $$ = "";
      }
      | string WORD
      {
        if (*$$)
            $$ = strappend($$, $2);
        else
            $$ = $2;
      }
      ;
