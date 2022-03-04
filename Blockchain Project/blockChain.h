/*-----------------------------------------------------------------
# Student's Name: Filip Dosenovic, Jethro Infante, Damion Shillinglaw
# Assignment 3 blockChain.h
# Lab Section: X01L, Fall 2021
# CMPUT 201, Fall 2021
# Lab Instructor's Name: Nesrine Abbas
*----------------------------------------------------------------*/

#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <openssl/evp.h>
#define MXTXNUM 64 
#define PEEPLEN 256


/*

DATA STRUCTURES

Structure to Represent a modification [2 bytes]. (modification)
	position	- Position in the peep that the event occured: Maximum position of 256
	character	- The character that was modified  - ASCII codes 32 to 127
     event_type	- Type of event that has occured.
				  0 for Insert
				  1 for Delete
NOTE: Bitfields must be used to condense data in memory and when written
*/
typedef struct{
    unsigned short position: 8;
    unsigned short character: 7;
    unsigned short event_type: 1;
}MODIFICATION;


/*
Structure to Represent a Transaction [8 bytes] (transaction) 
	timestamp	- Time that the event occurred (EPOCH time since January 1-st 2021). [6 bytes]
	modification	- The modification part of the transaction.                          [2 bytes]
*/
typedef struct{
    time_t timestamp: 48;
    MODIFICATION mod;
}TRANSACTION;


/*
Structure to Represent a Block  [32 bytes for the  head + up to 8*64 bytes for transactions] (block)
	previousHash	- Hash of the Previous block head. 0 if first          [7 bytes]
	timeStamp		- Time that the hashing of the block occured           [6 bytes]
	blockNum		- Incremental number of the block.                     [4 bytes]
	blockSize		- Size of transaction elements in the block            [1 byte]
	blockHash		- The Merkle tree hash of the transactions             [7 bytes]
                                
                                
	headHash		- Hash of the head elements above                      [7 bytes]
      
	transactions	- Transactions contained in the block (maximum of 64 transactions)

NOTE: a hash of length 7 bytes is obtained by taking the last 7 bytes of the sha256 hash 
      The truncation is to be applied only as the final step. 
      All intermediary hashes are full sha256 hashes

*/
typedef struct{
    unsigned char previousHash[7];
    time_t timeStamp: 48;
    unsigned int blockNum;
    unsigned char blockSize;
    unsigned char blockHash[7];
    unsigned char headHash[7];
    TRANSACTION transactions[MXTXNUM];
}BLOCK;

/*
Structure to represent a linkage of blocks. 
	block    -Object that stores data from the block struct above
	next     -Pointer to next block in block chain
*/
typedef struct link {
    struct link * next;
    /*pointer to the next node*/
    BLOCK block;
}LINK;


extern LINK *last_link; //Global variable set in client file

//FUNCTIONS

//writeBlockChain - writes a block chain to a file
void writeBlockChain(LINK * bc, char * filename);

//readBlockChain - reads a block chain from a file
LINK *readBlockChain(char * filename);

//verifyBlockChain - verifies the integrity of the block chain
int verifyBlockChain(LINK * bc);

/*Add new block (link) to block chain; conversely, start a new block chain by setting the first block.
 * A new block is added in one of two situations: 1) a session ends where at most 64 trans. are placed
 *                                                   in buffer
 *                                                2) During a session, user hits 64 trans. causing the
 *                                                   editor to flush the buffer and record them on ledger
 * Parameters: modBuffer <-- fill with struct typedef TRANSACTIONS during or after a session
 *             numTrns <-- count of trans. set in modBuffer
 * Pre: modBuffer is at MOST 64 trans.
 * Post: global variable 'last_link' is updated with lastest block/link in block chain
 * Return: None
 */
void addTransactions(TRANSACTION * modBuffer, int numTrns);

//printTransactions - prints all transactions contained in the blockchain
void printTransactions(LINK * bc);

//getPeep - produces the current peep by replaying all the transactions contained in the block chain
void getPeep(LINK * bc, char * peep);

//getPeepAtTime - produces the peep at a certain time stamp by replaying all the transactions in the 
//block chain up to (and including) the time stamp
void getPeepAtTime(LINK * bc, char * peep, int time);
#endif

