/*------------------------------------------------------- 
# Students/Group: Damion Shillinglaw, Jethro Infante, Filip Dosenovic
# Assignment #3  
# Lab Section: X01L 
# CMPUT 201 
# Instructor's Name: Nesrine Abbas 
*------------------------------------------------------*/

#include "peepEditor.h"
#include <math.h>

#define INSERT 0 
#define DELETE 1
#define CLEARLINE(c) while((c=getchar())!='\n') 

/*Utility function for isBufferFull: set all members of each trans. inside buffer 'b' to false values*/
void flushBuffer(TRANSACTION *b){
    int i;
    for(i=0;i<MXTXNUM;i++){
        b->timestamp = 0;
        (b->mod).position = 0;
        (b->mod).character = '\0';
        (b->mod).event_type = 0;
    }
}

int isBufferFull(TRANSACTION *buffer,int *n){
    //check buffer is full. True: add new block to block chain
    if((*n!=0) && ((*n%MXTXNUM)==0)){ 
            addTransactions(buffer,*n);
            flushBuffer(buffer);
            *n = 0; //reset number of transactions
            return 1;
    }else return 0;
}

/*Takes every element in the peep and prints it, utility function for editor*/
void printPeep(char *p)
{
    int i;
    for (i = 0; i < strlen(p); i++){
        if(p[i] != '\0'){
            printf("%d:%c   ", i+1, p[i]);
        }
    }
    printf("\n");
}

/*Utility function for getUserInput that checks if deleting or inserting is in range for the peep.
 * Parameters: peep <- dynamic string
 *             line <- static char array; contains entire line of user input
 *             len <- length of line
 * Post: None
 * Return: either -1 for erroneous input or a valid position in peep
 */
int checkInRange(char *peep,char line[],int len){
            int position,i,tempLen;
            int max_pos = strlen(peep)+1;
            char str_input[4]; //At most, a position is 3 characters long
            memset(str_input,'\0',4);

            for(i=0;i<4 && line[i]!='\n';i++){
                if(!(line[i]>='0' && line[i]<='9')) //True: when user enters a non-numeric character 
                        return -1;                 //Thus, input is garbage 
                else str_input[i] = line[i];
            }

            i=0;
            position = 0;
            tempLen = strlen(str_input); 
            for(;i<tempLen;i++) //convert str_input to position in peep
                 position += ((int) pow(10,tempLen-(i+1))) * (str_input[i] - '0') ;
            if((position>0) && (position<=max_pos)) return position;
            else return -1; //position not in range
}

/*Utility function for editor that gets user input and checks it for validity 
 * under PEEP EDITOR MENU or sub menus
 * Pre: context parameter must be a value between 1-3 to work with switch statement
 * Post: None
 * Return: either -1 for erroneous input or valid position in peep
 */
int getUserInput(int context,char *peep){
    int input,n=50;
    char line[n]; //Assumption (value n): user won't enter more than 50 characters; of course, more than 1 char is invalid
    fgets(line,n,stdin);

    switch(context){ 
        case 1: //Check PEEP EDITOR MENU option
            if(line[1]!='\n') input = -1; //User shouldn't enter more than one char.
            else input = line[0] - '0';
            break;
        case 2: //Check if inserting is feasible 
            if((strlen(peep)==0) && (line[1]=='\n') && (line[0]=='1')) input = 1;
            else input = checkInRange(peep,line,n);
            break;
        case 3: //Check if deleting is feasible 
            input = checkInRange(peep,line,n);
            if(input>strlen(peep)) input = -1; 
            break;
    }

    return input;
}

/* edits the peep, saves transactions in modBuffer, and returns the number of transactions written in th ebuffer
 * Pre: Must allocate space for modBuffer which MUST be size of macro called MXTXNUM (from blockchain.h), 
 * and for peep (size 256)
 * Parameters: peep <- dynamic string
 *             modBuffer <- Contains struct typedef Transaction objects; will keep track of modif. to peep
 * Post: changes to peep and modBuffer
 * Return: number of transactions made out during a session
 * */
