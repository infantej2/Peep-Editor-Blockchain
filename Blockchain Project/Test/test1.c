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

   //--------- TESTING GET PEEP AND GET PEEP AT TIME ----------
    printf("\n\n------------TESTING GET PEEP AND GET PEEP AT TIME------------\n\n");

    MODIFICATION *mod1 = malloc(sizeof(MODIFICATION));
	TRANSACTION *transList1 = malloc(sizeof(TRANSACTION) * 5);

	mod1->position = 1;
	mod1->character = 'a';
	mod1->event_type = 0;
	transList1[0].mod = *mod1;
	transList1[0].timestamp = 0;

	mod1->position = 2;
	mod1->character = 'b';
	mod1->event_type = 0;
	transList1[1].mod = *mod1;
	transList1[1].timestamp = 1;

	mod1->position = 3;
	mod1->character = 'c';
	mod1->event_type = 0;
	transList1[2].mod = *mod1;
	transList1[2].timestamp = 2;

	mod1->position = 3;
	mod1->event_type = 1;
	transList1[3].mod = *mod1;
	transList1[3].timestamp = 3;

	BLOCK *b1 = malloc(sizeof(BLOCK));
	
	b1->blockSize = 0;
	b1->transactions[0] = transList1[0];
	b1->blockSize++;
	b1->transactions[1] = transList1[1];
	b1->blockSize++;
	b1->transactions[2] = transList1[2];
	b1->blockSize++;
	b1->transactions[3] = transList1[3];
	b1->blockSize++;

	LINK *l1 = malloc(sizeof(LINK));
	l1->next = NULL;
	l1->block = *b1;
	l1->next = NULL;
	printTransactions(l1);

	char *peep = calloc(256, sizeof(char));
	getPeep(l1, peep);
	printf("Replaying peep: %s\n", peep);

	char *peep1 = calloc(256, sizeof(char));
	getPeepAtTime(l1,peep1,2);
	printf("Replaying peep up to time stamp: %s\n", peep1);

	return 0;
}
