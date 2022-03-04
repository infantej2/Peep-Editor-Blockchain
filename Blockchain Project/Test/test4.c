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

   //---------------- TESTING VVERIFY BLOCK CHAIN ------------------
   printf("\n\n\n------------TESTING VERIFY BLOCK CHAIN------------\n\n");
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

    int n1; //Number of transactions
    char *peep1 = calloc(256, sizeof(char));
    TRANSACTION *modBuffer1 = malloc(sizeof(TRANSACTION) * MXTXNUM);
    n1 = editor(peep1,modBuffer1);
    printf("\nFinal number of transactions after session: %d\n",n1);

    addTransactions(modBuffer1, n);
    printTransactions(first_link);

    free(modBuffer);
    free(peep);
    free(modBuffer1);
    free(peep1);

    char *peep2 = calloc(256, sizeof(char));
    getPeep(first_link,peep2);
    printf("%s\n", peep2);
    free(peep2);
    

    printf("\nVerifying block chain\n");
    int result;
    if((result = verifyBlockChain(first_link)) == 1){
        printf("Block chain has been compromised\n\n");
    } else {
        printf("Block chain has been verified\n\n");
    }

	printf("Create a compromise in the first block's block size\n");
    printf("Before: %d\n", first_link->block.blockSize);
    first_link->block.blockSize = 50;
    printf("After: %d\n", first_link->block.blockSize);


    writeBlockChain(first_link, "trythis.dat");
    LINK * temp = readBlockChain("trythis.dat");
    printTransactions(temp);

    char * peep3 = calloc(256, sizeof(char));
    getPeep(temp,peep3);
    printf("%s\n", peep3);
    free(peep3);


     printf("\nVerifying block chain\n");
    result = 0;
    if((result = verifyBlockChain(temp)) == 1){
        printf("Block chain has been compromised\n\n");
    } else {
        printf("Block chain has been verified\n\n");
    }


    LINK  *cur=first_link, *next=NULL;
    while(1){
        if(cur==NULL) break;
        next = cur->next;
        free(cur);
        cur = next;
    }

	return 0;
}
