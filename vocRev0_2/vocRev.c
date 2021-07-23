// implementation --- vocRev.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "vocRev.h"
#include "menu.h"

struct record rec;

pNode createNode(Entry entry)
{
	pNode pnode = (pNode) malloc(sizeof(Node));

	if(NULL == pnode)
	{
		fprintf(stderr, "\nError allocating memory for an entry.\n");
		return NULL;
	}
	pnode->entry = entry;
	pnode->left = pnode;		// every new node is initialized by
	pnode->right = pnode;		// poiting left and right pointers to itself

	return pnode;
}

pNode createList(Entry head)
{
	pNode pnode = (pNode) malloc(sizeof(Node));

	if(NULL == pnode)
	{
		fprintf(stderr, "\nError allocating memory for an entry.\n");
		return NULL;
	}
	pnode->entry = head;
	pnode->left = pnode;
	pnode->right = pnode;

	return pnode;
}

unsigned short getLength(const pNode list)
{
	unsigned short count = 0;
	pNode current = list->right;


	while(current != list)
	{
		count++;
		current = current->right;
	}
	return count;
}

bool listIsEmpty(const pNode list)
{
	unsigned short len = getLength(list);
	return len == 0;
}

bool listIsFull(const pNode list)
{
	unsigned short len = getLength(list);
	return len == MAX;
}

pNode addEntry(pNode list, Entry entry)
{
	pNode pnode = createNode(entry);
	pNode temp = list->left; // ref

	list->left->right = pnode; // recover link from right to left
	pnode->right = list;

	list->left = pnode;			// keep new node's addr in head's left pointer
	pnode->left = temp;			// recover link from left to right, ref
    rec.totalSaved++;

	return list;
}

pNode readFromFile(pNode list, FILE * records)
{
	pNode pnode = (pNode) malloc(sizeof(Node));

	if(NULL == pnode)
	{
		fprintf(stderr, "\nError allocating memory for an entry.\n");
		return NULL;
	}
	if(listIsFull(list))
	{
		fprintf(stderr, "\nList is full.\n");
		exit(EXIT_FAILURE);
	}
	while(fread(pnode, sizeof(Node), 1, records) == 1)
		list = addEntry(list, pnode->entry);

	free(pnode);

	return list;
}

pNode findEntry(pNode list, const char * wd, const char * pos)
{
	pNode current = list->right;
	while(current != list)
	{
		if(strcmp(wd, current->entry.word) == 0 &&
			strcmp(pos, current->entry.partOfSpeech) == 0)
			break;
		current = current->right;
	}
	if(current == list)
		return NULL;

	return current;
}

void taskManager(pNode list, const char * str)
{
	char wd[WORDLEN];
	char pos[POSLEN];
	pNode found;
	Entry entry;

	printf("Enter the word you'd like to %s:\n", str);
    getStr(wd, WORDLEN);
    printf("Enter the part of speech of the word:\n");
    getStr(pos, POSLEN);
    appendDot(pos);
    found = findEntry(list, wd, pos);

    if(strcmp("find", str) == 0)
    {
    	if(found == NULL)
    		printf("The word \"%s %s\" is not in the word bank.\n",
                        wd, pos);
    	else
    	{
            printf("Found:\n");
            printOneEntry(found->entry, stdout);
        }
    }
    else if(strcmp("add to the bank", str) == 0)
    {
    	if(found == NULL)
    	{
    		strcpy(entry.word, wd);
	    	strcpy(entry.partOfSpeech, pos);
	    	printf("Enter the meaning:\n");
	    	getStr(entry.meaning, MEANLEN);
	    	entry.familiarity = 0;
	    	entry.unfamiliarity = 0;
	    	getTime(entry.timeOfRecord, TIMEBUFF);
	    	if((list = addEntry(list, entry)) == NULL)
	    		printf("Word bank is full.\n");
	    	else
	    	{
	    		printf("Entry has been added to the word bank.\n");
	            printf("Number of entires established up-to-date: %u\n", rec.totalSaved);
	    	}
    	}
    	else
    	{
    		printf("\"%s %s\" already exists in the word bank:\n",
    			found->entry.word, found->entry.partOfSpeech);
                printOneEntry(found->entry, stdout);
    	}

    }
    else if(strcmp("delete", str) == 0)
    {
    	if(found == NULL)
    		printf("Entry not found.\n");
        else
            deleteEntry(found);
    }
    else if (strcmp("modify", str) == 0)
    {
    	if(found == NULL)
    		printf("Entry not found.\n");
        else
            modifyEntry(found);
    }
    else
    	printf("Erroneous choice made.\n");
}

