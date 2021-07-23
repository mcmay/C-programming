// main -- interface to the user

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vocRev.h"
#include "menu.h"

#define NAMELEN    81

extern struct record rec;
void writeRecFile(const struct record * rec,  const char * str, FILE * fpRec, bool firstCreatedRec);

int main(void)
{
	FILE * fpRW, * fpRec;
	const char * str[] = {
		"1) Create a new entry",
		"2) Find an entry",
		"3) Select entries for revision",
        "4) Delete an entry",
        "5) Save all changes",
        "6) Modify an entry",
		"7) Print all recorded entries",
		"8) Print all unfamiliar entries",
		"9) Save & Exit",
		"0) Exit without save"
	};
    const char * inititals = "1234567890";
	int choice;
    Entry head = {
        "notaword",
        "nopart",
        "nomeaning",
        "innotime",
        0,
        0
    };
	pNode list = createList(head);
	bool firstCreatedRW = false, firstCreatedRec = false;
    bool saveFlag = false; // determine last choice is/isn't Save
    bool loop = true;
    bool unfam = false;
    char userEnt[NAMELEN];
    char userRec[NAMELEN];
    char userTxt[NAMELEN];
    
    printf("Enter the initials of your name:\n");
    getStr(userEnt, NAMELEN);
    strcpy(userRec, userEnt);
    strcpy(userTxt, userEnt);
    strcat(userEnt, ".ent");
    if((fpRW = fopen(userEnt, "rb")) == NULL)
    {
    	fputs("Error reading input file. Creating a new input file.\n", stderr);
    	fpRW = fopen(userEnt, "wb");
    	if(fpRW == NULL)
    	{
    		fputs("Error creating input file.", stderr);
    		exit(EXIT_FAILURE);
    	}
        firstCreatedRW = true;
    }
    strcat(userRec, ".rec");
    if((fpRec = fopen(userRec, "rb")) == NULL)
    {
    	fputs("Error reading recorder file. Creating a new recorder file.\n", stderr);
    	fpRec = fopen(userRec, "wb");
    	if(fpRec == NULL)
    	{
    		fputs("Error creating input file.", stderr);
    		exit(EXIT_FAILURE);
    	}
        firstCreatedRec = true;
    }
    if(firstCreatedRec == false)
    {
        fread(&rec, sizeof(struct record), 1, fpRec);
        fclose(fpRec);
    }
    if(rec.lastScore != 0)
        printf("Your score in last revision is %u\n", rec.lastScore);
    if(rec.mastered != 0)
        printf("Congratulations! You have mastered %u words.\n", rec.mastered);
    else
        printf("No words have been mastered.\n");
   if(rec.totalSaved != 0)
        printf("There are still %u words to be reviewed. Hurry up!\n", rec.totalSaved);
    rec.totalSaved = 0; // start anew from zero before reading from file
	if(firstCreatedRW == false)
    {
        list = readFromFile(list, fpRW);
        fclose(fpRW);
    }
	while(loop)
	{
        printMenu(str, sizeof str/ sizeof str[0]);
        choice = getChoice(inititals);
        if(choice == '`')
        {
            fprintf(stderr, "End of file received from the standard input.\n");
            printf("Choice set to '9'\n");
            choice = 'E';
        }
        switch(choice)
        {
			case '1':
                taskManager(list, "add to the bank");
                saveFlag = false;
				break;
			case '2':
                taskManager(list, "find");
				break;
			case '3':
			    list = selectEntriesForRevision(list);
                saveFlag = false;
				break;
            case '4':
                taskManager(list, "delete");
                saveFlag = false;
                break;
            case '5':
                writeToFile(list, userEnt, fpRW, firstCreatedRW);
                writeRecFile(&rec, userRec, fpRec, firstCreatedRec);
                saveFlag = true;
                break;
            case '6':
                taskManager(list, "modify");
                saveFlag = false;
                break;
			case '7':
				printAllEntriesToFile(list, userTxt, unfam);
				break;
            case '8':
                unfam = true;
                printAllEntriesToFile(list, userTxt, unfam);
                break;
            case '9':
                if(saveFlag == false)
                {
                    writeToFile(list, userEnt, fpRW,  firstCreatedRW);
                    writeRecFile(&rec, userRec, fpRec, firstCreatedRec);
                }
                loop = false;
                break;
                case '0':
                    loop = false;
                    break;
			             default:
				               printf("Invalid input.\n");
		}
	}
	emptyAllEntries(list);
	printf("See you next time!\n");

	return 0;
}

void writeRecFile(const struct record * rec,  const char * str,FILE * fpRec, bool firstCreatedRec)
{

    if(firstCreatedRec == false)
    {
        fpRec = fopen(str, "wb");
        if(fpRec == NULL)
        {
            fputs("Error creating input file.", stderr);
            exit(EXIT_FAILURE);
        }
    }
    if(rec->lastScore != 0)
        printf("\nLastscore: %u\n", rec->lastScore);
    if(fwrite(rec, sizeof (struct record), 1, fpRec) == 1)
        printf("Record has been written to file.\n");
    else
        printf("A write failure has been encountered.\n");
        fclose(fpRec);
}
