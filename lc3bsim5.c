/*
    Name 1: Arnav Mehrotra
    UTEID 1: am87244
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX1, MARMUX0,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    LD_SSP,
    LD_PSR,
    RESETIE,
    SSPMUX,
    RESETEE,
    R6MUX,
    LD_INT,
    GATE_INT,
    GATE_PSR,
    PSRMUX1, PSRMUX0,
    GATE_SSP,
    INTMUX,
    LD_JVEC,
    GATE_JVEC,
    LD_VA,
    SET_REF,
    LD_DBUFF,
    GATE_DBUFF,
/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
                                      (x[J3] << 3) + (x[J2] << 2) +
                                      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1]<<1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return(x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return((x[MARMUX1] << 1) + x[MARMUX0]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); }
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_SSP(int *x)        { return(x[LD_SSP]); }
int GetLD_PSR(int *x)        { return(x[LD_PSR]); }
int GetRESETIE(int *x)         { return x[RESETIE];}
int GetSSPMUX(int *x)        { return x[SSPMUX];}
int GetRESETEE(int *x)         { return x[RESETEE];}
int GetR6MUX(int *x)         { return x[R6MUX];}
int GetLD_INT(int *x)        { return x[LD_INT];}
int GetGATE_INT(int *x)      { return x[GATE_INT];}
int GetGATE_PSR(int *x)      { return x[GATE_PSR];}
int GetPSRMUX(int *x)        { return (x[PSRMUX1]<<1) + x[PSRMUX0];}
int GetGATE_SSP(int *x)      { return x[GATE_SSP];}
int GetINTMUX(int *x)        { return x[INTMUX];}
int GetLD_JVEC(int *x)       { return x[LD_JVEC];}
int GetGATE_JVEC(int *x)     { return x[GATE_JVEC];}
int GetLD_VA(int *x)         { return x[LD_VA];}
int GetSET_REF(int *x)       { return x[SET_REF];}
int GetLD_DBUFF(int *x)      { return x[LD_DBUFF];}
int GetGATE_DBUFF(int *x)    { return x[GATE_DBUFF];}
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */


    int READY;	/* ready bit */
    /* The ready bit is also latched as you dont want the memory system to assert it
       at a bad point in the cycle*/

    int REGS[LC_3b_REGS]; /* register file. */

    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

    int STATE_NUMBER; /* Current State Number - Provided for debugging */

/* For lab 4 */
    int INTV; /* Interrupt vector register */
    int EXCV; /* Exception vector register */
    int SSP; /* Initial value of system stack pointer */
    int PSR;
    int IE;
    int EE;
    int INT;
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

/* For lab 5 */
    int PTBR; /* This is initialized when we load the page table */
    int VA;   /* Temporary VA register */
    int JVEC;
    int DBUFF;
/* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    eval_micro_sequencer();
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();

    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
    if(CYCLE_COUNT == 300){
        CURRENT_LATCHES.IE = 1;
        CURRENT_LATCHES.INTV = 0x01;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                   i, ucode_filename);
            exit(-1);
        }

        /* Put in bits one at a time. */
        index = 0;

        for (j = 0; j < CONTROL_STORE_BITS; j++) {
            /* Needs to find enough bits in line. */
            if (line[index] == '\0') {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                       ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1') {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                       ucode_filename, i, j);
                exit(-1);
            }

            /* Set the bit in the Control Store. */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }

        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    if (is_virtual_base) {
        if (CURRENT_LATCHES.PTBR == 0) {
            printf("Error: Page table base not loaded %s\n", program_filename);
            exit(-1);
        }

        /* convert virtual_base to physical_base */
        virtual_pc = program_base << 1;
        pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
              MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

        printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
        if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
            program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
            printf("physical base of program: %x\n\n", program_base);
            program_base = program_base >> 1;
        } else {
            printf("attempting to load a program into an invalid (non-resident) page\n\n");
            exit(-1);
        }
    }
    else {
        /* is page table */
        CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
        CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) {
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

/* MODIFY: you can add more initialization code HERE */
    CURRENT_LATCHES.PSR = 0x8002;
    CURRENT_LATCHES.INTV = 0;
    CURRENT_LATCHES.IE = 0;
    CURRENT_LATCHES.EE = 0;
    CURRENT_LATCHES.EXCV = 0;
    CURRENT_LATCHES.DBUFF = 0;

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
        printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int GateMARMUX;
int GateALU;
int GateSHF;
int GateMDR;
int GatePC;
int cycleready = 0;
int MDRbuff = 0;
int saver6;

int isOp(int state){
    int opcodes[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15};
    for(int i = 0; i<14; i++){
        if(state == opcodes[i]){
            return 1;
        }
    }
    return 0;
}

void eval_micro_sequencer() {

    int IRD = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    int COND1 = CURRENT_LATCHES.MICROINSTRUCTION[1];
    int COND0 = CURRENT_LATCHES.MICROINSTRUCTION[2];
    int J5 = CURRENT_LATCHES.MICROINSTRUCTION[3];
    int J4 = CURRENT_LATCHES.MICROINSTRUCTION[4];
    int J3 = CURRENT_LATCHES.MICROINSTRUCTION[5];
    int J2 = CURRENT_LATCHES.MICROINSTRUCTION[6];
    int J1 = CURRENT_LATCHES.MICROINSTRUCTION[7];
    int J0 = CURRENT_LATCHES.MICROINSTRUCTION[8];
    int ir11 = (Low16bits(CURRENT_LATCHES.IR) & 0x0800) >> 11;
    int ir15 = (Low16bits(CURRENT_LATCHES.IR) & 0xF000) >> 12;
    int nextstate = 0;

    int ld_jvec = GetLD_JVEC(CURRENT_LATCHES.MICROINSTRUCTION);
    int gate_jvec = GetGATE_JVEC(CURRENT_LATCHES.MICROINSTRUCTION);

    if(IRD){
        nextstate = ir15;
        if(!isOp(nextstate)){
            CURRENT_LATCHES.EE = 1;
            CURRENT_LATCHES.EXCV = 0x05;
        }
    }
    else{
        int and1 = (COND1 & (!COND0) & CURRENT_LATCHES.BEN);
        int and2 = ((!COND1) & COND0 & CURRENT_LATCHES.READY);
        int and3 = (COND1 & COND0 & ir11);
        int or1 = and1 | J2;
        int or2 = and2 | J1;
        int or3 = and3 | J0;
        nextstate = (32*J5) + (16*J4) + (8*J3) + (4*or1) + (2*or2) + or3;
    }

    if(ld_jvec){
        CURRENT_LATCHES.JVEC = nextstate;
        nextstate = 51;
    }


    if(gate_jvec){
        nextstate = CURRENT_LATCHES.JVEC;
    }


    if(CURRENT_LATCHES.IE && CURRENT_LATCHES.STATE_NUMBER != 34) {
        nextstate = 34;
        cycleready = 0;
    }
    if(CURRENT_LATCHES.EE && CURRENT_LATCHES.STATE_NUMBER !=37){
        nextstate = 37;
        cycleready = 0;
    }


    for(int i = 0; i<56; i++){
        NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[nextstate][i];
    }
    NEXT_LATCHES.STATE_NUMBER = nextstate;

}


void cycle_memory() {

    if((CURRENT_LATCHES.EE && CURRENT_LATCHES.STATE_NUMBER !=37) || (CURRENT_LATCHES.IE && CURRENT_LATCHES.STATE_NUMBER !=34)){
        return;
    }

    int mio = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
    int rw = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
    int size = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);


    if(mio){
        cycleready++;
        if(cycleready == 4){
            NEXT_LATCHES.READY = 1;
            cycleready  = 0;
            if(rw){
                if(!size){
                    if(CURRENT_LATCHES.VA & 0x0001){
                        MEMORY[CURRENT_LATCHES.MAR >> 1][1] = ((CURRENT_LATCHES.MDR >> 8) & 0x00FF);
                    }
                    else{
                        MEMORY[CURRENT_LATCHES.MAR >> 1][0] = (CURRENT_LATCHES.MDR & 0x00FF);
                    }
                }
                else{
                    MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (Low16bits(CURRENT_LATCHES.MDR ) >> 8) & 0x00FF;
                    MEMORY[CURRENT_LATCHES.MAR >> 1][0] = (Low16bits(CURRENT_LATCHES.MDR) & 0x00FF);
                }
            }
            else{
                MDRbuff = (MEMORY[CURRENT_LATCHES.MAR >> 1][0] & 0x00FF) + ((MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) & 0xFF00);
            }

        }
        else{
            NEXT_LATCHES.READY = 0;
        }
    }
    else{
        NEXT_LATCHES.READY = 0;
    }



}



