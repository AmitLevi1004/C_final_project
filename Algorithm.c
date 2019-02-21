#include "Algorithm.h"
#include "AssemblyDS.h"

void ErrorHandling (bool result);
void FinalDataConversion(char* fileName);
int NextCharOccurrence (char* arrayToHandle, int startingIndex);
int NextCommaOccurrence (char* arrayToHandle, int startingIndex);
int SkipWhiteSpace (char* arrayToHandle, int startingIndex);
void HandleCommand (char* arrayToHandle, int dataStartingIndex, char* commandName);
void SecondPassHandleCommand (char* arrayToHandle, int dataStartingIndex, char* commandName);
unsigned short HandleCommandParameter (char* parameterString, bool registerFlag, bool isFirstRegister);
void SecondPassHandleCommandParameter (char* parameterString, bool registerFlag);
void AddDecNumToAllAssemOpDS (char* stringNum, char* parameterString, currentRun currRun, unsigned char addressValue);
bool IsConstant (char* parameter);
bool IsVariable (char* parameter);
bool IsRegister(char* parameter);
bool IsLegitParameter (char* parameter, paramVldCases currentCase);
void HandleData(char* arrayToHandle, int dataStartingIndex, dataType dataType);
bool IsCommandLegit (char* label);
bool IsLabelLegit (char* label);
bool IsOperandLegit(unsigned short destContainer, unsigned short originContainer, char* command);

/*this function suitable specifically for checking data type
right after we can use atoi on the string that was checked for legit constant value
this function will check the case where some numeric values separated by white spaces which is a wrong statement*/
bool CheckIfLegitDataTypeConstant(char* dataConstant) {
    int startingIndex = 0;
    int finishingIndex = 0;
    char tempString[LINE_SIZE];
    startingIndex = SkipWhiteSpace(dataConstant, startingIndex);
    if (startingIndex == -1) {
        printf("Missing data type constant!\nline: %d\n", lineCount);
        return 0;
    }
    finishingIndex = startingIndex;
    while (dataConstant[finishingIndex] != ' ' && dataConstant[finishingIndex] != '\t' &&
            dataConstant[finishingIndex] != '\0' && dataConstant[finishingIndex] != '\n'){
        finishingIndex++;
    }

    memset(tempString, '\0', LINE_SIZE);
    if (dataConstant[finishingIndex] == ' ' || dataConstant[finishingIndex] == '\t' ||
            dataConstant[finishingIndex] == '\n') {
        strncpy(tempString, &dataConstant[startingIndex], finishingIndex - 1 - startingIndex + 1);
    } else{
        strncpy(tempString, &dataConstant[startingIndex], finishingIndex - startingIndex + 1);
    }

    if (!IsConstant(tempString)) {
        printf("Wrong data type constant!\nline: %d\n", lineCount);
        return 0;
    }
    finishingIndex = SkipWhiteSpace(dataConstant, finishingIndex);
    if (dataConstant[finishingIndex] != '\0' && dataConstant[finishingIndex] != '\n') {
        printf("Wrong data type constant!\nline: %d\n", lineCount);
        return 0;
    }

    return 1;
}

/*
 * This function extracts the data from the current read file line and will add it to the appropriate DS
 * it will also tag the data with its type
 */
void HandleData(char* arrayToHandle, int dataStartingIndex, dataType dataType) {
    int nextDataElementIndex = dataStartingIndex;
    int numericElement;
    char tempString[LINE_SIZE];
    /*keep processing the next element until there no more left (returned -1)*/
    while (nextDataElementIndex != -1) {
        switch (dataType) {
            case data:

                memset(tempString, '\0', LINE_SIZE);
                if (NextCommaOccurrence(arrayToHandle, nextDataElementIndex) != -1) {
                    strncpy(tempString, &arrayToHandle[nextDataElementIndex], NextCommaOccurrence(arrayToHandle, nextDataElementIndex) - 1
                                                       - nextDataElementIndex + 1);
                } else{
                    strncpy(tempString, &arrayToHandle[nextDataElementIndex], LINE_SIZE - 1
                                                       - nextDataElementIndex + 1);
                }
                if (!CheckIfLegitDataTypeConstant(tempString)) {
                    errorInTheFirstRun = 1;
                    return;
                }
                /*get the next number from the sequence of type 'data'*/
                numericElement = atoi(&arrayToHandle[nextDataElementIndex]);
                ExtendAllDataArray();
                allData[DC].integerData = numericElement;
                allData[DC].charData = '\0';
                DC++;
                nextDataElementIndex = NextCommaOccurrence(arrayToHandle, nextDataElementIndex);
                if (nextDataElementIndex == -1){
                    break;
                }
                nextDataElementIndex++;
                break;
            case string:
                ExtendAllDataArray();
                /*need to assign a value that will replace a null like variable in object*/
                allData[DC].integerData = IMG_COMP_MAX_SIGN_NUM + 1;
                allData[DC].charData = arrayToHandle[nextDataElementIndex];
                DC++;
                nextDataElementIndex = NextCharOccurrence(arrayToHandle, nextDataElementIndex);
                /*we finished reading the string and save its character data, adding null at the end of the string*/
                if (nextDataElementIndex == -1) {
                    ExtendAllDataArray();
                    allData[DC].integerData = IMG_COMP_MAX_SIGN_NUM + 1;
                    allData[DC].charData = '\0';
                    DC++;
                } else if (nextDataElementIndex == -2){
                    errorInTheFirstRun = 1;
                    printf("Wrong value of string data type!\nline: %d\n", lineCount);
                }
                break;
            default:
                /*error handling*/
                break;
        }

    }
}


