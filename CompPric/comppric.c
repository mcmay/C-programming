// main()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comppric.h"
#include "menu.h"

void setOption(int * ch, const char set);
void getStr(char * str, const int len);
void getItem(Item * pItem);
void getBcSnQty(char * s1, int len1, char * s2, int len2, double qty);

extern double getNumber(void);

int main(void)
{
	const char * mainMenu[] =
	{
		"1) Add a record;",
		"2) Find a record;",
		"3) Modify a record;",
		"4) Delete a record;",
		"5) Print all records to a text file;",
		"6) Save all records;",
		"7) Exit"
	};
	const char * mainMenuInitials = "1234567";
	int option;
	Item head = {
		"null",
		"null",
		"null",
		"null",
		"1900",
		0.0,
		0.0,
		0.0,
		"1900",
		"1900"
	};
	Item temp;
	FILE * records;
	pNode list = createList(head);
	pNode found;
	char bc[BCLEN];
	char sn[SNLEN];
	double qtty = 0.0;
	bool emptyFile = false, Loop = true;

	records = fopen("records.rec", "rb");
	if (records == NULL)
	{
		fprintf(stderr, "\nError reading record file.\n Creating a new record file.\n");
		records = fopen("records.rec", "wb");
		if(records == NULL)
		{
			fprintf(stderr, "\nError creating record file.\n");
			exit(EXIT_FAILURE);
		}
		emptyFile = true;
	}
	if(emptyFile)
		printf("Record file is empty.\n");
	else
    {
        list = readFromFile(list, records);
        printf("There are %u records in the database.\n", getLength(list));
        fclose(records);
    }
	printMenu(mainMenu, sizeof (mainMenu) / sizeof (mainMenu[0]));
	option = getChoice(mainMenuInitials);
	if(option == '`')
		setOption(&option, '7');
	while(Loop)
	{
		switch(option)
		{
			case '1':
				getItem(&temp);
				found = findItem(list, temp.barCode, temp.shopName, temp.quantity);
				if(found == NULL)
				{
					if(findRelatedItem(list, temp.barCode, temp.quantity) == false)
						printf("No records about the same commodity in other shops found.\n");
					list = insertNode(list, temp);
					if(list != NULL)
						printf("The information about the commodity has been recorded.\n");
				}
				else
				{
					printf("\nThe commodity already exists in the records:\n");
					printSingleItem(found->item, stdout);
				}
				break;
			case '2':
				getBcSnQty(bc, BCLEN, sn, SNLEN, qtty);
				found = findItem(list, bc, sn, qtty);
				if(found == NULL)
					printf("The commodity does not exist in the records.\n");
				else
				{
					printf("\nThe commodity exists in the records:\n");
					printSingleItem(found->item, stdout);
				}
				break;
			case '3':
				getBcSnQty(bc, BCLEN, sn, SNLEN, qtty);
				found = findItem(list, bc, sn, qtty);
				if(found != NULL)
				    modifyItem(&found->item);
				else
				    printf("The commodity does not exist in the records.\n");
				break;
			case '4':
				getBcSnQty(bc, BCLEN, sn, SNLEN, qtty);
				found = findItem(list, bc, sn, qtty);
				if(found != NULL)
				    deleteNode(list, found);
				else
				    printf("The commodity does not exist in the records.\n");
				break;
			case '5':
				printListToFile(list);
				break;
			case '6':
				writeToFile(list, records);
				break;
		    case '7':
		        writeToFile(list, records);
		        deleteList(list);
		        Loop = false;
		        break;
			default:
				printf("\nInvalid input for the main menu.\n");
		}
		if(Loop != false)
		{
			printMenu(mainMenu, sizeof (mainMenu) / sizeof (mainMenu[0]));
        	option = getChoice(mainMenuInitials);
        	if(option == '`')
            	setOption(&option, '7');
		}
	}
	printf("\nBye!\n");
	return 0;
}

void setOption(int * ch, const char set)
{
	printf("End of file character encountered. Choice set to %c\n", set);
	*ch = set;
}

void getStr(char * str, const int len)
{
	fgets(str, len, stdin);
	cutLine(str);
}

void getItem(Item * pItem)
{
	int ch;
	double discount;
	const char * subMenu[] =
	{
		"1) gram",
		"2) kilogram",
		"3) mililiter",
		"4) liter",
		"5) pack",
		"6) each",
		"7) other"
	};
    const char * submenuInitials = "1234567";

	printf("Enter the information about the commodity:");
	printf("\nBar code:\n");
	getStr(pItem->barCode, BCLEN);
	printf("\nName:\n");
	getStr(pItem->name, NAMELEN);
	printf("\nBrand:\n");
	getStr(pItem->brand, BRANDLEN);
	printf("\nShop Name:\n");
	getStr(pItem->shopName, SNLEN);
	printf("\nUnit:\n");
	printMenu(subMenu, sizeof (subMenu) / sizeof (subMenu[0]));
	ch = getChoice(submenuInitials);
	if(ch == '`')
		setOption(&ch, '6');
	if(ch >= '1' && ch <= '6')
	{
		const char * temp = subMenu[ch-48-1];
		strcpy(pItem->unit, (temp + 3));
	}
	else
	{
		char unit[UNITLEN];
		printf("\nPlease specify the unit: ");
		getStr(unit, UNITLEN);
		strcpy(pItem->unit, unit);
	}
	printf("Quantity:\n");
	pItem->quantity = getNumber();
	printf("\nRegular Price:\n");
	pItem->regPrice = getNumber();
	printf("\nIs there a speical price or discount now (y/n)?\n");
	ch = getChoice("YN");
	if(ch == '`')
		setOption(&ch, 'N');
	if(ch == 'Y')
	{
		printf("Speical price or discount (s/d)?");
		ch = getChoice("SD");
		if(ch == '`')
			setOption(&ch, 'S');
		if(ch == 'S')
		{
			printf("\nSpeical Price:\n");
			pItem->specPrice = getNumber();
		}
		if( ch == 'D')
		{
			printf("Please enter the discount rate (e.g., 0.7):\n");
			while((discount = getNumber()) >= 1.0)
				printf("Invalid discount rate. Enter again:\n");
			pItem->specPrice = pItem->regPrice * discount;
		}
		printf("Special price or discount starts on (dd/mm/yy, e.g., 07/08/2015):\n");
		getStr(pItem->specPriceStartDate, SPECDATELEN);
		printf("Speical price or discount ends on (dd/mm/yy, e.g., 07/08/2015):\n");
		getStr(pItem->specPriceEndDate, SPECDATELEN);
	}
	getTime(pItem->timeBuffer);
}

void getBcSnQty(char * s1, const int len1, char * s2, const int len2, double qty)
{
	printf("Enter the bar code of the commodity:\n");
	getStr(s1, len1);
	printf("Enter the shop name:\n");
	getStr(s2, len2);
	printf("Enter the quantity:\n");
	qty = getNumber();
}
