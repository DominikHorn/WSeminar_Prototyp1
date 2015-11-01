#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "main.h"

boolean test_runBFProgram()
{
    printf("\t-> testing runBFProgram() ... ");
    
    char* program = "[+]+.";
    char* output = calloc(1, sizeof(char)*PROGRAM_MAX_OUTPUT_SIZE);
    if (output == NULL) {
        fatalError("Out of Memory");
    }

    runBFProgram(program, output);
    
    if (strcmp(output, "a") == 0) {
        printf("works\n");
        return TRUE;
    }
    
    printf("broken! runBFProgram() returns: \"%s\"; expected: \"a\"\n", output);
    return FALSE;
}

boolean test_generateRandomProgram()
{
    printf("\t-> testing generateRandomProgram() ... ");
    
    char* program = calloc(1, sizeof(char)*PROGRAM_MAX_LENGTH);
    if (program == NULL) {
        fatalError("Out of Memory");
    }
    
    generateRandomProgram(PROGRAM_MAX_LENGTH, program);
   
    for (int i = 0; i < PROGRAM_MAX_LENGTH; i++) {
        switch (program[i]) {
            case '>':
                break;
            case '<':
                break;
            case '+':
                break;
            case '-':
                break;
            case '.':
                break;
            case '[':
                break;
            case ']':
                break;
            default:
                printf("broken! generateRandomProgram() doesn't return a brainfuck program\n");
                return FALSE;
                break;
        }
    }
    printf("works\n");
    return TRUE;
}

boolean test_scoreProgramOutput()
{
    printf("\t-> testing scoreProgramOutput() ... ");
    
    int score = scoreProgramOutput("random", "random");
    if (score == 0) {
        printf("works\n");
        return TRUE;
    }
    
    printf("broken! scoreProgramOutput() returns false score: %i\n", score);
    return FALSE;
}

boolean test_verifySyntax()
{
    printf("\t-> testing verifySyntax() ... ");
    
    if (verifySyntax(".+-...[[[[[[[[[[[....[[[") == FALSE) {
        printf("works\n");
        return TRUE;
    }
    
    printf("broken! verifySyntax() falsely verifys syntax\n");
    return FALSE;
}

boolean test_mutateProgram()
{
    printf("\t-> testing mutateProgram() ... ");
    
    char* program = calloc(1, sizeof(char)*PROGRAM_MAX_LENGTH);
    char* newProgram = calloc(1, sizeof(char)*PROGRAM_MAX_LENGTH);
    if (program == NULL || newProgram == NULL) {
        fatalError("Out of Memory");
    }
    
    do {
        generateRandomProgram(PROGRAM_MAX_LENGTH, program);
    } while (verifySyntax(program) == FALSE);

    strncpy(newProgram, program, PROGRAM_MAX_LENGTH);
    mutateProgram(newProgram, MUTATION_RATE);
    
    if (strcmp(program, newProgram)) {
        printf("works\n");
        return TRUE;
    }
    
    printf("broken! verifySyntax() falsely verifys syntax\n");
    return FALSE;
}

boolean testProgram()
{
    if (test_runBFProgram() == FALSE || test_generateRandomProgram() == FALSE || test_scoreProgramOutput() == FALSE || test_verifySyntax() == FALSE || test_mutateProgram() == FALSE) {
        return FALSE;
    }

    return TRUE;
}
