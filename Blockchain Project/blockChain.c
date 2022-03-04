/*-----------------------------------------------------------------
# Student's Name: Jethro Infante
# Lab 7 Exercise 3
# Lab Section: X01L, Fall 2021
# CMPUT 201, Fall 2021
# Lab Instructor's Name: Nesrine Abbas

Purpose: From lab specs: this is a program which takes the name of
a file as a command line argument and then produces the SHA256
digest of the content of the file concatenated with the current
timestamp (in seconds).
*----------------------------------------------------------------*/

#include "blockChain.h"

#define DIGESTLEN 32 //32 bytes  
#define CONCAT_HASHES(start,end,h1,h2) for(i=0;i<end;i++) h1[start+i] = h2[i];
#define TRIM_HASH(h1,h2) for(i=25,j=0;j<7;i++,j++) h1[j] = h2[i]; //get last 7 bytes from h2
#define PRINT_HASH(hash,end) for(i=0;i<end;i++) printf("%x",hash[i]);

LINK *last_link;

/*Depending on the type of data passed, more on this bellow after 'Pre:', a digest (hash) is
 * returned of data1 or data2.
 * Parameters: data1 <-- an array of unsigned chars
 *             data2 <-- pointer to struct typedef TRANSACTION 
 *             size <-- refers to the size of data1; pass 0 when hashing data2
 * Pre: if unsigned char hashing then pass NULL as second argument when invoking, else pass NULL
 *      for first argument when trans. hashing
 * Post: None
 * Return: a dynamic, unsigned char pointer to digest
 */
unsigned char *digestData(unsigned char *data1,TRANSACTION *data2,int size) {
    //creates the variable digest_len which will hole the length of the sha256 digest
    unsigned int digest_len = EVP_MD_size(EVP_sha256());
    //the variable digest will hold the final digest (hash)
    unsigned char *digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()));
    //Declare the message digest context variable
    EVP_MD_CTX * mdctx;
    //create a message digest context
    mdctx = EVP_MD_CTX_create();
    //initialise the context by identifying the algorithm to be used
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);

    //Update the digest with data
    if(data1 != NULL){ //Hashing a unsigned chars
        EVP_DigestUpdate(mdctx, data1, size);
        //finalize digest
        EVP_DigestFinal_ex(mdctx, digest, &digest_len);
    }else{ //Hashing a transaction
        EVP_DigestUpdate(mdctx, data2,sizeof(TRANSACTION));
        EVP_DigestFinal_ex(mdctx, digest, &digest_len);
    }

    //clean up the context
    EVP_MD_CTX_destroy(mdctx);
    //int i;
    //PRINT_HASH(digest,32);
    //printf("\n");
   
    return digest;
}

/*Utility function for getMerkleRoot, that takes the elements (hashes of trans.) 
 * of one array (source) and copies them into another (destination). This is in 
 * connection to splitting up the transactions into two separate arrays.
 * Post: destArray is updated 
 */
void copyIntoArray(unsigned char sourceArray[][DIGESTLEN],unsigned char destArray[][DIGESTLEN],
        int start,int end){
    int i,j=0;
    for(;start<end;start++,j++) CONCAT_HASHES(0,DIGESTLEN,destArray[j],sourceArray[start]);
}

/*Utility function for getBlocHash, getBlocHash invokes this function as 
 * a preliminary step for getting the value 'order', given that getMerkleRoot has a parameter 'order'. 
 * Parameters: sz <-- refers to the size of an array of struct typedef TRANSACTIONs' (defined 
 *             in blockChain.h)
 * Post: None
 * Return: order refers to the depth (rows) of the binary tree defined in the merkle algorithm,
 *         where order 1 refers to the leaves of the binary tree
 */
