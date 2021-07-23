// interface --- vocRev.h
// 21/10/15
#ifndef VOCREV_H_
#define VOCREV_H_

#include <stdbool.h>
#include <stdlib.h>

#define MAX			10000
#define WORDLEN		 32
#define POSLEN		  8
#define MEANLEN		256
#define TEST_ITEMS	 10
#define FAM           5
#define TIMEBUFF	 32

typedef struct
{
	char word[WORDLEN];
	char partOfSpeech[POSLEN];
	char meaning[MEANLEN];
	char timeOfRecord[TIMEBUFF];
	unsigned short familiarity;
	unsigned short unfamiliarity;
}Entry;

typedef struct node
{
	Entry entry;
	struct node *left;
	struct node * right;
}Node, *pNode;

struct record
{
    unsigned short totalSaved;
    unsigned short mastered;
    unsigned short lastScore;
};

pNode createNode(Entry etnry);
pNode createList(Entry head);
unsigned short getLength(const pNode list);
bool listIsEmpty(const pNode list);
bool listIsFull(const pNode list);
pNode readFromFile(pNode list, FILE * records);
pNode addEntry(pNode list, Entry entry);
pNode findEntry(const pNode list, const char * wd, const char * pos);
void taskManager(pNode list, const char * str);
void modifyEntry(pNode pnode);
void deleteEntry(pNode pnode);
pNode selectEntriesForRevision(pNode list);
void printOneEntry(Entry entry, FILE * fp);
void printAllEntriesToFile(const pNode list, char * str, bool unfam);
void emptyAllEntries(pNode list);
void writeToFile(const pNode list, const char * str, FILE * fp, bool firstReadRW);
void getTime(char * timeBuffer, const unsigned len);
void getStr(char * str, const unsigned len);
void cutLine(char * str);
void eatLine(void);
void appendDot(char * str);

#endif