/*The functions below are responsible for checking command's parameter propriety*/
bool IsLegitParameter (char* parameter, paramVldCases currentCase) {
    switch (currentCase) {
        case ConstantCase:
            return IsConstant(parameter);
        case RegisterCase:
            return IsRegister(parameter);
        case VariableCase:
            return IsVariable(parameter);
        default:
            break;
    }
    return 0;
}

bool IsRegister(char* parameter) {
    if (parameter[0] != 'r') {
        return 0;
    }
    if (parameter[1] > '7' || parameter[1] < '0') {
        return 0;
    }
    if (parameter[2] != '\0') {
        return 0;
    }

    return 1;
}
bool IsVariable (char* parameter) {
    return IsLabelLegit(parameter);

}
bool IsConstant (char* parameter) {
    if (parameter[0] != '-' && parameter[0] != '+'
        && !(parameter[0] >='0' && parameter[0] <= '9')) {
        return 0;
    }

    parameter++;

    while (*parameter != '\0') {
        if (!(*parameter >= '0' && *parameter <= '9')) {
            return 0;
        }
        parameter++;
    }
    return 1;
}

bool IsLabelLegit (char* label) {
    if (!(label[0] >= 'a' && label[0] <= 'z') && !(label[0] >= 'A' && label[0] <= 'Z')) {
        return 0;
    }

    while (*label != '\0') {
        if (!(*label >= 'a' && *label <= 'z') && !(*label >= 'A' && *label <= 'Z') &&
            !(*label >= '0' && *label <= '9')) {
            return 0;
        }
        label++;
    }

    return 1;
}

bool IsCommandLegit (char* label) {

    if (strcmp(label, "mov") == 0) {
        return 1;
    }

    return GetValue(commandHashTable, label);
}

/*
 * This function will add a decimal number to the commands DS as part of various parameters
 * it also adapted to each of the possible assembly runs (first and second)
 */
void AddDecNumToAllAssemOpDS (char* stringNum, char* parameterString, currentRun currRun, unsigned char addressValue) {
    char containerForNegativeValue;
    unsigned short value;
    char * keyType;
    if (currRun == firstRun) {
        containerForNegativeValue = (char)atoi(stringNum);
        value = (unsigned short)containerForNegativeValue;
    } else{
        value = addressValue;
    }
    /*save space of 2 bits for ERA*/
    value = value << 2;

    if (currRun == firstRun) {
        ExtendAllOperationArray();
        allAssemblerOperationsTest[IC] = value;
    } else{
        keyType = GetType(labelHashTable, parameterString);

        if (keyType == NULL) {
            printf("This label does not exist!\nline: %d\n", lineCount);
            errorInTheSecondRun = 1;
            return;
        }
        if (strcmp(keyType, "extern") == 0) {
            ExtendExternDS(&extDS, &currentExtTableSize);
            strncpy(extDS[currentExtTableSize - 1].labelName, parameterString, LINE_SIZE);
            extDS[currentExtTableSize - 1].address = (unsigned char)IC;
            value |= 1;
        } else if (strcmp(keyType, "entryData") == 0 || strcmp(keyType, "data") == 0){
            value = value >> 2;
            value += firstRunIC;
            value = value << 2;
            value |= 2;
        } else {
            value |= 2;
        }
        allAssemblerOperationsTest[IC] = value;
    }
    IC++;
}

/*
 * This function is closely implemented as the one responsible for the first run
 * this function will update the address value for parameters that are only variables (actually labels)
 */
void SecondPassHandleCommandParameter (char* parameterString, bool registerFlag) {
    char * preDelimiterLocation;
    char * postDelimiterLocation;
    unsigned char value;

    if (parameterString[0] == '#') {
        IC++;
        return;
    } else if ((preDelimiterLocation = strtok(parameterString, ".")) != NULL &&
               (postDelimiterLocation = strtok(NULL, ".")) != NULL){
        value = GetValue(labelHashTable, preDelimiterLocation);
        AddDecNumToAllAssemOpDS (NULL, parameterString, secondRun, value);
        IC++;
        return;
    } else if (IsRegister(parameterString)) {
        if (registerFlag == 1) {
            return;
        } else{
            IC++;
            return;
        }
    } else if (IsVariable(parameterString)) {
        value = GetValue(labelHashTable, parameterString);
        AddDecNumToAllAssemOpDS (NULL, parameterString, secondRun, value);
    } else{

        /*we encountered an error on this last case, we'll handle it later*/
    }
}

/*
 * this function will handle any type of parameters by saving them as words separated to left and right side
 * that are 5 bits each they are saved as their binary representation code.
 */