int getOrder(int sz){
    int order = 1;
    if(sz==2 || sz==1) return order;
    int result = sz;
    while(1){
        if(result%2!=0) result++; //result is odd
        result = result/2;
        order++;
        if(result == 2) break;
    }
    return order;
}
/*A recursive function that implements the merkle algorithm: root is the result of hashing trans.
 * Utility function for getBlocHash.
 * Parameters: mainArray <-- an array of hashes of transactions with at most 64
 *             size <-- number of hashes in mainArray
 *             order <-- refers to the depth of binary tree
 * Pre: before first call, mainArray MUST contain hashes of all set trans. in a block;
 *      call getOrder to get the depth of binary tree
 * Return: newHash <-- if orignal call then ptr to root; else, for all subsequent calls, 
 *                     a ptr to hash of two transactions is returned
 */
unsigned char* getMerkleRoot(unsigned char mainArray[MXTXNUM][DIGESTLEN],int size, int order){
    unsigned char tempHash[DIGESTLEN*2]; //Will contain two hashes, concatenated
                                        //These two hashes are hashes of trans'.
    unsigned char *newHash;
    int i;

    if(size%2 != 0){ //True: odd number of transactions
        //Add new trans. by copying the last trans.
        CONCAT_HASHES(0,DIGESTLEN,mainArray[size],mainArray[size-1]);
        size++;
    }
    
    if(order == 1){ //Base case: realized a leaf in binary tree (mainArr ONLY has 2 trans.)
        CONCAT_HASHES(0,DIGESTLEN,tempHash,mainArray[0]);
        CONCAT_HASHES(32,DIGESTLEN,tempHash,mainArray[1]);
        newHash = digestData(tempHash,NULL,64);
        return newHash;
    }else{ //Decompose mainArray; split array in half; two recursive calls where
           //each take the halfs'
        int halfSZ = size/2,newOrder = order-1;
        unsigned char *hash1,*hash2; 
        unsigned char arrayA[halfSZ][DIGESTLEN], arrayB[halfSZ][DIGESTLEN]; 
        
        //split mainArray in half
        copyIntoArray(mainArray,arrayA,0,halfSZ);  
        copyIntoArray(mainArray,arrayB,halfSZ,size);
        
        //Invoke two recursive calls for binary tree
        hash1 = getMerkleRoot(arrayA,halfSZ,newOrder); //arrayA is lower half
        hash2 = getMerkleRoot(arrayB,halfSZ,newOrder); //arrayB is upper half
        
        //Take hashes from order-1: 1)concatenate 2)hashing
        CONCAT_HASHES(0,DIGESTLEN,tempHash,hash1);
        CONCAT_HASHES(32,DIGESTLEN,tempHash,hash2);
        newHash = digestData(tempHash,NULL,64);
        free(hash1); free(hash2);
    }
    
    return newHash; 
}

