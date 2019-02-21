#include <stdio.h>
#include "AssemblyDS.h"
#include "Algorithm.h"


int main(int argc, char *argv[]) {

    int i;
    /*this function will initialize all the required data structures*/
    Initialize();

    /*perform both the assembly runs one after another for each of the files supplied by the user*/
    for (i = 1; i < argc; i++) {
        firstAssemblyScan(argv[i]);
        /*if there errors in the first scan there no need for the second scan*/
        if (!errorInTheFirstRun) {
            secondAssemblyScan(argv[i]);
        }
        /*clear all the allocated space on the heap*/
        ClearData();
        if (i != argc - 1) {
            /*initialize all the relevant DS for the next file*/
            Initialize();
        }
    }

    return 0;
}