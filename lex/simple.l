%{
#include <stdio.h>
%}

%%
^[A-Z]+     fputs("COMMAND ", stdout);
-?[0-9]+    fputs("INT ", stdout);
[^ \t\n]+   fputs("WORD ", stdout);
\n          fputs("NEWLINE\n", stdout);
[ \t]+      /* ignore */
%%
