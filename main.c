#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <strings.h>

#include "testCases.h"

int main (int argc, char* argv[])
{
    // Seed random number generator
    int i, stime;
    long ltime;
    
    /* get the current calendar time */
    ltime = time(NULL);
    stime = (unsigned) ltime/2;
    srand(stime);
    
    if (argc == 1) {    // temporary system to incorporate interactive testing
        char* desiredOutput = calloc(1, sizeof(char)*0xFF);
        if (desiredOutput == NULL) {
            fatalError("Out of Memory");
        }
        
        
        printf("Please enter desired program output: ");
        scanf("%s", desiredOutput);
        
        printf("-------------------------------------------------------------------------------------\n1. Verifying that functions work properly (WARNING: methods can falsely pass test)...\n");
//        if (testProgram() == FALSE) {
//            fatalError("(!) Functions don't work properly");
//        }
        
        printf("2. Generating initial population\n");
        char* programs[PROGRAM_COUNT*2];
        int lowestScore = PROGRAM_INIT_SCORE;
        unsigned int bestProgramIndex;
        for (int i = 0; i < PROGRAM_COUNT*2; i+=2) {
            programs[i] = calloc(1, sizeof(char)*PROGRAM_MAX_LENGTH);
            programs[i+1] = calloc(1, sizeof(char)*PROGRAM_MAX_OUTPUT_SIZE);
            
            if (programs[i] == NULL || programs[i+1] == NULL) {
                fatalError("Out of Memory");
            }
            
            // initial generation
            do {
                generateRandomProgram(PROGRAM_MAX_LENGTH-1, programs[i]);
            } while (verifySyntax(programs[i]) == FALSE);   // generate programs, kill all syntax invalid programs immediately
            
            // initial run
            if (runBFProgram(programs[i], programs[i+1]) == FALSE) {    // program failed, just continue, we won't save it
                continue;
            }
            
            // score every program, best one is chosen as parent for next generation
            int score = scoreProgramOutput(desiredOutput, programs[i+1]);
            if (lowestScore >= score) {
                lowestScore = score;
                bestProgramIndex = i;
            }
        }
        printf("Best: \"%s\", output: \"%s\", desired: \"%s\"\n", programs[bestProgramIndex], programs[(bestProgramIndex+1)], desiredOutput);
        printf("3. Done Initializing\n-------------------------------------------------------------------------------------\n\n");
        
        
        printf("Generation     Score                     Output\n\n");
        int popCount = 0;
        while (lowestScore > 0) {
            // reset all programs and outputs
            for (int i = 0; i < PROGRAM_COUNT*2; i+=2) {
                if (i != bestProgramIndex) {
                    strncpy(programs[i], programs[bestProgramIndex], PROGRAM_MAX_LENGTH);
                    //bzero(programs[i+1], PROGRAM_MAX_LENGTH * sizeof(char));
					memset(programs[i+1], 0, PROGRAM_MAX_OUTPUT_SIZE);
                }
            }
            
            // mutate, run and score all programs, select best
            for (int i = 2; i < PROGRAM_COUNT*2; i+=2) {
                mutateProgram(programs[i], MUTATION_RATE);
                runBFProgram(programs[i], programs[i+1]);
                
                int score = scoreProgramOutput(desiredOutput, programs[i+1]);
                if (lowestScore >= score) {
                    lowestScore = score;
                    bestProgramIndex = i;
                }
            }
            
            runBFProgram(programs[bestProgramIndex], programs[bestProgramIndex+1]);
            if (++popCount % 10 == 0) {
                printf("%6i    |%5i    |   \"%.32s\"\n", popCount, lowestScore, programs[bestProgramIndex+1]);
            }
        }
        
        printf("\nFound Solution :D ... Summary:\n\t-> Program: \"%s\"\n\t-> Output: \"%s\"\n\t-> Desired Output: \"%s\"\n\t-> scoring: %i\n", programs[bestProgramIndex], programs[bestProgramIndex+1], desiredOutput, lowestScore);
        
        // free memory
        for (int i = 0; i < PROGRAM_COUNT*2; i++) {
            free(programs[i]);
        }
    } else {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "-i")) {
                // interactive or interface mode
                printf("Welcome to interactive mode. Type \"help\" for help.\n");
                
                char* userinput = calloc(1, sizeof(char)*512);
                char* command = calloc(1, sizeof(char)*32);
                if (userinput == NULL || command == NULL) {
                    fatalError("Out of Memory");
                }
                
                strncpy(userinput, ".", 1);
                
                while (strcmp(command, "exit") && strcmp(command, "quit")) {
                    // TODO: provide interface for all methods
                    
					memset(userinput, 0, 512);
					memset(command, 0, 32);
                    //bzero(userinput, 512);
                    //bzero(command, 32);
                    
                    printf("Command $ ");
                    scanf("%s", userinput);
                    
                    char currentChar = '-';
                    unsigned int index = 0, inputLength = strlen(userinput);
                    
                    if (inputLength == 0) {
                        strncpy(userinput, ".", 1);
                        continue;
                    }
                    
                    while (index < inputLength && index < 32) {
                        currentChar = userinput[index];
                        if (currentChar == ' ') {
                            index++;
                            break;
                        } else {
                            command[index] = userinput[index];
                            index++;
                        }
                    }
                    
                    if (!strcmp(command, "help")) {
                        printf("-> type \"run\" to execute custom brainfuck code\n");
                    }
                    if (!strcmp(command, "run")) {
                        // second argument starts at index
                        char* program = calloc(1, sizeof(char)*512);
                        char* output = calloc(1, sizeof(char)*512);
                        if (program == NULL || output == NULL) {
                            fatalError("Out of Memory");
                        }
                        
                        printf("program: ");
                        scanf("%s", program);
                        
                        runBFProgram(program, output);
                        
                        printf("\tOutput: \"%s\"\n", output);
                        
                        free(program);
                        free(output);
                    }
                }
                
                free(userinput);
                free(command);
            }
        }
    }
    
    return 0;
}