unsigned short HandleCommandParameter (char* parameterString, bool registerFlag, bool isFirstRegister) {
    unsigned short currentWord;
    char * preDelimiterLocation;
    char * postDelimiterLocation;
    char registerCode;

    if (parameterString[0] == '#') {
        if (!IsLegitParameter(parameterString + 1, ConstantCase)) {
            /*error handle*/
            printf("Wrong constant variable value!\nline: %d\n", lineCount);
        }
        AddDecNumToAllAssemOpDS (parameterString + 1, NULL, firstRun, 0);
        return 0;
    } else if ((preDelimiterLocation = strtok(parameterString, ".")) != NULL &&
            (postDelimiterLocation = strtok(NULL, ".")) != NULL){

        if (!IsLegitParameter(preDelimiterLocation, VariableCase)) {
            /*handle error*/
            printf("Wrong struct variable name!\nline: %d\n", lineCount);
            errorInTheFirstRun = 1;
            return 5;
        }

        if ((postDelimiterLocation[0] != '1' && postDelimiterLocation[0] != '2') ||
                postDelimiterLocation[1] != '\0') {
            /*handle error*/
            printf("Wrong struct field value!\nline: %d\n", lineCount);
            errorInTheFirstRun = 1;
            return 5;
        }

        ExtendAllOperationArray();

        /*we don't know yet the address (IC value) of the struct variable we will update it on the second run*/
        allAssemblerOperationsTest[IC] = 0;
        IC++;
        /*on the other side we do know the value of the field of that struct that we want to access*/
        AddDecNumToAllAssemOpDS (postDelimiterLocation, NULL, firstRun, 0);
        return 2;
    } else if (IsRegister(parameterString)) {
        /*if its the second parameter that is a register, there not needed to allocate more space in command DS*/
        if (registerFlag == 1) {
            currentWord = allAssemblerOperationsTest[IC - 1];
            registerCode = GetValue(registerCodeHashTable, parameterString);
            registerCode <<= 2;
            currentWord = currentWord | (unsigned short)registerCode;
            allAssemblerOperationsTest[IC - 1] = currentWord;
            /*case where there have not been a register as a previous parameter*/
        } else{
            ExtendAllOperationArray();
            /*case if it is the first parameter*/
            if (isFirstRegister) {
                registerCode = GetValue(registerCodeHashTable, parameterString);
                currentWord = (unsigned short)registerCode<<8;
                allAssemblerOperationsTest[IC] = currentWord;
                /*case if it is the second*/
            } else{
                registerCode = GetValue(registerCodeHashTable, parameterString);
                currentWord = (unsigned short)registerCode<<2;
                allAssemblerOperationsTest[IC] = currentWord;
            }
            IC++;
        }
        return 3;
    } else if (IsVariable(parameterString)) {
        ExtendAllOperationArray();

        /*we don't know yet the address (IC value) of the struct variable we will update it on the second run*/
        allAssemblerOperationsTest[IC] = 0;
        IC++;
        return 1;
        /*on the first run we just need to create space in the appropriate DS for the variable type parameter*/
    } else{
        /*we encountered an error on this last case, we'll handle it later*/
        printf("Wrong command parameter!\nline: %d\n", lineCount);
        errorInTheFirstRun = 1;
    }
    return 5;
}

void SecondPassHandleCommand (char* arrayToHandle, int dataStartingIndex, char* commandName) {
    int firstDataIndexFound;
    int nextDataElementIndex = dataStartingIndex;
    char parameterTempString[LINE_SIZE];
    bool isFirstParameter = 1;
    bool isRegister = 0;
    int jmpOrBneWithArgs = 0;
    IC++;

    if (strcmp(commandName, "rts") == 0 || strcmp(commandName, "stop") == 0) {
        return;
    }

    /*we will skip all the whitespaces to find the first occurrence of the parameter value*/
    nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
    firstDataIndexFound = nextDataElementIndex;

    /*keep processing the next element until there no more left (returned -1)*/
    while (nextDataElementIndex != -1 && nextDataElementIndex != LINE_SIZE) {
        if (arrayToHandle[nextDataElementIndex] != ' ' && arrayToHandle[nextDataElementIndex] != '\t' &&
            arrayToHandle[nextDataElementIndex] != ',' && arrayToHandle[nextDataElementIndex] != '\n'
            && arrayToHandle[nextDataElementIndex] != '(' && arrayToHandle[nextDataElementIndex] != ')'){
            nextDataElementIndex++;
            continue;
        }

        memset(parameterTempString, '\0', LINE_SIZE);

        strncpy(parameterTempString, &arrayToHandle[firstDataIndexFound],
                nextDataElementIndex - 1 - firstDataIndexFound + 1);
        /*this is when we will call the SecondPassHandleCommandParameter function to update all the missing info
        from the first run (label addresses as they found in the first run and saved in appropriate DS)*/
        if (isFirstParameter == 1) {
            isRegister = IsRegister(parameterTempString);
            SecondPassHandleCommandParameter (parameterTempString, 0);
            isFirstParameter = 0;
        } else if (jmpOrBneWithArgs == 1) {
            isRegister = IsRegister(parameterTempString);
            SecondPassHandleCommandParameter (parameterTempString, 0);
            jmpOrBneWithArgs = 0;
        }else {
            SecondPassHandleCommandParameter (parameterTempString, isRegister);
        }

        if ((arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] == '('
             || arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] == ')')
            && (strcmp(commandName, "jmp") == 0 || strcmp(commandName, "bne") == 0)) {

            if (arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] == ')') {
                break;
            }

            jmpOrBneWithArgs = 1;
            nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
            nextDataElementIndex++;
            nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
            firstDataIndexFound = nextDataElementIndex;
            continue;
        }
        /*we will skip all the whitespaces to find the next occurrence of the parameter value*/
        nextDataElementIndex = NextCommaOccurrence(arrayToHandle, nextDataElementIndex);
        if (nextDataElementIndex == -1) {
            break;
        }
        nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex + 1);
        firstDataIndexFound = nextDataElementIndex;
    }
}

