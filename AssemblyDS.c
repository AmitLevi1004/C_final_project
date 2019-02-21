#include "AssemblyDS.h"

void Initialize() {
    currentExtTableSize = 0;
    currentEntTableSize = 0;
    errorInTheFirstRun = 0;
    errorInTheSecondRun = 0;
    allAssemblerOperationsTest = NULL;
    allData = NULL;
    extDS = NULL;
    entDS = NULL;

    InitializeThirtyTwoBitArray();
    InitializeCommandDS();
    InitializeCommandArgsDS();
    InitializeRegisterDS();
    labelHashTable = CreateHashTable(65536);
    IC = 0;
    DC = 0;

    lineCount = 0;
    firstRunAsFile = NULL;
    secondRunAsFile = NULL;
    obOutputFile = NULL;
    entOutputFile = NULL;
    extOutputFile = NULL;
}


void InitializeThirtyTwoBitArray() {
    binaryBitRep[0] = '.';
    binaryBitRep[1] = '/';
}

void InitializeCommandDS() {
    commandHashTable = CreateHashTable(17);
    SetValue(commandHashTable, "mov", 0, NULL);
    SetValue(commandHashTable, "cmp", 1, NULL);
    SetValue(commandHashTable, "add", 2, NULL);
    SetValue(commandHashTable, "sub", 3, NULL);
    SetValue(commandHashTable, "not", 4, NULL);
    SetValue(commandHashTable, "clr", 5, NULL);
    SetValue(commandHashTable, "lea", 6, NULL);
    SetValue(commandHashTable, "inc", 7, NULL);
    SetValue(commandHashTable, "dec", 8, NULL);
    SetValue(commandHashTable, "jmp", 9, NULL);
    SetValue(commandHashTable, "bne", 10, NULL);
    SetValue(commandHashTable, "red", 11, NULL);
    SetValue(commandHashTable, "prn", 12, NULL);
    SetValue(commandHashTable, "jsr", 13, NULL);
    SetValue(commandHashTable, "rts", 14, NULL);
    SetValue(commandHashTable, "stop",15, NULL);
}

void InitializeRegisterDS() {
    registerCodeHashTable = CreateHashTable(11);
    SetValue(registerCodeHashTable, "r0", 0, NULL);
    SetValue(registerCodeHashTable, "r1", 1, NULL);
    SetValue(registerCodeHashTable, "r2", 2, NULL);
    SetValue(registerCodeHashTable, "r3", 3, NULL);
    SetValue(registerCodeHashTable, "r4", 4, NULL);
    SetValue(registerCodeHashTable, "r5", 5, NULL);
    SetValue(registerCodeHashTable, "r6", 6, NULL);
    SetValue(registerCodeHashTable, "r7", 7, NULL);
}

void ExtendAllDataArray() {
    if (allData == NULL) {
        allData = (AllData*)malloc(sizeof(AllData));
        if (allData == NULL) {
            exit(-1);
            /*malloc failed*/
        }
        return;
    }

    allData = (AllData*)realloc(allData, (DC + 1)* sizeof(AllData));

    if (allData == NULL) {
        exit(-1);
        /*malloc failed*/
    }
}

void ExtendAllOperationArray() {
    if (allAssemblerOperationsTest == NULL) {
        allAssemblerOperationsTest = (unsigned short*)malloc(sizeof(short));
        if (allAssemblerOperationsTest == NULL) {
            exit(-1);
            /*malloc failed*/
        }
        return;
    }

    allAssemblerOperationsTest =
            (unsigned short*)realloc(allAssemblerOperationsTest, (IC + 1)* sizeof(short));

    if (allAssemblerOperationsTest == NULL) {
        exit(-1);
        /*malloc failed*/
    }
}

void ExtendExternDS(extOrEntTable** extendTable, int* currentTableSize) {
    if (*extendTable == NULL) {
        *extendTable = (extOrEntTable*)malloc(sizeof(extOrEntTable));
        if (*extendTable == NULL) {
            exit(-1);
            /*malloc failed*/
        }
        (*currentTableSize)++;
        return;
    }

    *extendTable = (extOrEntTable*)realloc(*extendTable, (*currentTableSize + 1)* sizeof(extOrEntTable));

    if (*extendTable == NULL) {
        exit(-1);
        /*malloc failed*/
    }

    (*currentTableSize)++;
}

void ClearData() {
    free(allAssemblerOperationsTest);
    free(allData);
    free(extDS);
    free(entDS);
    if (firstRunAsFile != NULL) {
        fclose(firstRunAsFile->fd);
        free(firstRunAsFile);
    }
    if (secondRunAsFile != NULL) {
        fclose(secondRunAsFile->fd);
        free(secondRunAsFile);
    }
    if (obOutputFile != NULL) {
        fclose(obOutputFile->fd);
        free(obOutputFile);
    }
    if (entOutputFile != NULL) {
        fclose(entOutputFile->fd);
        free(entOutputFile);
    }
    if (extOutputFile != NULL) {
        fclose(extOutputFile->fd);
        free(extOutputFile);
    }
    ClearHashTable(&labelHashTable);
    ClearHashTable(&commandHashTable);
    ClearHashTable(&commandNumOfArgs);
    ClearHashTable(&registerCodeHashTable);
}

void InitializeCommandArgsDS() {
    commandNumOfArgs = CreateHashTable(17);
    SetValue(commandNumOfArgs, "mov", 2, NULL);
    SetValue(commandNumOfArgs, "cmp", 2, NULL);
    SetValue(commandNumOfArgs, "add", 2, NULL);
    SetValue(commandNumOfArgs, "sub", 2, NULL);
    SetValue(commandNumOfArgs, "not", 1, NULL);
    SetValue(commandNumOfArgs, "clr", 1, NULL);
    SetValue(commandNumOfArgs, "lea", 2, NULL);
    SetValue(commandNumOfArgs, "inc", 1, NULL);
    SetValue(commandNumOfArgs, "dec", 1, NULL);
    SetValue(commandNumOfArgs, "jmp", 1, NULL);
    SetValue(commandNumOfArgs, "bne", 1, NULL);
    SetValue(commandNumOfArgs, "red", 1, NULL);
    SetValue(commandNumOfArgs, "prn", 1, NULL);
    SetValue(commandNumOfArgs, "jsr", 1, NULL);
    SetValue(commandNumOfArgs, "rts", 0, NULL);
    SetValue(commandNumOfArgs, "stop",0, NULL);
}