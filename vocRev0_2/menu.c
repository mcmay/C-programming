// menu.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "menu.h"

int longestStr(const char *str[], int n)
{
	int longest = 0;

	for(int i = 1; i < n; i++)
		if(strlen(str[i]) > strlen(str[longest]))
			longest = i;
	return strlen(str[longest]);
}
void printStars(int n)
{
    putchar('\n');
	for(int i = 0; i < n; i++)
		putchar('*');
	putchar('\n');
}
void printMenu(const char *str[], int n)
{
	int longest = longestStr(str, n);
	printStars(longest);
	for(int i = 0; i < n; i++)
		printf("%s\n", str[i]);
	printStars(longest);
	printf("Your choice: ");
}

int getChoice(const char * s)
{
	int ch;

	ch = getchar();
	ch = toupper(ch);
	while(ch != EOF && strchr(s, ch) == NULL)
	{
		printf("Please make sure the choice you entered is within %s\n", s);
		printf("Enter again please: ");
		while(getchar() != '\n')
			continue;
		ch = getchar();
		ch = toupper(ch);
	}
	while(getchar() != '\n')
			continue;
	if(ch == EOF)
		return '0';

	return ch;
}
