/*-----------------------------------------------------------------
# Student's Name: Filip Dosenovic, Jethro Infante, Damion Shillinglaw
# Assignment 3 Testing
# Lab Section: X01L, Fall 2021
# CMPUT 201, Fall 2021
# Lab Instructor's Name: Nesrine Abbas


Purpose:
*----------------------------------------------------------------*/
#include <stdio.h>
#include "../blockChain.h"
#include "../peepEditor.h"
LINK *last_link = NULL;

int main(){
   
    //-------------- TESTING READ AND WRITE FOR ONLY ONE BLOCK --------------------
	printf("\n\n------------TESTING READ AND WRITE FOR ONLY ONE BLOCK------------\n\n");
    int i;
    LINK *first_link = malloc(sizeof(LINK)); //Dummy link for brand new block chain
                                             //Might set during session
    first_link->next = NULL;
    for(i=0;i<7;i++) first_link->block.previousHash[i] = 0;
    for(i=0;i<7;i++) first_link->block.blockHash[i] = 0;
    for(i=0;i<7;i++) first_link->block.headHash[i] = 0;
    first_link->block.timeStamp = 0;
    first_link->block.blockNum = 0;
    first_link->block.blockSize = 0;

    last_link = first_link;

    int n; //Number of transactions
    char *peep = calloc(256, sizeof(char));
    TRANSACTION *modBuffer = malloc(sizeof(TRANSACTION) * MXTXNUM);
    n = editor(peep,modBuffer);
    printf("\nFinal number of transactions after session: %d\n",n);

    addTransactions(modBuffer, n);
    printTransactions(first_link);

	free(modBuffer);
    free(peep);

	printf("\nWRITING BLOCKCHAIN\n");
	writeBlockChain(first_link, "tester.dat");

    printf("\nREADING BLOCKCHAIN\n");
	LINK * readLink = readBlockChain("tester.dat");

	printTransactions(readLink);  
    getPeep(readLink, peep);
    printf("Here is the peep from the file that was read: %s\n", peep);

	LINK *cur=first_link, *next=NULL;
    while(1){
        if(cur==NULL) break;
        next = cur->next;
        free(cur);
        cur = next;
    }

	return 0;
}
