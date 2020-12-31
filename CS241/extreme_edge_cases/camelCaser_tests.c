/**
 * extreme_edge_cases
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

void printCamelCase(char** result){
    char ** iter = result;
    while(*iter){
        printf("%s,\n", *iter);
        iter++;
    }
    printf("NULL\n");
    destroy(result); 
}


int testOutline(const char* testString, char** correctOutput, char **(*camelCaser)(const char *),
                void (*destroy)(char **)) {
    char** result = camelCaser(testString);
    if(correctOutput ==  NULL || result == NULL){
        return correctOutput == result;
    }
    char ** iter1 = result;
    char ** iter2 = correctOutput;
    while(*iter1 || *iter2){
        if(!(*iter1) || !(*iter2)){
            return 0;
        }
        if(strcmp(*iter1, *iter2)){
            return 0;
        }
        iter1++;
        iter2++;
    }
    destroy(result);
    return 1;
}

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) { 

    
    

    //Test1 Empty String test
    char* testString = "";
    char* correctOutput1[] = {NULL};
    if(!testOutline(testString, correctOutput1, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test2 Null test
    testString = NULL;
    char** correctOutput2 = NULL;
    if(!testOutline(testString, correctOutput2, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test3 Provided on MP Documentation
    testString = "The Heisenbug is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion.";
    char* correctOutput3[] = {
        "theHeisenbugIsAnIncredibleCreature",
        "facenovelServersGetTheirPowerFromItsIndeterminism",
        "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener",
        "godObjectsAreTheNewReligion",
        NULL
        };
    if(!testOutline(testString, correctOutput3, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test4 Reference testcase
    testString = "hello. welcome to cs241   ";
    char* correctOutput4[] = {"hello", NULL};
    if(!testOutline(testString, correctOutput4, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test5 spaces only
    testString = "   ";
    char* correctOutput5[] = {NULL};
    if(!testOutline(testString, correctOutput5, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test6 ... only
    testString = "...";
    char* correctOutput6[] = {
        "",
        "",
        "",
        NULL
    };
    if(!testOutline(testString, correctOutput6, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test7 bunch of punctuation
    testString = "@.^dads.dsadasfdsSDFdsa.dsDSAS.dsad";
    char* correctOutput7[] = {
        "",
        "",
        "",
        "dads",
        "dsadasfdssdfdsa",
        "dsdsas",
        NULL
    };
    if(!testOutline(testString, correctOutput7, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test8 bunch of punctuation
    testString = "@.^dads.dsadasf\ndsSDFdsa.dsDSAS.ds\aad$$";
    char* correctOutput8[] = {
        "",
        "",
        "",
        "dads",
        "dsadasfDssdfdsa",
        "dsdsas",
        "ds\aad",
        "",
        NULL
    };
    if(!testOutline(testString, correctOutput8, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
            char** result = camelCaser(testString);
            printCamelCase(result);
        #endif
        return 0;
    }

    
    //Test9 Number fun
    testString = "dsadas890.dsadase213dsadas.21312dsadsadas";
    char* correctOutput9[] = {
        "dsadas890",
        "dsadase213dsadas",
        NULL
    };
    if(!testOutline(testString, correctOutput9, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test10 Only punctuation
    testString = "$$.??\"";
    char* correctOutput10[] = {
        "",
        "",
        "",
        "",
        "",
        "",
        NULL
    };
    if(!testOutline(testString, correctOutput10, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
        #endif
        return 0;
    }

    //Test11 Escape characters
    testString = "\? dsada \t dsadas \n dasdsa \" \a \b  ";
    char* correctOutput11[] = {
        "",
        "dsadaDsadasDasdsa",
        NULL
    };
    if(!testOutline(testString, correctOutput11, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
            char** result = camelCaser(testString);
            printCamelCase(result);
        #endif
        return 0;
    }

    //Test12 Number fun again
    testString = "dsadaS890. DsadasE213DsEdas.      21312Dsadsadas";
    char* correctOutput12[] = {
        "dsadas890",
        "dsadase213dsedas",
        NULL
    };
    if(!testOutline(testString, correctOutput12, camelCaser, destroy)){
        #ifdef DEBUG
            fprintf(stderr, "Failed testString: %s\n", testString);
            char** result = camelCaser(testString);
            printCamelCase(result);
        #endif
        return 0;
    }
 
    return 1;
}
