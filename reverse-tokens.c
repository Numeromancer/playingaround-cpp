// sample input:	“abc 123 xyzzz”
// result:		“xyzzz 123 abc”

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

void
reverse_string_inplace
(char * str, char * back) {
    char * front = str;
    back--;
    while(front < back) {
        char tmp = *front;
        *front = *back;
        *back = tmp;
        front++; back--;
    }
}

char * find_next_space(char * str) {
	while(*str && *str != ' ')
		str++;
	return str;
}

void reverse_tokens_inplace(char * str) {
	char * end = str + strlen(str);
	char * tokenp = str;
        char * tokenend;
	do {
            tokenend = find_next_space(tokenp);
            reverse_string_inplace(tokenp, tokenend);
            if (tokenend < end) tokenp = tokenend + 1;
        } while(tokenend < end);
}

void reverse_tokens(char* str)
{
    reverse_string_inplace(str, str + strlen(str));
    reverse_tokens_inplace(str);
}

int
main(int argc, char ** argv) {
    char * p = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(p, argv[1]);
    reverse_tokens(p);
    printf("%s\n", p);
    free(p);
    return 0;
}
