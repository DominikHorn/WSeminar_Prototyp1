#define MEMORY_SIZE 1
#define MEMORY_MIN_NUMBER 97
#define MEMORY_MAX_NUMBER 122
#define PROGRAM_MAX_LENGTH 100
#define PROGRAM_COUNT 10000
#define PROGRAM_MAX_OUTPUT_SIZE PROGRAM_MAX_LENGTH+1
#define PROGRAM_INIT_SCORE 0xFFFFFF
#define MUTATION_RATE 2
#define TRUE 1
#define FALSE 0

typedef short boolean;

void fatalError(char*);
void generateRandomProgram(int, char*);
void mutateProgram(char*, int);
//void optimizeProgram(char* program);
int randomBetween(int, int);
int scoreProgramOutput(char*, char*);
char getRandomInstruction();
char getMutationSafeRandomInstruction();
boolean verifySyntax(char*);
boolean runBFProgram(char*, char*);