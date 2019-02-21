#ifndef OPENUTASK14_ALGORITHM_H
#define OPENUTASK14_ALGORITHM_H

#include "FileManagement.h"
#define IMG_COMP_MAX_SIGN_NUM 512

typedef enum CurrentRun {firstRun, secondRun} currentRun;
typedef enum ParameterValidationCases {ConstantCase, RegisterCase, StructCase, VariableCase} paramVldCases;

/*here we will implement the algorithm shown in the exercise, assembly first and second scan*/
void firstAssemblyScan(char* fileName);
void secondAssemblyScan(char* fileName);
#endif
