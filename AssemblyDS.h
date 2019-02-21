#ifndef OPENUTASK14_ASSEMBLYRELATIVEDS_H
#define OPENUTASK14_ASSEMBLYRELATIVEDS_H

#include "HashTabel.h"
#include "FileManagement.h"

#define LINE_SIZE 80
#define BIT_REP_SIZE 2
typedef int bool;
typedef enum DataType {data, string} dataType;

typedef struct assemblyAllData {
    /*all the data types known by the assembler (.data, .string)*/
    char charData;
    int integerData;
} AllData;

typedef struct ExternOrEntryAddressTable {
    char labelName[LINE_SIZE];
    unsigned char address;
} extOrEntTable;

/*Instruction counter, data counter*/
int IC;
int DC;

int lineCount;

int firstRunIC;

/*external and entry DS counters for reallocating memory purposes*/
int currentExtTableSize;
int currentEntTableSize;
/*variable that indicates that an error occurred while one of assembly runs*/
bool errorInTheFirstRun;
bool errorInTheSecondRun;

/*it stores here labels with their addresses (IC or DC) that represent the location the command or the data starts*/
hashTable* labelHashTable;
/*it stores here all the commands and their binary code representation*/
hashTable* commandHashTable;
hashTable* commandNumOfArgs;
hashTable* registerCodeHashTable;
/*it stores here all the data possible (of each data types)*/
AllData* allData;
extOrEntTable* extDS;
extOrEntTable* entDS;

/*relevant data structures.*/
/*here we will store each character of the 32 base "imaginary" computer*/
char binaryBitRep[BIT_REP_SIZE];
/*here will be stored arrays of chars (commands) of size 10 (5 words each word of 10 bits - 2 chars for each word)
in the sequence they were scanned from file.*/
char** allAssemblerOperation;
unsigned short* allAssemblerOperationsTest;

fileInfo* firstRunAsFile;
fileInfo* secondRunAsFile;
fileInfo* obOutputFile;
fileInfo* entOutputFile;
fileInfo* extOutputFile;

/*relevant initialization functions*/
void Initialize();
void InitializeThirtyTwoBitArray();
void InitializeCommandDS();
void InitializeRegisterDS();
void InitializeCommandArgsDS();

/*clear allocated space for relevant DS from heap*/
void ClearData();

/*extension function of the used DS*/
void ExtendAllOperationArray();
void ExtendAllDataArray();
void ExtendExternDS(extOrEntTable** extendTable, int* currentTableSize);

#endif
