#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>

typedef struct VCB //Volume Control Block
{
	int root;               //Address of root directory
    int freeBlock;          //Address of first free block (LBA)
    int magicNumber;
    int blockSize;    
    int blockCount;         //How many blocks we have
    int freeBlockCount;     //How many free blocks
} VCB;

typedef struct FCB //File Control Block
{
    int sizeInBytes;        //0th byte
    int sizeInBlocks;       //4th byte
    int startBlock;         //8th byte
    char * fileName;        //12th byte -limit name to 32 characters
    char * filePath;        //44th byte -limit path to 384 characters
    char * dateModified;    //428th byte
} FCB;

typedef struct DiE //Directory Entry
{
    int parentDirectory;        //0th byte
    int fileControlBlocks[16];  //4th byte
    int childDirectories[16];   //68th byte
    char * directoryName;       //132nd byte -limit name to 32 characters
    char * directoryPath;       //164th byte -limit path to 384 characters
} DiE;

typedef struct freeSpaceLink
{
    int blockNumber;
    int nextBlock;
} freeSpaceLink;

// Function to Compare Strings -------------------------------------------
    int compareStrings(char * String1, char * String2)
    {
        int i = 0;
        while(String1[i] != 0 && String2[i] != 0)
        {
            if(tolower(String1[i]) == tolower(String2[i])) //lower case all words
            {
                i++;
                if(String1[i] == 0 && String2[i] == 0) return 1;
            }
            else {return 0;}
        }
        if(String1[i] == 0 && String2[i] == 0) return 1;
        return 0;
    }
// -----------------------------------------------------------------------

// interpret function ----------------------------------------------------
int interpret(char * examineString)
{
    // as, ls, cp, mv , rm, cp21, cp2fs, cd, pwd, history, help
    char * EXIT = "exit";
    char * LS = "ls";
    if(compareStrings(examineString, EXIT))
    {
        return 2;
    }
    if(compareStrings(examineString, LS))
    {
        printf("..\t.\n");
        return 1;
    }
    return 0;
}

int main (int argc, char * argv[]) {

void * buf = malloc (512);
int fileDescriptor = open ("FileSystemStorage.dat", O_CREAT | O_RDWR);
read(fileDescriptor,buf,24);
VCB * ptrVCB;
ptrVCB = (VCB*) buf;
VCB data;
data = *ptrVCB;

int * block;
//if(ptrVCB->magicNumber != 137538490)
if(ptrVCB->magicNumber == 137538490)
{
    //initialize VCB and write to volume
    data.magicNumber = 137538490;
    data.blockSize = 512;
    data.blockCount = 19532;
    data.freeBlockCount = 19530;
    data.freeBlock = 2;
    data.root = 1;

    lseek(fileDescriptor, 0, SEEK_SET);
    write(fileDescriptor,&data,24);
    printf("Volume written to disk \n");

    //initialize root directory and write to volume
    DiE rootDirectory;
    rootDirectory.directoryName = "root";
    rootDirectory.parentDirectory = -1;
    rootDirectory.directoryPath = "root";
    lseek(fileDescriptor, data.blockSize, SEEK_SET);
    write(fileDescriptor, &rootDirectory, 4);
    lseek(fileDescriptor, data.blockSize + 132, SEEK_SET);
    write(fileDescriptor, rootDirectory.directoryName, 4);
    lseek(fileDescriptor, data.blockSize + 164, SEEK_SET);
    write(fileDescriptor, rootDirectory.directoryPath, 4);

    //Initialize free space and write to volume
    for(int i = 2; i < data.blockCount; i++)
    {
        *block = i;
        lseek(fileDescriptor, data.blockSize * i, SEEK_SET);
        write(fileDescriptor, block, 4);
        *block = i+1;
        write(fileDescriptor, block, 4);
    }


}

//Program loop
char * userInput;
int interpretToken;
while(1)
{
    printf("~root $ ");
    scanf("%s",userInput);
    interpretToken = interpret(userInput);
    if(interpretToken == 2) {break;}
}

close(fileDescriptor);

return 0;
}