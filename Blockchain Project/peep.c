/*------------------------------------------------------- 
# Students/Group: Damion Shillinglaw, Jethro Infante, Filip Dosenovic
# Assignment #3  
# Lab Section: X01L 
# CMPUT 201 
# Instructor's Name: Nesrine Abbas 
*------------------------------------------------------*/

#include "peepEditor.h"
#include "blockChain.h"
#include <unistd.h>
#include <getopt.h>
#include <math.h>
LINK *last_link = NULL; //Stores most recent block in block chain

/*Utility function for atTimePeep function*/
int strToNum(char *time){
    int l = strlen(time);
    int exponent = l-1;
    int total=0,i;
     
    for(i=0;i<l;i++){
        if(time[i]<'0' || time[i]>'9'){
            fprintf(stderr,"Invalid argument provided for -t option.\n");
            exit(0);
        }
        total += (time[i] - '0') * ((int) pow(10.0,(float)exponent)); 
        exponent--;
    }
    return total;
}

/*Utility function for initializing a peep with null characters for use*/
char *initializePeep(void){
    char *peep;
    peep = (char *) malloc(sizeof(char)*PEEPLEN);
    memset(peep,'\0',sizeof(char)*PEEPLEN);
    return peep;
}

/*Utility function for missUse function that tells a user how to use 
 * this program.
 */
void printUsage(){
    printf("Usage: ./peep [options]\n \
            Options:\n \
            \t-c [file] Name of output file for writing a new peep\n \
            \t-e [file] Name of input/output file for editing a peep\n \
            \t-v [file] Name of input file for verifying the \
integrity of an existing peep\n \
            \t-h [file] Name of input file for printing transaction \
history of an existing peep\n \
            \t-p [file] Name of input file for reproducing a current \
peep\n \
            \t-t [timestamp] Reproduce a peep at a given timestamp \
(used with -p)\n"); 
    return;
}

/*If there is an error in parsing the arguments then this function is
 * called.
 */
void missUse(void){
    printUsage();
    exit(0);
}

/*Purpose: create a new peep, and, after a session, take the transactions and enter them
 * into a block chain object (represented using a linked list). Once a session is over and 
 * block chain is created, the block chain object is written to a specified binary file.
 * Parameters: fil - name of binary file for writing block chain object
 */ 
void newPeep(char *fil){
    unsigned int num_trans;
    int i;
    LINK *first_link = malloc(sizeof(LINK));  //First node of the block chain
    //set dummy link which might get set during session
    first_link->next = NULL;
    for(i=0;i<7;i++) first_link->block.previousHash[i] = 0;
    for(i=0;i<7;i++) first_link->block.blockHash[i] = 0;
    for(i=0;i<7;i++) first_link->block.headHash[i] = 0;
    first_link->block.timeStamp = 0;
    first_link->block.blockNum = 0;
    first_link->block.blockSize = 0;
    last_link = first_link; //Dummy link placed in last_link 

    TRANSACTION * modBuffer = malloc(sizeof(TRANSACTION) * MXTXNUM);
    char *peep = initializePeep();
    num_trans = editor(peep,modBuffer);  //start session
    addTransactions(modBuffer,num_trans); 
    writeBlockChain(first_link,fil);
    free(modBuffer);
    free(peep);
    return;
}

/*Purpose: edit an existing peep by reading a block chain object from a specified 
 * binary file, then invoke editor for new transactions. Finally, after a session,
 * add transactions into existing block chain and write the object to the same 
 * binary file.
 * Parameters: fil - name of binary file for reading and writing block chain object
 */
void editPeep(char *fil){
    LINK *first_link;  //First node of the block chain
    LINK *next_link,*cur_link;  
    unsigned int num_trans;
    TRANSACTION *modBuffer = malloc(sizeof(TRANSACTION) *MXTXNUM);
    char *peep = initializePeep();
    
    //Set first link in block chain; set peep
    first_link = readBlockChain(fil);
    getPeep(first_link,peep);

    //Set last_link
    cur_link = first_link;
    next_link = cur_link->next;
    while(next_link!=NULL){
        cur_link = next_link;
        next_link = cur_link->next;
    }
    last_link = cur_link;
    num_trans = editor(peep,modBuffer);  //start session
    addTransactions(modBuffer,num_trans);
    writeBlockChain(first_link,fil);
    free(modBuffer);
    free(peep);

    return;
}

/*Purpose: verify the integrity of an existing peep
 * Parameters: fil - name of binary file for reading and verifying the block chain
 */
