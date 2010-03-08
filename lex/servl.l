%{
/* generated from server.l */

#include <stdio.h>
#include <string.h>
#include "y.tab.h"
%}
%%
[0-9]+      yylval.num = atoi(yytext); return NUMBER;
LOGIN       return TOKLOGIN;
PLAY        return TOKPLAY;
LOGOUT      return TOKLOGOUT;
MSG         return TOKMSG;
\n          return NEWLINE;
[^ \t\n]+   yylval.str = strdup(yytext); return WORD;
[ \t]       /* ignore whitespace */
%%
