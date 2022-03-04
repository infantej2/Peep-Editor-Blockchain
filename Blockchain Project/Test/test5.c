/*-----------------------------------------------------------------
# Student's Name: Jethro Infante
# Assignment 3 Testing
# Lab Section: X01L, Fall 2021
# CMPUT 201, Fall 2021
# Lab Instructor's Name: Nesrine Abbas


Purpose:
*----------------------------------------------------------------*/
#include <stdio.h>
#include "../blockChain.h"
#include "../peepEditor.h"

int main() {
    int n; //Number of transactions
    char *peep = calloc(MXTXNUM, sizeof(char));
    TRANSACTION *modBuffer = malloc(sizeof(TRANSACTION) * MXTXNUM);
    n = editor(peep,modBuffer);
    printf("\nFinal number of transactions after session: %d\n",n);
    free(modBuffer);
    free(peep);
    return 0;
}