void verifyPeep(char *fil){
    /*
    int valid;
    Node *first = NULL;  //First node of the block chain
    first = readBlockChain(fil);e
    valid = verifyBlockChain(&first);

    if(valid)
        printf("Block chain broken: data has been tampered with.\n");
    else printf("Block chain verified: data is secure.\n");

    freeBlockChain(&first);
    return;
    */
    /*
    int valid;
    BLOCK *first = malloc(sizeof(LINK));  //First node of the block chain
    readBlockChain(fil);
    valid = verifyBlockChain(first);

    if(valid)
        printf("Block chain broken: data has been tampered with.\n");
    else printf("Block chain verified: data is secure.\n");

    freeBlockChain(first);
    */
    return;
}

/*Purpose: print transaction history of existing peep
 * Parameters: fil - name of binary file for reading block chain
 */
void historyPeep(char *fil){
    LINK *first;  //First node of the block chain
    first = readBlockChain(fil);
    printTransactions(first);
    return;

}

/*Purpose: reproduce current peep based on specified binary file
 * parameters: fil - name of binary file for reading block chain
 * (in other words, for getting current peep)
 */
void reproducePeep(char *fil){
    char *peep = initializePeep();
    LINK *first; //First node of the block chain

    first = readBlockChain(fil);
    getPeep(first,peep);
    /*for(int i=0;i<PEEPLEN && peep[i]!='\0';i++)
        printf("%d:%c ",i+1,peep[i]);
    printf("\n");
    */
    printf("%s\n", peep);
    free(peep);
    
    return;
}

/*Purpose: reproduce a peep at a given timestamp (in secs since 
 * epoch: January 1,2021). 
 * Parameters: fil - name of binary file for reading block chain
 *             time - pointer to a string containing number 
 *                    characters
 */
void atTimePeep(char *fil,char *time){
    char *peep = initializePeep();
    LINK *first;  //First node of the block chain
    int secs = strToNum(time);

    first =readBlockChain(fil);
    getPeepAtTime(first,peep,secs);
    for(int i=0;i<PEEPLEN && peep[i]!='\0';i++)
        printf("%d:%c ",i+1,peep[i]);
    printf("\n");
    //printf("%s\n", peep);
    free(peep);
    
    return;

}

int main(int argc,char **argv){
    
    int option;
    int cflag=0,eflag=0,vflag=0,hflag=0,pflag=0,tflag=0; 
     
    char *infile, *outfile, *timestamp;

    //Process the options for peep program
    while((option=getopt(argc,argv,":c:e:v:h:p:t:"))!=-1){
        switch(option){
            case 'c':  //Set cflag: create a new peep
                cflag = 1;                  
                outfile = optarg;
                break;
            case 'e':  //Set eflag: edit an existing peep
                eflag = 1;
                infile = optarg;
                outfile = optarg;
                break;
            case 'v':  //Set vflag: Verify integrity of existing peep
                vflag = 1;
                infile = optarg;
                break;
            case 'h':  //Set hflag: print the transaction history 
                       //of an existing peep
                hflag = 1;
                infile = optarg;
                break;
            case 'p':  //Set pflag: reproduce current peep
                pflag = 1;
                infile = optarg;
                break;
            case 't':  //Set tflag: reproduce a peep at a given 
                       //timestamp
                tflag = 1;
                timestamp = optarg;
                break;
            case ':':  //Missing argument
                fprintf(stderr, "%s: option '-%c' requires an argument\n",
                        argv[0], optopt);
                missUse();
                break;
            case '?':  //Incorrect option
                fprintf(stderr, "%s: option '-%c' is invalid: ignored\n",
                        argv[0], optopt);
                missUse();
                break;
            }
    }

    if(optind < argc){  //Don't run if too many arguments are provided
        fprintf(stderr,"Failed to run.\n");
        fprintf(stderr,"Argument(s) ignored:");
        for(;optind<argc;optind++){
            fprintf(stderr," \"%s\"", argv[optind]);
            if(optind+1<argc) fprintf(stderr,",");
        }
        fprintf(stderr,"\n");
        missUse();
    }
     
    //Use the flags to determine a course of action for an existing/new peep
    int flag_sum = cflag + eflag + vflag + hflag + pflag + tflag;
    if(flag_sum>2){  //Too many valid flags used 
        fprintf(stderr,"%s: invalid use of options.\n",argv[0]);
        missUse();
    }else if(flag_sum==2){
        if(pflag & tflag) atTimePeep(infile,timestamp);
        else{ 
            fprintf(stderr,"%s: invalid use of options.\n",argv[0]);
            missUse();
        }
    }else{  //when only one option is provided
        if(cflag) newPeep(outfile);
        else if(eflag) editPeep(infile);
        else if(vflag) verifyPeep(infile);
        else if(hflag) historyPeep(infile);
        else if(pflag) reproducePeep(infile);
        else if(tflag){ 
            fprintf(stderr,"%s: invalid use of options.\n",argv[0]);
            missUse();
        }
        else{
            fprintf(stderr,"%s: no operands.\n",argv[0]);
            missUse();
        }
    }
        
    return 0;
}