/*Return: 0 means success*/
int isMemoryAvailable(void * ptr){
    if(!ptr){
        fprintf(stderr,"Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }else return 0;
}

/*Attain hash of head elements of a block.
 * Parameters: headHash <-- pointer to a static array of unsigned chars
 *             b <-- a memember of a struct typedef link
 * Pre: all members in b must be set first; headHash MUST be 7 bytes;
 *      call getBlocHash before this function to attain blocHash for b
 * Post: headHash will point to hash of head elements 
 * Return: None
 */
void getHeadHash(unsigned char *headHash, BLOCK block) {
    int i,i2,i3,j; //indexing declarations
    unsigned char * finalHash;
    unsigned char *data = malloc(7); //data <-- will store all values in 
                                     //block by concatenating them, then
                                     //data will be hashed.
                                     //Initially, data is size 7 in prep.
                                     //for block's previousHash. 
    int size=0; //size of data

    
    //concat previousHash to data 
    CONCAT_HASHES(0,7,data,block.previousHash);
    size +=7;

    //Concat time stamp to data
    unsigned long long tempTime = block.timeStamp;
    unsigned char tempTimeArr[6];
    int count = 0;
    for(i2=0;i2<6;i2++){ //Extract 1 byte at a time from tempTime
                         //and store value into tempTimeArr
        if((tempTime & 0xff) == 0x0) break;
        size++;
        tempTimeArr[i2] = tempTime & 0xff;
        count++;
        tempTime = tempTime >> 8;
    }
    data = realloc(data,size);
    isMemoryAvailable(data);
    for(i2=count-1,i3=0;i2>=0;i2--,i3++) data[7+i3] = 
        tempTimeArr[i2]; //Finally, using tempTimeArr set bytes in data 

    //concat block number to data
    unsigned int tempInt = block.blockNum;
    unsigned char tempBlocNumArr[4];
    count = 0;
    i2 = 0;
    while((tempInt & 0xff)!= 0x00){ //Extract 1 byte at a time from
                                   //tempInt and store value in tempBlocNumArr
        count++;
        size++;
        tempBlocNumArr[i2] = tempInt & 0xff;
        i2++;
        tempInt = tempInt >> 8;
    }
    data = realloc(data,size); 
    isMemoryAvailable(data);
    for(i2=count-1,i3=0;i2>=0;i2--,i3++) data[(size-count)+i3]
        = tempBlocNumArr[i2]; //Finally, using tempBlocNumArr set bytes in data

    //concat blockSize to data
    data = realloc(data,++size);
    isMemoryAvailable(data);
    data[size-1] = block.blockSize;
    
    //concat blockHash to data
    size += 7;
    data = realloc(data,size);
    isMemoryAvailable(data);
    CONCAT_HASHES(size-7,7,data,block.previousHash);

    //hash the concatenated data
    finalHash = digestData(data, NULL,size);
    TRIM_HASH(headHash,finalHash);
    free(finalHash);
    free(data);
    return; 
}


/*Attain the merkle root when provided a set of transactions, at 
 * most 64, from a block or session (call to peep editor).
 * Parameters: blockHash <-- pointer to a static array of unsigned chars
 *             arrTrans <-- a pointer to an array of struct typedef TRANSACTION 
 *                          (from blockChain.h) 
 *             size <-- Is the number of transactions in arrTrans
 * Pre: blockHash <-- MUST have size of 7 [7 bytes] 
 *      arrTrans <-- MUST be a pointer to an array of TRANSACTIONs that are
 *                   set. For example, if a block doesn't have 64 transactions, 
 *                   then don't pass transactions that aren't set; some trans.
 *                   can be thought of as NULL, and the others containing a mod. 
 * Post: blockHash will point to the last 7 bytes of merkle root 
 *       (used to define blocHash of BLOCK).
 * Return: None
 */
void getBlocHash(unsigned char *blockHash,TRANSACTION *arrTrans,int size){
    unsigned char arrDigest[MXTXNUM][DIGESTLEN]; //store hashes of trans.
    unsigned char *root,*tempHashTrans;
    int i,j;

    for(i=0;i<size;i++){ //fill arrDigest with hashes of each transaction
        tempHashTrans = digestData(NULL,arrTrans+i,0);
        for(j=0;j<DIGESTLEN;j++) arrDigest[i][j] = tempHashTrans[j];
        free(tempHashTrans);
    }
    
    //Set root and get last 7 bytes of root for blockHash
    root = getMerkleRoot(arrDigest,size,getOrder(size));
    TRIM_HASH(blockHash,root);
    free(root);
    return;
}

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
void addTransactions(TRANSACTION * modBuffer, int numTrns) {
    if(numTrns==0) return; //Nothing to record on ledger
    int i, j;
    time_t before_epoch = 1609484400; //Seconds passed from Jan. 1, 1970 to Jan. 1, 2021 (current epoch)

    //set members for block and add link to block chain
    if(!(last_link->block.timeStamp>0)){//True: 1st block in block chain;
                                        //timeStamp is the indicator

        last_link->block.timeStamp = time(NULL) - before_epoch; 
        last_link->block.blockSize = numTrns; 

        for (j=0;j<numTrns;j++) last_link->block.transactions[j] = modBuffer[j];
        getBlocHash(last_link->block.blockHash, modBuffer, numTrns);
        last_link->block.blockNum = 1; 
        last_link->next = NULL;

        for(j=0;j<7;j++) last_link->block.previousHash[j] = 0x00;
        getHeadHash(last_link->block.headHash, last_link->block);
    }else{ //create the new block; set members; add to block chain 
        LINK * newBC = malloc(sizeof(LINK));
        isMemoryAvailable(newBC);
        newBC->block.timeStamp = time(NULL) - before_epoch; 
        newBC->block.blockSize = numTrns; 

        for (j=0;j<numTrns;j++) newBC->block.transactions[j] = modBuffer[j];

        //set block hash 
        getBlocHash(newBC->block.blockHash, modBuffer, numTrns);
        last_link->next = newBC;
        newBC->block.blockNum = last_link->block.blockNum + 1;
        CONCAT_HASHES(0,7,newBC->block.previousHash,last_link->block.headHash);
        last_link = newBC;
        newBC->next = NULL; //Latest block in chain

        //Finally, set head hash
        getHeadHash(newBC->block.headHash, newBC->block);
    }
    return;
}

/*Take first link/block in block chain 
 * and cycle thru while freeing links
 */
void freeBlockChain(LINK *head){
    LINK *cur_link=head, *next_link;
    while(cur_link!=NULL){
        next_link = cur_link->next;
        free(cur_link);
        cur_link = next_link;
    }
    return;
}

/*Reads the provided file and recreates the block chain through linked list traversal.
 * Parameters: filename <-- the filename (char string) that will be read
 * Post: global variable 'last_link' is updated with lastest block/link in block chain
 * Return: the first block of the recreated block chain
 */
LINK * readBlockChain(char *filename){
        FILE *fp;
	fp = fopen(filename, "rb"); 
	if (fp == NULL) { //Check if file can be read
		fprintf(stderr, "Can't open file '%s'\n",filename);
		exit(1);
	}

        //Get number of blocks from file
        int nBlocks;
        fseek(fp,0,SEEK_END);
        nBlocks = (int) ftell(fp)/sizeof(BLOCK);
        fseek(fp,0,SEEK_SET);

        if(nBlocks==0){
            fprintf(stderr,"File contains no blocks.\n");
            fclose(fp);
            return NULL; 
        }

        //Build array of blocks
        BLOCK arrBlock[nBlocks];
        fread(arrBlock,sizeof(BLOCK),nBlocks,fp);
        fclose(fp);
        
        //Initialize block chain in memory as a set of 
        //struct typedef LINKS which model a linked-list 
        int i;
        LINK *head = malloc(sizeof(LINK));
        LINK *nextLink,*curLink;
        isMemoryAvailable(head);
        curLink = head;
        for(i=0;i<nBlocks;i++){
            if(!(i+1==nBlocks)){ 
                nextLink = malloc(sizeof(LINK));
                isMemoryAvailable(nextLink);
                curLink->block = arrBlock[i];
                curLink->next = nextLink;
                curLink = nextLink;
            }else{ //Hit the last link/block in block chain 
                curLink->block = arrBlock[i];
                curLink->next = NULL;
            }
        }

    return head;
}



// writeBlockChain - writes a block chain to a file
void writeBlockChain(LINK * head, char *filename)
{
        if(!(last_link->block.timeStamp>0)){ 
            freeBlockChain(head);
            return; //No link to add
        }

	FILE *fp = NULL;
	fp = fopen(filename, "wb");
        //Check if file is writable
	if (fp == NULL) {  
		fprintf(stderr, "Problem opening file\n");
		exit(1);
	}

        //Store blocks in arrBlock
        int i=0,nlinks = last_link->block.blockNum;
        BLOCK arrBlock[nlinks];
	LINK *cur_link=head;
        while(cur_link!=NULL){ 
            arrBlock[i] = cur_link->block;
            cur_link = cur_link->next;
        }
        
        //Write block chain to file;close file; free block 
	fwrite(arrBlock, sizeof(BLOCK), nlinks, fp); 
	fclose(fp);
        freeBlockChain(head);
}


/*This function takes the first block in the block chain, traverses through the block chain in a linked
 * list like implementation and prints out information in each block and each transaction regarding info
 * such as the time, position, character, and event type of each transaction.
 * Parameters: Link * bc <-- pointer the first block (head node)
 * Post: All transactions in the block chain will have all been printed
 * Returns: None
 */
void printTransactions(LINK * bc){
    LINK *head = bc;  
    printf("\nPrinting transactions now\n");

    int i, j;
    long int time;

    j = 1;
    //go through the whole block chain
    while (bc != NULL) {
        
        //Display what block we are currently in the block chain
        printf("Block %d of block chain\n", j);

        //go through all the transactions in the current block
        for (i=0;i<bc->block.blockSize;i++) {
            time = bc->block.transactions[i].timestamp;

            //if the event type was an insert
            if (bc->block.transactions[i].mod.event_type == 0){
                printf("Time: %ld    Position: %d    Character: %c   Type: Insert\n", time, 
                        bc->block.transactions[i].mod.position, bc->block.transactions[i].mod.character);
            
            //if the event type was a delete
            } else if (bc->block.transactions[i].mod.event_type == 1){
                printf("Time: %ld    Position: %d    Character: %-1c   Type: Delete\n", time, 
                        bc->block.transactions[i].mod.position,bc->block.transactions[i].mod.character);
            }
        }
        bc = bc->next; //move to the next block
        j++;
    }
    freeBlockChain(head);
    return;
}


/*This function takes the first block in the block chain and an empty peep(pointer to a string) and
 * will traverse through the whole block chain like a linked list implementation in order to recreate
 * the peep in a single string. This will insert and delete as provided by each transaction and will
 * concatenate the results of each block chain to the end of the peep.
 * Parameters: Link * bc <-- pointer the first block (head node)
 *             char * peep <-- pointer to the peep
 * Pre: pointer to the peep must be empty so that the results will be completely from the blockchain
 * Post: pointer to the peep will have the recreated results from the all the transactions in blockchain;
 *       All struct typdef links are freed
 * Return: None
 */
void getPeep(LINK * bc, char * peep){
    LINK *head = bc;
    printf("\nRecreating peep now\n");
    
    int i, peeplen, pos, j;
    char c, tempc;

    //go through the whole block chain
    while (bc != NULL) {

        peeplen = 0;
        char *tempeep = calloc(PEEPLEN, sizeof(char)); //create a temporary peep to put the current block's info in (buffer)
        
        //go through all the transactions in the current block
        for (i=0;i<bc->block.blockSize;i++) {
            
            peeplen = strlen(tempeep);
            pos = (bc->block.transactions[i].mod.position);
            j = 0;

            //perform actions here if transaction is insert
            if (bc->block.transactions[i].mod.event_type == 0) {
                c = bc->block.transactions[i].mod.character;
                
                //perform the insertion at the end
                if (pos == peeplen+1) {
                    tempeep[pos-1] = c;

                //perform the insertion at the given position
                } else {
                    j = pos - 1;
                    while(tempeep[j]!='\0') { //shift characters to the right after insert pos
                        tempc = tempeep[j];
                        tempeep[j] = c;
                        c = tempc;
                        j++;
                    }
                    tempeep[j] = c;
                }

            //perform actions here if transaction is delete
            } else if (bc->block.transactions[i].mod.event_type == 1) {
                for(j=pos-1;j<peeplen-1;j++){ //shift characters to left for the peep
                    tempc = *(tempeep+j+1); 
                    *(tempeep+j) = tempc;
                }
                *(tempeep+j) = '\0';
            }
        }
        //now we concatenate the temporary peep to our main peep
        strcat(peep,tempeep);
        free(tempeep);
        bc = bc->next; // move on to the next block
    }
    freeBlockChain(head);
    return;
}


/*This function takes the first block in the block chain and an empty peep(pointer to a string) and
 * will traverse through the whole block chain like a linked list implementation in order to recreate
 * the peep in a single string. This will insert and delete as provided by each transaction and will
 * concatenate the results of each block chain to the end of the peep. The difference between this and
 * getPeep() is that this function will only go up to (and including) the transaction at the timestamp
 * that it is given.
 * Parameters: Link * bc <-- pointer the first block (head node)
 *             char * peep <-- pointer to the peep
 *             int time <-- provided timestamp where the function will stop recreating the peep
 * Pre: pointer to the peep must be empty so that the results will be completely from the blockchain
 * Post: pointer to the peep will have the recreated results from the all the transactions in blockchain;
 *       All struct typdef links are freed
 * Return: None
 */
void getPeepAtTime(LINK * bc, char * peep, int time){
    LINK *head = bc;
    printf("\nRecreating peep now at timestamp\n");
    
    int i, peeplen, pos, j, flag;
    char c, tempc;

    flag = 0;

    //go through the whole block chain
    while (bc != NULL) {
        peeplen = 0;
        char *tempeep = calloc(PEEPLEN, sizeof(char)); //create a temporary peep to put the current block's info in (buffer)
        
        //go through all the transactions in the current block
        for (i=0;i<bc->block.blockSize;i++) {
            
            peeplen = strlen(tempeep);
            pos = (bc->block.transactions[i].mod.position);
            j = 0;

            //perform actions here if transaction is insert
            if (bc->block.transactions[i].mod.event_type == 0) {
                c = bc->block.transactions[i].mod.character;
                
                //perform the insertion at the end
                if (pos == peeplen+1) {
                    tempeep[pos-1] = c;

                //perform the insertion at the given position
                } else {
                    j = pos - 1;
                    while(tempeep[j]!='\0') { //shift characters to the right after insert pos
                        tempc = tempeep[j];
                        tempeep[j] = c;
                        c = tempc;
                        j++;
                    }
                    tempeep[j] = c;
                }

            //perform actions here if transaction is delete
            } else if (bc->block.transactions[i].mod.event_type == 1) {
                for(j=pos-1;j<peeplen-1;j++){ //shift characters to left for the peep
                    tempc = *(tempeep+j+1); 
                    *(tempeep+j) = tempc;
                }
                *(tempeep+j) = '\0';
            }

            //if we are at the timestamp then we break out of the loop
            if (bc->block.transactions[i].timestamp == time){
                printf("Found the time stamp\n");
                flag = 1; //this is a flag to find if it has been to the time stamp
                break;
            }
        }
        //now we concatenate the temporary peep to our main peep
        strcat(peep,tempeep);
        bc = bc->next; //move to the next block
        
        //found the time stamp so we gotta stop recreating
        if (flag == 1) {
            break;
        }
    }
    freeBlockChain(head);
    return;
}

/*This function takes the first block in the block chain, traverses through the block chain in a linked
 * list like implementation and find the block before the current (last block). Upon finding the second
 * last block we compare the headHash of the second last block to the prevHash of the last block. If the
 * values are the same then the block chain has been validated. Else, it has been compromised.
 * Parameters: Link * bc <-- pointer the first block (head node)
 * Post: Assurance of verification or a warning that the block chain has been compromised
 * Returns: None
 */
int verifyBlockChain(LINK * bc) {

    LINK * curr = bc, *prev = NULL;

    if (curr == NULL || curr->next == NULL) {
        printf("Nothing to verify.\n");
        return 0;
    }
    //traverse through linked list to find the second last block
    while(curr->next != NULL) {
        prev = curr;
        curr = curr->next;
        //found the second last block
        if(curr->next == NULL) {

            getBlocHash(prev->block.blockHash, prev->block.transactions, prev->block.blockSize);
            getHeadHash(prev->block.headHash, prev->block);
            
            for (int i = 0; i<7; i++){//we gotta compare the hashes to each other
                //printf("previous: %x    current: %x\n", prev->block.headHash[i], curr->block.previousHash[i]);
                if(prev->block.headHash[i] != curr->block.previousHash[i]) return 1; //we've found a compromise
            }
        } else return 0;
    }
    return 0;
}