void modifyEntry(pNode pnode)
{
    const char * choices[] =
    {
        "1) modify the headword;",
        "2) modify the part of speech;",
        "3) modify the meaning;",
        "4) quit modification"
    };
    const char * inits = "1234";
    int ch;
    bool loop = true;

    printf("Entry found:\n");
    printOneEntry(pnode->entry, stdout);
    while(loop)
    {
        printMenu(choices, sizeof choices / sizeof choices[0]);
        ch = getChoice(inits);
        if('`' == ch)
        {
        	fprintf(stderr, "End of file encountered. Choice set to '4'.\n");
        	ch = '4';
        }
        switch (ch)
        {
            case '1':
                printf("Enter the modified headword:\n");
                getStr(pnode->entry.word, WORDLEN);
                break;
            case '2':
                printf("Enter the modified part of speech:\n");
                getStr(pnode->entry.partOfSpeech, POSLEN);
                break;
            case '3':
                printf("Enter the modified meaning:\n");
                getStr(pnode->entry.meaning, MEANLEN);
                break;
            case '4':
                loop = false;
                break;
            default:
               printf("Invalid input!\n");
        }
    }
    getTime(pnode->entry.timeOfRecord, TIMEBUFF);
}

void deleteEntry(pNode pnode)
{
	pNode temp = pnode->left;

	temp->right = pnode->right;
	pnode->right->left = temp;
	free(pnode);
	rec.totalSaved--;
	printf("The entry has been removed from the word bank.\n");
}

pNode selectEntriesForRevision(pNode list)
{

	if(listIsEmpty(list))
	{
		printf("No word has been added to the word bank.\n");
		return list;
	}
	unsigned short len;
	len = getLength(list);
	if(len < TEST_ITEMS)
	{
		printf("The number of words is smaller than %d.\n", TEST_ITEMS);
		printf("A revision requires that at least %d words should remain in the word bank.\n", TEST_ITEMS);
		return list;
	}
	unsigned short count;
	unsigned short index;
	bool tested;
	unsigned short testedEntryIndexes[TEST_ITEMS];

	// array of pointers to pNode
	pNode * arrPNodes = (pNode *)malloc(sizeof (pNode) * len);
	pNode current = list->right;

	for(count = 0; count < TEST_ITEMS; count++)
		testedEntryIndexes[count] = len; // impossible index of tested items

	count = 0;
	while(current != list && count < len)
	{
		arrPNodes[count] = current;
		count++;
		current = current->right;
	}
	count = 0;
	srand(time(0));
	rec.lastScore = 0;
	while(count < TEST_ITEMS)
	{
		tested = false;
		index = rand() % len; // 0 <= tested < len
		for(int i = 0; i < TEST_ITEMS; i++)
			if(testedEntryIndexes[i] == index)
			{
				tested = true;
				break;
			}
		if(tested)
			continue;
		testedEntryIndexes[count] = index;
		if(arrPNodes[index]->entry.familiarity < FAM)
			printf("\n%d: %s %s", count+1,
				arrPNodes[index]->entry.word, arrPNodes[index]->entry.partOfSpeech);
		int ch;
		printf("\nDo you know how to read it and what it means? (y/n)\n");
        while((ch = getchar()) != EOF && strchr("yn", tolower(ch)) == NULL)
        {
            printf("Enter 'y' or 'n' only.\n");
           	eatLine();
        }
        eatLine();
        if(ch == 'y')
        {
            arrPNodes[index]->entry.familiarity += 1;
            if(arrPNodes[index]->entry.familiarity >= FAM)
            {
                deleteEntry(arrPNodes[index]);
                rec.mastered++;
            }
            if(arrPNodes[index]->entry.unfamiliarity > 0)
                arrPNodes[index]->entry.unfamiliarity -= 1;
            rec.lastScore += 10;
        }
        else
        {
            printOneEntry(arrPNodes[index]->entry, stdout);
            puts("Please try to remember it!");
            arrPNodes[index]->entry.unfamiliarity += 1;
        }
        count++;
    }
    printf("Your score in this test is %u\n", rec.lastScore);
    free(arrPNodes);
    arrPNodes = NULL;
    return list;
}