void fatalError(char* errorMsg)
{
    printf("ERROR: %s\n", errorMsg);
    exit(1);
}

/* this method only run's the program, doesn't validate it's syntax though */
boolean runBFProgram(char* program, char* output)  // TODO : replace output by struct containing output string and number of cpu cycles the program used
{
    if (program == NULL || output == NULL) return FALSE;
    
    unsigned int programCounter = 0;
    unsigned int programLength = strlen(program);
    
    unsigned char memory[MEMORY_SIZE];
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = MEMORY_MIN_NUMBER;
    }
    
    char instruction;
    int memoryPointer = 0, outputPointer = 0, emergencyBrake = 0xFFFF;
    
    //    printf("Running: \"%s\"; length: %i ...\n\n", program, programLength);
    while (programCounter <= programLength-1) {
        emergencyBrake--;
        if (emergencyBrake <= 0) {
            return FALSE;
        }
        
        // fetch instruction
        instruction = program[programCounter];
        
        switch (instruction) {
            case '>':       // increase memory Pointer
                memoryPointer++;
                if (memoryPointer > MEMORY_SIZE-1) {
                    memoryPointer = 0;
                }
                break;
            case '<':       // decrease memory Pointer
                memoryPointer--;
                if (memoryPointer < 0) {
                    memoryPointer = MEMORY_SIZE-1;
                }
                break;
            case '+':       // increase number at adress pointed to by pointer
                memory[memoryPointer]++;
                if (memory[memoryPointer] > MEMORY_MAX_NUMBER) {
                    memory[memoryPointer] = MEMORY_MIN_NUMBER;
                }
                break;
            case '-':       // decrease number at adress pointed to by pointer
                memory[memoryPointer]--;
                if (memory[memoryPointer] < MEMORY_MIN_NUMBER) {
                    memory[memoryPointer] = MEMORY_MAX_NUMBER;
                }
                break;
            case '.':       // print out ascii char from number at adress pointed to by pointer
                //                printf("%c", memory[memoryPointer]);
                output[outputPointer] = memory[memoryPointer];
                outputPointer = outputPointer == PROGRAM_MAX_OUTPUT_SIZE-2 ? 0 : outputPointer+1;
                break;
            case '[':
                if (memory[memoryPointer] == MEMORY_MIN_NUMBER) {
                    while (programCounter <= programLength-1 && program[programCounter] != ']') {
                        programCounter++;
                    }
                    programCounter++;
                    
                    if (programCounter == programLength) {
                        return TRUE;
                    }
                }
                break;
            case ']':
                if (memory[memoryPointer] != MEMORY_MIN_NUMBER) {
                    while (programCounter > 0 && program[programCounter] != '[') {
                        programCounter--;
                    }
                }
                break;
            default:
                break;
        }
        programCounter++;
    }
    
    return TRUE;
}

