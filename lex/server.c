#include <string.h>
#include <stdlib.h>

int main(void)
{
    yyparse();
}

char *strappend(char *s1, char *s2)
{
    char *tmp = malloc(strlen(s1) + strlen(s2));
    strcpy(tmp, s1);
    strcat(tmp, " ");
    strcat(tmp, s2);
    free(s1);
    return tmp;
}