/*This function will handle the line that represents a command and its parameters (if there is)*/
void HandleCommand (char* arrayToHandle, int dataStartingIndex, char* commandName) {
    int firstDataIndexFound = dataStartingIndex;
    int nextDataElementIndex = dataStartingIndex;
    char commandValue = GetValue(commandHashTable, commandName);
    char parameterTempString[LINE_SIZE];
    int result;
    int commandIC = IC;
    unsigned short destContainer = 0, originContainer = 0, jmpOrBneContainer = 2;
    bool isRegister;
    unsigned short commandWord;
    unsigned char commandMaxAmountOfArgs = GetValue(commandNumOfArgs, commandName);
    int parameterNum = 0;
    int jmpOrBneWithArgs = 0;
    ExtendAllOperationArray();
    IC++;


    /*handle commands that does not have parameters (actually here we know that we can return to the previous loop and
    continue the scanning process with the next assembly code line) check if there unnecessary character to come next*/
    if (strcmp(commandName, "rts") == 0 || strcmp(commandName, "stop") == 0) {
        result = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
        if (arrayToHandle[result] != '\n') {
            printf("rts and stop command do not take any arguments!\nline: %d\n", lineCount);
            errorInTheFirstRun = 1;
            return;
        }
    } else {

        /*we will skip all the whitespaces to find the first occurrence of the parameter value*/
        nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
        firstDataIndexFound = nextDataElementIndex;

        if (arrayToHandle[nextDataElementIndex] == '\n') {
            printf("Command requires at least one argument!\nline: %d\n", lineCount);
            errorInTheFirstRun = 1;
            return;
        }

        /*keep processing the next element until there no more left (returned -1)*/
        while (nextDataElementIndex != -1 && nextDataElementIndex != LINE_SIZE) {
            if (arrayToHandle[nextDataElementIndex] != ' ' && arrayToHandle[nextDataElementIndex] != '\t' &&
                arrayToHandle[nextDataElementIndex] != ',' && arrayToHandle[nextDataElementIndex] != '\n'
                    && arrayToHandle[nextDataElementIndex] != '(' && arrayToHandle[nextDataElementIndex] != ')') {
                nextDataElementIndex++;
                continue;
            }

            memset(parameterTempString, '\0', LINE_SIZE);

            strncpy(parameterTempString, &arrayToHandle[firstDataIndexFound],
                    nextDataElementIndex - 1 - firstDataIndexFound + 1);

            /*this is when we will call the HandleCommandParameter function that knows to process all kind of parameters*/
            if (parameterNum == 0) {
                isRegister = IsRegister(parameterTempString);
                originContainer = HandleCommandParameter(parameterTempString, 0, 1);
                parameterNum++;
            }
            else if (jmpOrBneWithArgs == 1 && parameterNum == 1) {
                isRegister = IsRegister(parameterTempString);
                originContainer = HandleCommandParameter(parameterTempString, 0, 1);
                parameterNum++;
            }
            else {
                destContainer = HandleCommandParameter(parameterTempString, isRegister, 0);
                parameterNum++;
            }

            if ((arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] == '('
                || arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] == ')')
                    && (strcmp(commandName, "jmp") == 0 || strcmp(commandName, "bne") == 0)) {

                if (arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] == ')') {
                    nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
                    nextDataElementIndex++;
                    if (arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] != '\n') {
                        errorInTheFirstRun = 1;
                        printf("Wrong command argument!\nline: %d\n", lineCount);
                    }
                    break;
                }

                jmpOrBneWithArgs = 1;
                commandMaxAmountOfArgs = 3;
                nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
                nextDataElementIndex++;
                nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex);
                firstDataIndexFound = nextDataElementIndex;
                continue;
            }

            if (arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] != ',' &&
                    arrayToHandle[SkipWhiteSpace(arrayToHandle, nextDataElementIndex)] != '\n') {
                errorInTheFirstRun = 1;
                printf("Wrong command argument!\nline: %d\n", lineCount);
            }

            /*in this case the command has more than maximum two possible parameters*/
            if (parameterNum > commandMaxAmountOfArgs) {
                printf("Too much arguments!\nline: %d\n", lineCount);
                errorInTheFirstRun = 1;
                return;
            }
            /*we will skip all the whitespaces to find the next occurrence of the parameter value*/
            nextDataElementIndex = NextCommaOccurrence(arrayToHandle, nextDataElementIndex);
            if (nextDataElementIndex == -1) {
                break;
            }
            nextDataElementIndex = SkipWhiteSpace(arrayToHandle, nextDataElementIndex + 1);
            firstDataIndexFound = nextDataElementIndex;
        }
    }

    if (parameterNum != commandMaxAmountOfArgs) {
        printf("Not enough arguments!\nline: %d\n", lineCount);
        errorInTheFirstRun = 1;
        return;
    }

    /*if there an unary function than there no origin container*/
    if (parameterNum == 1) {
        destContainer = originContainer;
        originContainer = 0;
    }

    if (!IsOperandLegit(destContainer, originContainer, commandName)) {
        printf("Wrong operands!\nline: %d\n", lineCount);
        errorInTheFirstRun = 1;
        return;
    }

    /*after processing previously (in the loop above) all the command's parameters
    we can now construct the main command operation word (the first one out of maximum of four)*/
    commandWord = (unsigned short)commandValue<<6;
    if (jmpOrBneWithArgs) {
        jmpOrBneContainer <<= 2;
        destContainer <<= 10;
        originContainer <<= 12;
        commandWord = commandWord | destContainer | originContainer | jmpOrBneContainer;
    } else {
        destContainer <<= 2;
        originContainer <<= 4;
        commandWord = commandWord | destContainer | originContainer;
    }
    allAssemblerOperationsTest[commandIC] = commandWord;
}