void generateRandomProgram(int progLength, char* program)
{
    if (program == NULL) return;
    
    for (int i = 0; i < progLength; i++) {
        program[i] = getRandomInstruction();
    }
}

void mutateProgram(char* newProgram, int mutationRate)
{
    if (newProgram == NULL) return;
    int programLength = strlen(newProgram);
    if (programLength == 0) return;
    
    while (mutationRate > 0) {
        int random_index;
        do {
            random_index = rand() % programLength;
        } while (newProgram[random_index] == '[' || newProgram[random_index] == ']');
        
        newProgram[random_index] = getMutationSafeRandomInstruction();
        mutationRate--;
    }
}

char getRandomInstruction()
{
    short instruction = rand() % 3;
    switch (instruction) {
        case 0:
            return '+';
            break;
        case 1:
            return '-';
            break;
        case 2:
            return '.';
            break;
            //        case 3:
            //            return '>';
            //            break;
            //        case 4:
            //            return '<';
            //            break;
            //        case 5:
            //            return '[';
            //            break;
            //        case 6:
            //            return ']';
            //            break;
        default:
            break;
    }
    return ' ';
}

char getMutationSafeRandomInstruction() {
    short instruction = rand() % 3;
    switch (instruction) {
        case 0:
            return '+';
            break;
        case 1:
            return '-';
            break;
        case 2:
            return '.';
            break;
            //        case 3:
            //            return '>';
            //            break;
            //        case 4:
            //            return '<';
            //            break;
        default:
            break;
    }
    return ' ';
}

int scoreProgramOutput(char* desiredOut, char* actualOut)
{
    if (desiredOut == NULL || actualOut == NULL) return PROGRAM_INIT_SCORE;
    
    int desOutLen = strlen(desiredOut);
    int actOutLen = strlen(actualOut);
    int score = 0;
    
    if (actOutLen > desOutLen) {
        score += /*((MEMORY_MAX_NUMBER - MEMORY_MIN_NUMBER)/2)*/13*(actOutLen-desOutLen);
    } else if (desOutLen > actOutLen) {
        score += /*((MEMORY_MAX_NUMBER - MEMORY_MIN_NUMBER)/2)*/13*(desOutLen-actOutLen);
    }
    
    for (int i = 0; i < (actOutLen > desOutLen ? desOutLen : actOutLen); i++) {
        score += actualOut[i] > desiredOut[i] ? actualOut[i] - desiredOut[i] : desiredOut[i] - actualOut[i];
    }
    
    return score;
}

int randomBetween(int start, int end)
{
    return (rand() % (end - start)) + start;
}

boolean verifySyntax(char* program)
{
    if (program == NULL) return FALSE;
    
    unsigned int progLength = strlen(program);
    int brackets = 0;
    
    for (int i = 0; i < progLength; i++) {
        if (program[i] == '[') {
            break;
        } else if (program[i] == ']') {
            return FALSE;
        }
    }
    
    for (int i = 0; i < progLength; i++) {
        switch (program[i]) {
            case '[':
                brackets++;
                break;
            case ']':
                brackets--;
                break;
            default:
                break;
        }
    }
    
    if (brackets == 0) {
        return TRUE;
    }
    
    return FALSE;
}
/*void optimizeProgram(char* program) {
    if (program == NULL) {
        return;
    }
    
    unsigned int progLength = strlen(program);
    unsigned int breakIndex = 0;
    boolean foundRedundance = FALSE;
    
    do {
        foundRedundance = FALSE;
        
        // find redundancy
        for (int i = 1; i < progLength-1; i++) {
            if (program[i] == '+') {
                if (program[i-1] == '-') {
                    foundRedundance = TRUE;
                    breakIndex = i-1;
                    break;
                } else if (program[i+1] == '-') {
                    foundRedundance = TRUE;
                    breakIndex = i;
                    break;
                }
            }
        }
        
        // shift redundancy
        if (foundRedundance == TRUE) {
            for (int j = breakIndex; j < progLength-2; j++) {
                if (program[j+2] == ' ') {
                    program[j] = ' ';
                    program[j+1] = ' ';
                } else {
                    program[j] = program[j+2];
                }
            }
        }
    } while (foundRedundance == TRUE);
}*/