void eval_bus_drivers() {

    if((CURRENT_LATCHES.EE && CURRENT_LATCHES.STATE_NUMBER !=37) || (CURRENT_LATCHES.IE && CURRENT_LATCHES.STATE_NUMBER !=34)){
        return;
    }

    int alu = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
    int marmux = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int addr2mux = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int addr1mux = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int sr1mux = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int pcmux = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int LSFH1 = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);
    int size = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);



    int addr2val;
    int addr1val;

    int sreg;





    if(sr1mux){
        sreg = (Low16bits(CURRENT_LATCHES.IR)& 0x01C0) >> 6;
    }
    else{
        sreg = (Low16bits(CURRENT_LATCHES.IR) & 0x0E00) >> 9;
    }

    //evaluate addr2mux and shift
    if(addr2mux == 0){
        addr2val = 0;
    }
    else if(addr2mux == 1){
        addr2val = Low16bits(CURRENT_LATCHES.IR) & 0x003F;
        if((Low16bits(CURRENT_LATCHES.IR) & 0x0020) >> 5){
            addr2val = addr2val + 0xFFC0;
        }
    }
    else if(addr2mux == 2){
        addr2val = Low16bits(CURRENT_LATCHES.IR) & 0x01FF;
        if((Low16bits(CURRENT_LATCHES.IR) & 0x0100) >> 8){
            addr2val = addr2val + 0xFE00;
        }
    }
    else if(addr2mux == 3){
        addr2val = Low16bits(CURRENT_LATCHES.IR) & 0x07F;
        if((Low16bits(CURRENT_LATCHES.IR) & 0x0400) >> 10){
            addr2val = addr2val + 0xF800;
        }
    }

    if(LSFH1){
        addr2val = addr2val << 1;
    }

    int shfval = CURRENT_LATCHES.IR & 0x000F;
    if(((CURRENT_LATCHES.IR & 0x0030) >> 4) == 0){
        GateSHF = CURRENT_LATCHES.REGS[sreg] << shfval;
    }
    else if(((CURRENT_LATCHES.IR & 0x0030) >> 4) == 1){
        GateSHF = CURRENT_LATCHES.REGS[sreg] >> shfval;
    }
    else if(((CURRENT_LATCHES.IR & 0x0030) >> 4) == 3){
        GateSHF = CURRENT_LATCHES.REGS[sreg];
        for(int i = 0; i<shfval; i++){
            int shfbuff = GateSHF;
            GateSHF = shfbuff >> 1;
            if((shfbuff & 0x8000) >> 15){
                GateSHF += 0x8000;
            }
        }
    }



    //evaluate addr1mux
    if(!addr1mux){
        addr1val = Low16bits(CURRENT_LATCHES.PC);
    }
    else{
        addr1val = Low16bits(CURRENT_LATCHES.REGS[sreg]);
    }

    //evaluate alu
    if(alu == 0){
        if((CURRENT_LATCHES.IR & 0x0020) >> 5){
            int IRval = Low16bits(CURRENT_LATCHES.IR & 0x001F);
            if((IRval & 0x0010) >> 4){
                IRval = IRval + 0xFFE0;
            }
            GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg] + IRval);
        }
        else{
            int sreg2 = Low16bits(CURRENT_LATCHES.IR) & 0x0007;
            GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg] + CURRENT_LATCHES.REGS[sreg2]);
        }

    }
    else if(alu == 1){
        if((CURRENT_LATCHES.IR & 0x0020) >> 5){
            int IRval = Low16bits(CURRENT_LATCHES.IR & 0x001F);
            if((IRval & 0x0010) >> 4){
                IRval = IRval + 0xFFE0;
            }
            GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg]) & IRval;
        }
        else{
            int sreg2 = Low16bits(CURRENT_LATCHES.IR) & 0x0007;
            GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg]) & CURRENT_LATCHES.REGS[sreg2];
        }
    }
    else if(alu == 2){
        if((CURRENT_LATCHES.IR & 0x0020) >> 5){
            int IRval = Low16bits(CURRENT_LATCHES.IR & 0x001F);
            if((IRval & 0x0010) >> 4){
                IRval = IRval + 0xFFE0;
            }
            GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg])^ IRval;
        }
        else{
            int sreg2 = Low16bits(CURRENT_LATCHES.IR) & 0x0007;
            GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg]) ^ Low16bits(CURRENT_LATCHES.REGS[sreg2]);
        }
    }
    else if(alu == 3){
        GateALU = Low16bits(CURRENT_LATCHES.REGS[sreg]);
    }

    //evauluate gatemarmux
    if(marmux == 1){
        GateMARMUX = addr1val + addr2val;
    }
    else if(marmux == 2){
        GateMARMUX = CURRENT_LATCHES.PTBR + (2*((CURRENT_LATCHES.MAR >> 9) & 0x007F));
    }
    else if(marmux == 3){
        GateMARMUX = (CURRENT_LATCHES.MDR & 0x3E00) + (CURRENT_LATCHES.VA & 0x01FF);
    }
    else{
        GateMARMUX = (Low16bits(CURRENT_LATCHES.IR) & 0x00FF) << 1;
    }

    //evaluate PCMUX
    if(pcmux == 0){
        GatePC = CURRENT_LATCHES.PC +2;
    }
    else if(pcmux == 3){
        GatePC = CURRENT_LATCHES.PC -2;
    }
    if(pcmux == 2){
        GatePC = addr1val + addr2val;
    }

    if(size){
        GateMDR = Low16bits(CURRENT_LATCHES.MDR);
    }
    else{
        if(CURRENT_LATCHES.MAR & 0x0001){
            GateMDR = (CURRENT_LATCHES.MDR >> 8) & 0x00FF;
        }
        else{
            GateMDR = CURRENT_LATCHES.MDR & 0x00FF;
        }

    }

}