void printOneEntry(Entry entry, FILE * fp)
{
	 fprintf(fp, "%s %s %s\n", entry.word,
                 		       entry.partOfSpeech,
                        	   entry.meaning);
     fprintf(fp, "Familiarity: %u\n", entry.familiarity);
     if(entry.unfamiliarity > FAM)
     {
     	fprintf(fp, "Unfamiliarity: %u\n", entry.unfamiliarity);
     	fprintf(fp, "Your unfamiliarity with \"%s\" is greater than %d.\n", entry.word, FAM);
     	fprintf(fp, "You should try harder to master it.\n");
     }
     fprintf(fp, "Time of entry: %s\n", entry.timeOfRecord);
}

void printAllEntriesToFile(const pNode list,  char * str, bool unfam)
{
	if(listIsEmpty(list))
    {
        printf("No entry has been added to the word bank.\n");
        return;
    }
	FILE * fp;
    pNode current = list->right;
     unsigned short unfamCount = 0;
    
    if(unfam)
         strcat(str, "AllunfamEntries.txt");
    else
        strcat(str, "AllEntries.txt");

    if((fp = fopen(str,  "w")) == NULL)
    {
        fputs("Error writing file.\n", stderr);
        exit(EXIT_FAILURE);
    }
    while(current != list)
    {
    	if(unfam)
    	{
    		if(current->entry.unfamiliarity >= 5)
            {
                unfamCount++;
                printOneEntry(current->entry, fp);
            }
    	}	// here we can't use if(unfam && current->entry.unfamiliarity >= 5)
        else // because else would mean unfam == true but current->entry.unfamiliarity < 5
            printOneEntry(current->entry, fp);
        current = current->right;
    }
     if(unfam)
        {
            if(unfamCount != 0)
                printf("All unfamiliar entries have been printed to the text file named \"%s\"\n", str);
            else
                printf("There are no unfamiliar words in the word bank.\n");
        }
        else
                printf("All entries have been printed to the text file named \"%s\"\n", str);
    fclose(fp);
}

void emptyAllEntries(pNode list)
{
	pNode save;
	pNode current = list->right;

    if(listIsEmpty(list))
    {
        printf("No word has been added to the word bank.\n");
        return;
    }
    while(current != list)
    {
        save = current;
        current = current->right;
        free(save);
    }
}

void writeToFile(const pNode list, const char * str, FILE * fp, bool firstCreatedRW)
{
	if(listIsEmpty(list))
	{
		printf("No word has been recorded.\n");
		return;
	}
	pNode current = list->right;
	if(firstCreatedRW == false)
		if((fp = fopen(str,  "wb")) == NULL)
		{
			fprintf(stderr, "Error writing to file.\n");
			exit(EXIT_FAILURE);
		}
	while(current != list)
	{
		fwrite(current, sizeof(Node), 1, fp);
		current = current->right;
	}
	fclose(fp);
	printf("All entries have been saved to the word bank.\n");
}

void getStr(char * str, const unsigned len)
{
	fgets(str, len, stdin);
    cutLine(str);
}

void getTime(char * timeBuffer, const unsigned len)
{
	time_t curTime;
	struct tm * localTime;

	curTime = time(NULL);
	localTime = localtime(&curTime);
	strncpy(timeBuffer, asctime(localTime), len);
}

void cutLine(char * str)
{
	char * pstr = str;

	while(*pstr)
	{
		if(*pstr == '\n')
		{
			*pstr = '\0';
			break;
		}
		pstr++;
	}
}

void eatLine(void)
{
	while(getchar() != '\n')
		continue;
}

void appendDot(char * str)
{
    char * pstr = str;

    while(*pstr)
    {
        if(*pstr == '.')
            break;
        pstr++;
    }
    if(*pstr == '\0')
        strcat(str, ".");
}