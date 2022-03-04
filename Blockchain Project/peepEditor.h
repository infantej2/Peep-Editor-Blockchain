/*------------------------------------------------------- 
# Students/Group: Damion Shillinglaw, Jethro Infante, Filip Dosenovic
# Assignment #3  
# Lab Section: X01L 
# CMPUT 201 
# Instructor's Name: Nesrine Abbas 
*------------------------------------------------------*/

#ifndef PEEPEDITOR_H
#define PEEPEDITOR_H

#include "blockChain.h"

/* edits the peep, saves transactions in modBuffer, and returns the number of transactions written in th ebuffer
 * Pre: Must allocate space for modBuffer (Assumed size of 64 for M2), and for peep (size 256)
 * Parameters: peep <- dynamic string
 *             modBuffer <- Contains struct typedef Transaction objects; will keep track of modif. to peep
 * Post: changes to peep and modBuffer
 * Return: number of transactions made out during a session
 */
unsigned int editor(char * peep, TRANSACTION * modBuffer );
#endif