bool IsOperandLegit(unsigned short destContainer, unsigned short originContainer, char* command) {

    if (strcmp(command, "prn") != 0 && strcmp(command, "cmp") != 0 && strcmp(command, "rts") != 0 &&
            strcmp(command, "stop") != 0) {

        if (strcmp(command, "lea") == 0) {
            if (originContainer != 1) {
                return 0;
            }
        }

        if (destContainer == 0) {
            return 0;
        }
    }

    return 1;
}

/*String manipulation functions (the three below)*/
int SkipWhiteSpace (char* arrayToHandle, int startingIndex) {
    int counter = 0;
    while (arrayToHandle[startingIndex + counter] == ' ' || arrayToHandle[startingIndex + counter] == '\t') {
        counter++;
    }
    return startingIndex + counter;
}

int NextCommaOccurrence (char* arrayToHandle, int startingIndex){
    int counter = 0;
    while ((startingIndex+counter) != 79 && arrayToHandle[startingIndex + counter] != ',') {
        counter++;
    }
    if ((startingIndex+counter) == 79) {
        return -1;
    }
    return startingIndex + counter;
}

int NextCharOccurrence (char* arrayToHandle, int startingIndex) {
    if (startingIndex != LINE_SIZE - 1 && arrayToHandle[startingIndex + 1] != '"') {
        return startingIndex + 1;
    }
    /*end of line reached no closing quotation mark found*/
    if (startingIndex == LINE_SIZE - 1) {
        return -2;
    }
    return -1;
}
/*
 * This function will go through the assembly code file for the first time
 * this function will update the appropriate DS to get them ready for the final product
 * that is 32 bit representation "imaginary" machine file
 */
