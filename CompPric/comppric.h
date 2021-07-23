#ifndef COMPPRIC_H_
#define COMPPRIC_H

#define MAX         5000
#define BCLEN    	16
#define NAMELEN  	32
#define BRANDLEN 	32
#define SNLEN    	16
#define TIMEBUF	 	32
#define SPECDATELEN 16
#define UNITLEN		16

#include <stdbool.h>

typedef struct
{
	char barCode[BCLEN];
	char name[NAMELEN];
	char brand[BRANDLEN];
	char shopName[SNLEN];
	char unit[UNITLEN];
	double quantity;
	double regPrice;
	double specPrice;
	char specPriceStartDate[SPECDATELEN];
	char specPriceEndDate[SPECDATELEN];
	char timeBuffer[TIMEBUF];
}Item;

typedef struct node
{
	Item item;
	struct node * prev;
	struct node * next;
}Node, * pNode;

extern void setOption(int * ch, const char set);
extern void getStr(char * str, const int len);

bool listIsFull(const pNode nd);
bool listIsEmpty(const pNode nd);
unsigned getLength(pNode nd);
pNode createNode(Item item);
pNode createList(Item head);
pNode insertNode(pNode nd, Item item);
pNode findItem(const pNode nd, const char * bc, const char * sn, const double qty);
bool findRelatedItem(const pNode nd, const char * bc, const double qty);
void modifyItem(Item * pItem);
void deleteNode(pNode nd, pNode fn);
pNode readFromFile(pNode nd, FILE * fp);
void writeToFile(const pNode nd, FILE * fp);
void printListToFile(const pNode nd);
void printSingleItem(const Item item, FILE * fp);
bool deleteList(pNode nd);
void getTime(char *);
double getNumber(void);
void cutLine(char *);
void eatLine(void);

#endif