unsigned int editor(char *peep, TRANSACTION *modBuffer){
    int userChoice; 
    int numTransactions = 0;
    time_t before_epoch = 1609484400; //Seconds passed from Jan. 1, 1970 to Jan. 1, 2021 (current epoch)

    while(1){ //Main loop for running editor
        printf("PEEP EDITOR MENU\n");
        printf("Choose an Option\n");
        printf("1. View Characters\n");
        printf("2. Insert\n");
        printf("3. Delete\n");
        printf("4. Save and Exit\n");

        userChoice = getUserInput(1,peep);
        if (userChoice == 1){ //View Characters option
            printPeep(peep);
            printf("\n");
	}
        else if (userChoice == 2){ //Insertion option
            int insertPosition,j;  //j <-- indexing value used in "Perform insertion" part, bellow
            int plen = strlen(peep);
            char insertChar,tempChar;

            //Get and check input for insertion
            if(!(plen+1<PEEPLEN)){ //Check if peep has room for insertion
                printf("Peep is full. Delete characters to make room.\n\n");
                continue;
            }
            printf("Position to insert: ");
            insertPosition = getUserInput(2,peep);
            if(insertPosition == -1){
                printf("Invalid Position.\n\n");
                continue;
            }
            printf("Character to insert: ");
            scanf("%c", &insertChar); //Assumption: if user enters more than one char., then take the first one
            CLEARLINE(tempChar);
            if(!((insertChar>=32) && (insertChar<=127))){
                printf("Invalid character.\n\n");
                continue;
            }

            //Perform insertion
            numTransactions++;
            if(insertPosition == plen+1) peep[insertPosition-1] = insertChar; //Inserting at the end
            else{ //Not inserting at the end
                j = insertPosition - 1;
                while(peep[j]!='\0'){ //Shift characters to the right after insert pos.
                        tempChar = peep[j];
                        peep[j] = insertChar;
                        insertChar = tempChar;
                        j++;
                }
                peep[j] = insertChar;
            }

            //Display resulting peep
            printf("Peep becomes\n");
            printPeep(peep);
            printf("\n");
            
            //Initialize insert transaction and place in buffer
            TRANSACTION tempInsert; //Set a temp insert value
            tempInsert.mod.position = insertPosition;
            tempInsert.mod.character = insertChar; 
            tempInsert.mod.event_type = 0; 
            tempInsert.timestamp = time(NULL) - before_epoch; //Take the timestamp 
            modBuffer[numTransactions-1] = tempInsert; 

            isBufferFull(modBuffer,&numTransactions);

        }
        else if (userChoice == 3){  //Deletion option
            int delPosition,i; 

            //Get and check input for deletion
            printf("Current Peep\n");
            printPeep(peep);
            printf("Position to delete: ");
            delPosition = getUserInput(3,peep);
            if(delPosition == -1){
                printf("Invalid Position.\n\n");
                continue;
            }

            //Perform deletion
            numTransactions++;
            char tempChar;
            for(i=delPosition-1;i<strlen(peep)-1;i++){ //shift characters to left for the peep
                tempChar = *(peep+i+1); 
                *(peep+i) = tempChar;
            }
            *(peep+i) = '\0';
            printf("Peep becomes\n");
            printPeep(peep);
            printf("\n");

            //Initialize deletion transaction and place in buffer
            TRANSACTION tempDel;
            tempDel.mod.position = delPosition;
            tempDel.mod.event_type = 1;
            tempDel.timestamp = time(NULL) - before_epoch;
            modBuffer[numTransactions-1] = tempDel;

            isBufferFull(modBuffer,&numTransactions);
	}
        else if(userChoice == 4){ //Save and Exit option 
            break;
        }
        else printf("Invalid choice\n"); //Default output when user enters invalid choice
        
    }

    return numTransactions;
}

/*Test function for peepEditor module
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
*/