void firstAssemblyScan(char* fileName) {
    int i;
    firstRunAsFile = OpenOrCreateFile(fileName, "r");
    ReadFromFile(firstRunAsFile);
    /*keep scanning the current line until EOF is reached*/
    while(firstRunAsFile->currentLineData[0] != '\0') {
        int firstDataIndexFound = 0;
        int secondDataIndexFound;
        int foundWordStringFlag = 0, labelFoundFlag = 0;
        char tempString[LINE_SIZE], commandTempString[LINE_SIZE], labelTempString[LINE_SIZE];
        lineCount++;
        /*go through all the line characters*/
        for (i = 0; i < LINE_SIZE; i++) {
            /*code reach the end of line, go to the next line*/
            if (firstRunAsFile->currentLineData[i] == '\n' && foundWordStringFlag == 0) {
                break;
            }
            /*save the index of the first index of the character that is not a white space*/
            if (firstRunAsFile->currentLineData[i] != ' ' && firstRunAsFile->currentLineData[i] != '\t' &&
                    firstRunAsFile->currentLineData[i] != '\n') {

                /*check if its a comment, it starts with the ';' character*/
                if (firstRunAsFile->currentLineData[i] == ';' && foundWordStringFlag == 0) {
                    break;
                }

                if (!foundWordStringFlag) {
                    /*save the starting index of the word*/
                    firstDataIndexFound = i;
                }
                /*assign 1 to a flag that indicates we found a potential legitimate word*/
                foundWordStringFlag = 1;

                /*case we found white space,
                there an option its before any word that found or it is after we found a word*/
            } else if (firstRunAsFile->currentLineData[i] == ' ' || firstRunAsFile->currentLineData[i] == '\t' ||
                    firstRunAsFile->currentLineData[i] == '\n') {
                /*case we found a word, next it is needed to check and process it*/
                if (foundWordStringFlag == 1) {
                    /*case we found a label*/
                    if (firstRunAsFile->currentLineData[i - 1] == ':') {
                        /*save the label name to appropriate array*/
                        memset(labelTempString, '\0', LINE_SIZE);
                        strncpy(labelTempString, &firstRunAsFile->currentLineData[firstDataIndexFound],
                                i - 2 - firstDataIndexFound + 1);
                        ErrorHandling(IsLabelLegit (labelTempString));

                        if (!IsLabelLegit(labelTempString)) {
                            errorInTheFirstRun = 1;
                            printf("Wrong label format!\nline: %d\n", lineCount);
                            break;
                        }

                        labelFoundFlag = 1;
                    }
                    /*there two cases that are possible here:
                    first case: we found assembly code that represents data
                    second case: we found the words entry or extern that right after them will appear a label name*/
                    else if (firstRunAsFile->currentLineData[firstDataIndexFound] == '.') {

                        memset(tempString, '\0', LINE_SIZE);
                        strncpy(tempString, &firstRunAsFile->currentLineData[firstDataIndexFound + 1],
                                i - 1 - firstDataIndexFound);
                        /*first case*/
                        if (labelFoundFlag == 1){

                            /*case there is a data label, save the label with its address (DC + firstRunIC) to the label hash table
                            The firstRunIC variable will be added later*/
                            if (GetType(labelHashTable, labelTempString) == NULL) {
                                SetValue(labelHashTable, labelTempString, (unsigned char)DC, "data");
                            } else {
                                if (strcmp(GetType(labelHashTable, labelTempString), "entry") == 0) {
                                    SetValue(labelHashTable, labelTempString, (unsigned char)DC, "entryData");
                                } else{
                                    SetValue(labelHashTable, labelTempString, (unsigned char)DC, "externData");
                                }
                                ExtendExternDS(&entDS, &currentEntTableSize);
                                strncpy(entDS[currentEntTableSize - 1].labelName, labelTempString, LINE_SIZE);
                                entDS[currentEntTableSize - 1].address = (unsigned char)DC;
                            }

                            if (strcmp(tempString, "data") == 0) {
                                HandleData(firstRunAsFile->currentLineData,
                                           SkipWhiteSpace (firstRunAsFile->currentLineData, i), data);
                            } else if (strcmp(tempString, "string") == 0) {
                                if (firstRunAsFile->currentLineData[SkipWhiteSpace (firstRunAsFile->currentLineData, i)] != '"') {
                                    errorInTheFirstRun = 1;
                                    printf("Wrong value of string data type!\nline: %d\n", lineCount);
                                    break;
                                }
                                HandleData(firstRunAsFile->currentLineData,
                                           SkipWhiteSpace (firstRunAsFile->currentLineData, i) + 1, string);
                            }
                                /*encountered an error*/
                            else{
                                errorInTheFirstRun = 1;
                                printf("Wrong data type!\nline: %d\n", lineCount);
                            }
                            labelFoundFlag = 0;
                            break;
                            /*second case*/
                        } else{
                            if (strcmp(tempString, "entry") == 0) {
                                /*find the label that comes right after the entry keyword*/
                                firstDataIndexFound = SkipWhiteSpace (firstRunAsFile->currentLineData, i);
                                secondDataIndexFound = firstDataIndexFound;

                                while (firstRunAsFile->currentLineData[secondDataIndexFound]!= ' ' &&
                                        firstRunAsFile->currentLineData[secondDataIndexFound] != '\t' &&
                                        firstRunAsFile->currentLineData[secondDataIndexFound] != '\n') {
                                    secondDataIndexFound++;
                                }

                                if (firstRunAsFile->currentLineData[SkipWhiteSpace(firstRunAsFile->currentLineData,
                                                                                   secondDataIndexFound)] != '\n') {
                                    errorInTheFirstRun = 1;
                                    printf("Wrong entry label format!\nline: %d\n", lineCount);
                                    break;
                                }

                                memset(tempString, '\0', LINE_SIZE);
                                strncpy(tempString, &firstRunAsFile->currentLineData[firstDataIndexFound],
                                        secondDataIndexFound - 1 - firstDataIndexFound + 1);

                                ErrorHandling(IsLabelLegit (tempString));

                                if (!IsLabelLegit(tempString)) {
                                    errorInTheFirstRun = 1;
                                    printf("Wrong label format!\nline: %d\n", lineCount);
                                    break;
                                }
                                /*if there was a definition of the label previously (before we found "entry" instruction)*/
                                if (GetType(labelHashTable, tempString) != NULL) {
                                    ExtendExternDS(&entDS, &currentEntTableSize);
                                    strncpy(entDS[currentEntTableSize - 1].labelName, tempString, LINE_SIZE);
                                    entDS[currentEntTableSize - 1].address = GetValue(labelHashTable, tempString);
                                    if (strcmp(GetType(labelHashTable, tempString), "data") == 0) {
                                        SetValue(labelHashTable, tempString, GetValue(labelHashTable, tempString), "entryData");
                                    } else{
                                        SetValue(labelHashTable, tempString, GetValue(labelHashTable, tempString), "entryCommand");
                                    }
                                } else {
                                    SetValue(labelHashTable, tempString, 0, "entry");
                                }
                            } else if (strcmp(tempString, "extern") == 0) {
                                /*find the label that comes right after the extern keyword*/
                                firstDataIndexFound = SkipWhiteSpace (firstRunAsFile->currentLineData, i);
                                secondDataIndexFound = firstDataIndexFound;

                                while (firstRunAsFile->currentLineData[secondDataIndexFound]!= ' ' &&
                                        firstRunAsFile->currentLineData[secondDataIndexFound] != '\t' &&
                                        firstRunAsFile->currentLineData[secondDataIndexFound] != '\n') {
                                    secondDataIndexFound++;
                                }

                                if (firstRunAsFile->currentLineData[SkipWhiteSpace(firstRunAsFile->currentLineData,
                                                                                   secondDataIndexFound)] != '\n') {
                                    errorInTheFirstRun = 1;
                                    printf("Wrong extern label format!\nline: %d\n", lineCount);
                                    break;
                                }

                                memset(tempString, '\0', LINE_SIZE);
                                strncpy(tempString, &firstRunAsFile->currentLineData[firstDataIndexFound],
                                        secondDataIndexFound - 1 - firstDataIndexFound + 1);

                                ErrorHandling(IsLabelLegit (tempString));

                                if (!IsLabelLegit(tempString)) {
                                    errorInTheFirstRun = 1;
                                    printf("Wrong label format!\nline: %d\n", lineCount);
                                    break;
                                }

                                /*add the label to the appropriate hash table*/
                                SetValue(labelHashTable, tempString, 0, "extern");
                                /*encountered an error*/
                            } else{
                                printf("Wrong instruction statement!\nline: %d\n", lineCount);
                                errorInTheFirstRun = 1;
                            }
                            break;
                        }
                        /*found a command statement*/
                    } else {
                        /*case there is a command label, save the label with its address (IC) to the label hash table*/
                        if (labelFoundFlag == 1) {
                            if (GetType(labelHashTable, labelTempString) == NULL) {
                                SetValue(labelHashTable, labelTempString, (unsigned char)IC, "command");
                            } else {
                                if (strcmp(GetType(labelHashTable, labelTempString), "entry") == 0) {
                                    SetValue(labelHashTable, labelTempString, (unsigned char)IC, "entryCommand");
                                } else{
                                    SetValue(labelHashTable, labelTempString, (unsigned char)IC, "externCommand");
                                }
                                ExtendExternDS(&entDS, &currentEntTableSize);
                                strncpy(entDS[currentEntTableSize - 1].labelName, labelTempString, LINE_SIZE);
                                entDS[currentEntTableSize - 1].address = (unsigned char)IC;
                            }
                            labelFoundFlag = 0;
                        }

                        memset(commandTempString, '\0', LINE_SIZE);
                        strncpy(commandTempString, &firstRunAsFile->currentLineData[firstDataIndexFound],
                                i - 1 - firstDataIndexFound + 1);
                        /*check here if its a legit command*/
                        ErrorHandling(IsCommandLegit(commandTempString));

                        if (!IsCommandLegit(commandTempString)) {
                            errorInTheFirstRun = 1;
                            printf("Unknown command!\nline: %d\n", lineCount);
                            break;
                        }

                        HandleCommand (firstRunAsFile->currentLineData, i, commandTempString);
                        break;
                    }
                }
                /*set the flag back to 0 - it means we finished processing the current word*/
                foundWordStringFlag = 0;
            }

        }
        memset(firstRunAsFile->currentLineData, '\0', LINE_SIZE);
        /*read the next line from file until it reaches EOF (loop condition termination)*/
        ReadFromFile(firstRunAsFile);
    }
    firstRunIC = IC;
    for (i = 0; i < currentEntTableSize; i++) {
        if (strcmp(GetType(labelHashTable, entDS[i].labelName), "entryData") == 0) {
            entDS[i].address += firstRunIC;
        }
    }
}