void drive_bus() {

    if((CURRENT_LATCHES.EE && CURRENT_LATCHES.STATE_NUMBER !=37) || (CURRENT_LATCHES.IE && CURRENT_LATCHES.STATE_NUMBER !=34)){
        return;
    }

    int SHFlatch = GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION);
    int Marmuxlatch = GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int ALUlatch = GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION);
    int MDRlatch = GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
    int PClatch = GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION);

    int INTlatch = GetGATE_INT(CURRENT_LATCHES.MICROINSTRUCTION);
    int PSRlatch = GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION);
    int SSPlatch = GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION);
    int DBUFFlatch = GetGATE_DBUFF(CURRENT_LATCHES.MICROINSTRUCTION);



    if(INTlatch){
        BUS = CURRENT_LATCHES.INT;
    }
    else if(PSRlatch){
        BUS = CURRENT_LATCHES.PSR;
    }
    else if(SSPlatch){
        BUS = CURRENT_LATCHES.SSP;
    }
    else if(SHFlatch){
        BUS = GateSHF;
    }
    else if(Marmuxlatch){
        BUS = GateMARMUX;
    }
    else if(ALUlatch){
        BUS = GateALU;
    }
    else if(MDRlatch){
        BUS = GateMDR;
    }
    else if(PClatch){
        BUS = CURRENT_LATCHES.PC;
    }
    else if(DBUFFlatch){
        BUS = CURRENT_LATCHES.DBUFF;
    }
    else{
        BUS = 0;
    }

}