void secondAssemblyScan(char* fileName) {
    IC = 0;
    secondRunAsFile = OpenOrCreateFile(fileName, "r");
    ReadFromFile(secondRunAsFile);
    /*keep scanning the current line until EOF is reached*/
    while(secondRunAsFile->currentLineData[0] != '\0') {
        int i;
        int firstDataIndexFound = 0;
        int foundWordStringFlag = 0;
        char tempString[LINE_SIZE], commandTempString[LINE_SIZE];
        /*go through all the line characters*/
        for (i = 0; i < LINE_SIZE; i++) {
            /*code reach the end of line, go to the next line*/
            if (secondRunAsFile->currentLineData[i] == '\n' && foundWordStringFlag == 0) {
                break;
            }
            /*save the index of the first index of the character that is not a white space*/
            if (secondRunAsFile->currentLineData[i] != ' ' && secondRunAsFile->currentLineData[i] != '\t'&&
                    secondRunAsFile->currentLineData[i] != '\n') {

                /*check if its a comment, it starts with the ';' character*/
                if (secondRunAsFile->currentLineData[i] == ';') {
                    break;
                }

                if (!foundWordStringFlag) {
                    /*save the starting index of the word*/
                    firstDataIndexFound = i;
                }
                /*assign 1 to a flag that indicates we found a potential legitimate word*/
                foundWordStringFlag = 1;

            } else if (secondRunAsFile->currentLineData[i] == ' ' || secondRunAsFile->currentLineData[i] == '\t' ||
                    secondRunAsFile->currentLineData[i] == '\n') {

                if (foundWordStringFlag == 1) {
                    /*line that starts with ';' character*/
                    if (secondRunAsFile->currentLineData[i - 1] == ':') {
                        ErrorHandling(IsLabelLegit (tempString));
                    }
                    else if (secondRunAsFile->currentLineData[firstDataIndexFound] == '.') {
                        break;
                        /*found a command statement*/
                    } else {
                        memset(commandTempString, '\0', LINE_SIZE);
                        strncpy(commandTempString, &secondRunAsFile->currentLineData[firstDataIndexFound],
                                i - 1 - firstDataIndexFound + 1);
                        SecondPassHandleCommand (secondRunAsFile->currentLineData, i, commandTempString);
                        break;
                    }
                }
                /*set the flag back to 0 - it means we finished processing the current word*/
                foundWordStringFlag = 0;
            }

        }
        memset(secondRunAsFile->currentLineData, '\0', LINE_SIZE);
        /*read the next line from file until it reaches EOF (loop condition termination)*/
        ReadFromFile(secondRunAsFile);
    }
    /*call the function that is responsible for creating the files
    that are the final product of the assembly program*/
    if (!errorInTheSecondRun) {
        FinalDataConversion(fileName);
    }

    firstRunIC = 0;
}

/*This function will create the final product files (three of them as described in assignment)*/
void FinalDataConversion(char* fileName) {
    /*first file handling*/

    /*create the first file with ".ob" extension for writing*/
    char newFileName[LINE_SIZE];
    char * delimiterLocation = strtok(fileName, ".");
    int i, j;
    unsigned short address;
    unsigned short binaryCode;
    unsigned short mask;
    char reminder;
    char convertedNumToStr[3];

    memset(newFileName, '\0', LINE_SIZE);

    strncpy(newFileName, delimiterLocation, LINE_SIZE);
    strcat(newFileName, ".ob");

    obOutputFile = OpenOrCreateFile(newFileName, "w");


    /*create each line separately by converting all the command data that found in both of the running
    to the 32 bit base "imaginary" machine run file as described in the assignment*/
    for (i = 0; i < IC; i++) {
        address = (unsigned short)i;
        binaryCode = allAssemblerOperationsTest[i];
        memset(obOutputFile->currentLineData, '\0', LINE_SIZE);

        obOutputFile->currentLineData[0] = '0';
        obOutputFile->currentLineData[1] = '0';
        obOutputFile->currentLineData[2] = '0';
        obOutputFile->currentLineData[3] = '0';
        obOutputFile->currentLineData[4] = '\t';

        j = 3;

        while (address != 0) {
            reminder = address%10;
            obOutputFile->currentLineData[j] = reminder + 48;
            address /= 10;
            j--;
        }

        mask = 8192;

        for(j = 5; mask > 0; j++) {
            if (mask&binaryCode) {
                obOutputFile->currentLineData[j] = binaryBitRep[1];
            } else {
                obOutputFile->currentLineData[j] = binaryBitRep[0];
            }
            mask = mask >> 1;
        }

        obOutputFile->currentLineData[j] = '\n';

        WriteToFile(obOutputFile);
    }

    /*append to the command words in the file that created previously, the data that comes right after*/
    for (i = 0; i < DC; i++) {
        /*continue from the address of the last command*/
        address = (unsigned short)(i + IC);
        /*check what type of data is saved in the current index out of array of all data*/
        if (allData[i].integerData != IMG_COMP_MAX_SIGN_NUM + 1) {
            binaryCode = (unsigned short)allData[i].integerData;
        } else if (allData[i].charData != '\0') {
            binaryCode = (unsigned short)allData[i].charData;
        }

        memset(obOutputFile->currentLineData, '\0', LINE_SIZE);

        obOutputFile->currentLineData[0] = '0';
        obOutputFile->currentLineData[1] = '0';
        obOutputFile->currentLineData[2] = '0';
        obOutputFile->currentLineData[3] = '0';
        obOutputFile->currentLineData[4] = '\t';

        j = 3;

        while (address != 0) {
            reminder = address%10;
            obOutputFile->currentLineData[j] = reminder + 48;
            address /= 10;
            j--;
        }

        mask = 8192;

        for(j = 5; mask > 0; j++) {
            if (mask&binaryCode) {
                obOutputFile->currentLineData[j] = binaryBitRep[1];
            } else {
                obOutputFile->currentLineData[j] = binaryBitRep[0];
            }
            mask = mask >> 1;
        }

        obOutputFile->currentLineData[j] = '\n';

        WriteToFile(obOutputFile);
    }

    /*second file handling*/

    /*create the second file with ".ent" extension for writing*/
    memset(newFileName, '\0', LINE_SIZE);
    strncpy(newFileName, delimiterLocation, LINE_SIZE);
    strcat(newFileName, ".ent");
    entOutputFile = OpenOrCreateFile(newFileName, "w");



    /*go through the DS that maps all the labels that are entries to their address
    save to that file the name of each label and the address that comes right after, in a separate line*/
    for (i = 0; i < currentEntTableSize; i++) {
        memset(entOutputFile->currentLineData, '\0', LINE_SIZE);
        strncpy(entOutputFile->currentLineData, entDS[i].labelName, LINE_SIZE);
        strcat(entOutputFile->currentLineData, "\t");
        sprintf(convertedNumToStr, "%d", entDS[i].address);
        strcat(entOutputFile->currentLineData, convertedNumToStr);
        strcat(entOutputFile->currentLineData, "\n");
        WriteToFile(entOutputFile);
    }

    /*second file handling*/

    /*create the second file with ".ent" extension for writing*/
    memset(newFileName, '\0', LINE_SIZE);
    strncpy(newFileName, delimiterLocation, LINE_SIZE);
    strcat(newFileName, ".ext");
    extOutputFile = OpenOrCreateFile(newFileName, "w");

    for (i = 0; i < currentExtTableSize; i++) {
        memset(extOutputFile->currentLineData, '\0', LINE_SIZE);
        strncpy(extOutputFile->currentLineData, extDS[i].labelName, LINE_SIZE);
        strcat(extOutputFile->currentLineData, "\t");
        sprintf(convertedNumToStr, "%d", extDS[i].address);
        strcat(extOutputFile->currentLineData, convertedNumToStr);
        strcat(extOutputFile->currentLineData, "\n");
        WriteToFile(extOutputFile);
    }
}

void ErrorHandling (bool result) {
    /*there already been an error than we can return from this function*/
    if(errorInTheFirstRun == 1) {
        return;
    }
    if (result == 0) {
        errorInTheFirstRun = 1;
    }
}