void latch_datapath_values() {

    if((CURRENT_LATCHES.EE && CURRENT_LATCHES.STATE_NUMBER !=37) || (CURRENT_LATCHES.IE && CURRENT_LATCHES.STATE_NUMBER !=34)){
        NEXT_LATCHES.EXCV = CURRENT_LATCHES.EXCV;
        NEXT_LATCHES.INTV = CURRENT_LATCHES.INTV;
        NEXT_LATCHES.IE = CURRENT_LATCHES.IE;
        NEXT_LATCHES.EE = CURRENT_LATCHES.EE;
        return;
    }

    int drmux = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDPC = GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDCC = GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDREG = GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDBEN = GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDIR = GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDMDR = GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDMAR = GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION);
    int mio = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
    int size = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
    int pcmux = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int mdrlatch = GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION);

    int r6mux = GetR6MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDINT = GetLD_INT(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDSSP = GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDPSR = GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION);
    int psrmux = GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);

    int resetie = GetRESETIE(CURRENT_LATCHES.MICROINSTRUCTION);
    int resetee = GetRESETEE(CURRENT_LATCHES.MICROINSTRUCTION);

    int sspmux = GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int intmux = GetINTMUX(CURRENT_LATCHES.MICROINSTRUCTION);

    int LDVA = GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION);
    int setref = GetSET_REF(CURRENT_LATCHES.MICROINSTRUCTION);
    int LDDBUFF = GetLD_DBUFF(CURRENT_LATCHES.MICROINSTRUCTION);


    NEXT_LATCHES.JVEC = CURRENT_LATCHES.JVEC;

    if(LDVA){
        if((CURRENT_LATCHES.MAR & 0x0001) && GetDATA_SIZE(CONTROL_STORE[CURRENT_LATCHES.JVEC])){
            NEXT_LATCHES.EE = 1;
            NEXT_LATCHES.EXCV = 0x03;
            return;
        }
        NEXT_LATCHES.VA = CURRENT_LATCHES.MAR;
    }
    else{
        NEXT_LATCHES.VA = CURRENT_LATCHES.VA;
    }

    if(LDDBUFF){
        NEXT_LATCHES.DBUFF = CURRENT_LATCHES.MDR;
    }
    else{
        NEXT_LATCHES.DBUFF = CURRENT_LATCHES.DBUFF;
    }

    if(resetie){
        NEXT_LATCHES.IE = 0;
    }
    else{
        NEXT_LATCHES.IE = CURRENT_LATCHES.IE;
    }

    if(resetee){
        NEXT_LATCHES.EE = 0;
    }
    else{
        NEXT_LATCHES.EE = CURRENT_LATCHES.EE;
    }

    if(LDINT){
        if(intmux){
            NEXT_LATCHES.INT = 0x0200 + (CURRENT_LATCHES.EXCV << 1);
        }
        else{
            NEXT_LATCHES.INT = 0x0200 + (CURRENT_LATCHES.INTV << 1);
        }
    }
    else{
        NEXT_LATCHES.INT = CURRENT_LATCHES.INT;
    }

    if(LDSSP){
        if(sspmux){
            NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP - 2;
        }
        else{
            NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP +2;
        }
    }
    else{
        NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP;
    }





    int dreg;
    if(drmux == 1){
        dreg = 7;
    }
    else if(drmux == 0){
        dreg = (Low16bits(CURRENT_LATCHES.IR) & 0x0E00) >> 9;
    }
    else{
        dreg = 6;
    }
    if(LDREG){
        if (drmux == 2){
            if(r6mux){
                NEXT_LATCHES.REGS[6] = Low16bits(saver6);
            }
            else{
                saver6 = NEXT_LATCHES.REGS[6];
                NEXT_LATCHES.REGS[6] = BUS;
            }
        }
        else{
            NEXT_LATCHES.REGS[dreg] = Low16bits(BUS);
            if(!size && mdrlatch){
                NEXT_LATCHES.REGS[dreg] &= 0x00FF;
                if((NEXT_LATCHES.REGS[dreg] & 0x0080) >> 7){
                    NEXT_LATCHES.REGS[dreg] += 0xFF00;
                }
            }
        }
        for(int i = 0; i<8; i++){
            if(i != dreg){
                NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
            }
        }
    }
    else{
        for(int i = 0; i<8; i++){
            NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
        }
    }

    if(LDPC){
        if(pcmux == 1){
            NEXT_LATCHES.PC = Low16bits(BUS);
        }
        else{
            NEXT_LATCHES.PC = Low16bits(GatePC);
        }
    }
    else{
        NEXT_LATCHES.PC = CURRENT_LATCHES.PC;
    }

    if(LDCC){
        if(NEXT_LATCHES.REGS[dreg] == 0){
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
            NEXT_LATCHES.N = 0;
        }
        else if((NEXT_LATCHES.REGS[dreg] & 0x8000) >> 15){
            NEXT_LATCHES.P = 0;
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
    }
    else{
        NEXT_LATCHES.N = CURRENT_LATCHES.N;
        NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
        NEXT_LATCHES.P = CURRENT_LATCHES.P;
    }

    if(LDPSR){
        if(psrmux == 0){
            int buff = CURRENT_LATCHES.PSR & 0x8000;
            buff = buff + (NEXT_LATCHES.N << 2) + (NEXT_LATCHES.Z << 1) + (NEXT_LATCHES.P);
            NEXT_LATCHES.PSR = buff;
        }
        else if(psrmux == 1){
            NEXT_LATCHES.PSR = BUS;
        }
        else{
            NEXT_LATCHES.PSR = CURRENT_LATCHES.PSR & 0x7FFF;
        }
    }
    else{
        NEXT_LATCHES.PSR = CURRENT_LATCHES.PSR;
    }

    if(LDIR){
        NEXT_LATCHES.IR =  Low16bits(BUS);
    }
    else{
        NEXT_LATCHES.IR = CURRENT_LATCHES.IR;
    }

    if(LDMDR){
        if(mio){
            if(CURRENT_LATCHES.READY){
                NEXT_LATCHES.MDR = Low16bits(MDRbuff);
            }
        }
        else{
            if(size){
                NEXT_LATCHES.MDR = Low16bits(BUS);
            }
            else{
                NEXT_LATCHES.MDR = BUS & 0x00FF;
            }
        }
        if(setref){
            int PFN = (CURRENT_LATCHES.MDR >> 9) & 0x007F;
            if(PFN<23 && (CURRENT_LATCHES.PSR >> 15) && ((CURRENT_LATCHES.IR >> 12) != 15)){
                NEXT_LATCHES.EE = 1;
                NEXT_LATCHES.EXCV = 0x04;
                return;
            }
            if(!((CURRENT_LATCHES.MDR >> 2) & 0x0001)){
                NEXT_LATCHES.EE = 1;
                NEXT_LATCHES.EXCV = 0x02;
                return;
            }
            NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR | 0x0001;
        }
    }
    //MAYBE DON'T DO THIS
    else{
        NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR;
    }
    if(LDMAR){
        NEXT_LATCHES.MAR = Low16bits(BUS);
    }
    else{
        NEXT_LATCHES.MAR = Low16bits(CURRENT_LATCHES.MAR);
    }
    if(LDBEN){
        NEXT_LATCHES.BEN = (((CURRENT_LATCHES.IR & 0x0800) >> 11) & CURRENT_LATCHES.N) | (((CURRENT_LATCHES.IR & 0x0400) >> 10) & CURRENT_LATCHES.Z) | (((NEXT_LATCHES.IR & 0x0200) >> 9) & CURRENT_LATCHES.P);
    }
    else{
        NEXT_LATCHES.BEN = Low16bits(CURRENT_LATCHES.BEN);
    }

}

