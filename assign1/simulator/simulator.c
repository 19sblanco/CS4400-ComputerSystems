/*
  Author: Daniel Kopta
  CS 4400, University of Utah

  * Simulator handout
  * A simple x86-like processor simulator.
  * Read in a binary file that encodes instructions to execute.
  * Simulate a processor by executing instructions one at a time and appropriately 
  * updating register and memory contents.

  * Some code and pseudo code has been provided as a starting point.

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "instruction.h"

#include <limits.h>

// Forward declarations for helper functions
unsigned int get_file_size(int file_descriptor);
unsigned int* load_file(int file_descriptor, unsigned int size);
instruction_t* decode_instructions(unsigned int* bytes, unsigned int num_instructions);
unsigned int execute_instruction(unsigned int program_counter, instruction_t* instructions, 
				 int* registers, unsigned char* memory);
void print_instructions(instruction_t* instructions, unsigned int num_instructions);
void error_exit(const char* message);

// 17 registers
#define NUM_REGS 17
// 1024-byte stack
#define STACK_SIZE 1024

#define MIN_INT 0x80000000
#define MAX_INT 0x7fffffff

int main(int argc, char** argv)
{
  // Make sure we have enough arguments
  if(argc < 2)
    error_exit("must provide an argument specifying a binary file to execute");

  // Open the binary file
  int file_descriptor = open(argv[1], O_RDONLY);
  if (file_descriptor == -1) 
    error_exit("unable to open input file");

  // Get the size of the file
  unsigned int file_size = get_file_size(file_descriptor);
  // Make sure the file size is a multiple of 4 bytes
  // since machine code instructions are 4 bytes each
  if(file_size % 4 != 0)
    error_exit("invalid input file");

  // Load the file into memory
  // We use an unsigned int array to represent the raw bytes
  // We could use any 4-byte integer type
  unsigned int* instruction_bytes = load_file(file_descriptor, file_size);
  close(file_descriptor);

  unsigned int num_instructions = file_size / 4;


  /****************************************/
  /**** Begin code to modify/implement ****/
  /****************************************/

  // Allocate and decode instructions (left for you to fill in)
  instruction_t* instructions = decode_instructions(instruction_bytes, num_instructions);

  // Optionally print the decoded instructions for debugging
  // Will not work until you implement decode_instructions
  // Do not call this function in your handed in final version
  //print_instructions(instructions, num_instructions);


// Once you have completed part 1 (decoding instructions), uncomment the below block

  // Allocate and initialize registers
  int* registers = (int*)malloc(sizeof(int) * NUM_REGS);
  int length = sizeof(registers) / sizeof(int);
  for (int i = 0; i < length; i++) {
    registers[i] = 0;
  }
  registers[8] = STACK_SIZE;

  // Stack memory is byte-addressed, so it must be a 1-byte type
  // TODO allocate the stack memory. Do not assign to NULL.
  unsigned char* memory = (char*)malloc(sizeof(char) * 1024);
  length = sizeof(memory) / sizeof(char);
  for (int i = 0; i < length; i++) {
    memory[i] = 0;
  }
 


  // Run the simulation
  unsigned int program_counter = 0;

  // program_counter is a byte address, so we must multiply num_instructions by 4 to get the address past the last instruction
  while(program_counter != num_instructions * 4)
  {
    program_counter = execute_instruction(program_counter, instructions, registers, memory);
  }


    // todo: you probably want to free memeory on the heap here
    // just search every where you used malloc
  
  return 0;
}



/*
 * Decodes the array of raw instruction bytes into an array of instruction_t
 * Each raw instruction is encoded as a 4-byte unsigned int
*/
instruction_t* decode_instructions(unsigned int* bytes, unsigned int num_instructions)
{
  instruction_t* retval = (instruction_t*) malloc(sizeof(instruction_t) * num_instructions);

  int i;
  for(i = 0; i < num_instructions; i++) {
    unsigned int instruction = bytes[i]; // grab the first 4 bytes of next instruction

    unsigned char opcode = (instruction >> (sizeof(int)*8 - 5)) & 0b11111; 
    unsigned char reg1 = (instruction >> (sizeof(int)*8 - 10)) & 0b11111;
    unsigned char reg2 = (instruction >> (sizeof(int)*8 - 15)) & 0b11111;
    uint16_t immediate = (short)(instruction & 0xffff);

    instruction_t instr;
    instr.opcode = opcode;
    instr.first_register = reg1;
    instr.second_register = reg2;
    instr.immediate = immediate;
    
    retval[i] = instr;
  }

  return retval;
}


/*
 * Executes a single instruction and returns the next program counter
*/
unsigned int execute_instruction(unsigned int program_counter, instruction_t* instructions, int* registers, unsigned char* memory)
{
  // program_counter is a byte address, but instructions are 4 bytes each
  // divide by 4 to get the index into the instructions array
  instruction_t instr = instructions[program_counter / 4];
  
  int base_num = 0;
  unsigned int reg1 = (unsigned int)registers[instr.first_register];
  unsigned int reg2 = (unsigned int)registers[instr.second_register];
  long res = (long)reg2 - (long)reg1;
  switch(instr.opcode)
  {
  case subl: // 0
    registers[instr.first_register] = registers[instr.first_register] - instr.immediate;
    break;
  case addl_reg_reg: // 1
    registers[instr.second_register] = registers[instr.first_register] + registers[instr.second_register];
    break;
  case printr: // 20
    printf("%d (0x%x)\n", registers[instr.first_register], registers[instr.first_register]);
    break;
  case readr: // 21
    scanf("%d", &(registers[instr.first_register]));
    break;
  // my instructions
  case addl_imm_reg: // 2
    registers[instr.first_register] = registers[instr.first_register] + instr.immediate;
    break;
  case imull: // 3
    registers[instr.second_register] = registers[instr.first_register] * registers[instr.second_register];
    break;
  case shrl: // 4
    registers[instr.first_register] = (unsigned int)registers[instr.first_register] >> 1;
    break;
  case movl_reg_reg: // 5
    registers[(int)instr.second_register] = (int)registers[(int)instr.first_register];
    break;
  case movl_deref_reg: // 6 //wrong
    ;int* var1 = (int*)(memory + registers[instr.first_register] + instr.immediate);
    registers[instr.second_register] = *var1;
    break;
  case movl_reg_deref: // 7 //wrong remember that mem is a char and you have to cast correctly
    ;int* var2 = (int*)(memory + registers[instr.second_register] + instr.immediate);
    *var2 = registers[instr.first_register];

    break;
  case movl_imm_reg: // 8
    registers[instr.first_register] = (int)instr.immediate;
    break;
  case cmpl: // 9
    if (res < 0 || res > UINT_MAX) { // carry/borrow flag
        base_num = base_num + 1; 
    }
    if ((registers[instr.second_register] - registers[instr.first_register]) == 0) { // zero flag
        base_num = base_num + 64;
    }
    if ((registers[instr.second_register] - registers[instr.first_register]) >> 31) { // sign flag
        base_num = base_num + 128;
    }
    if (((long)registers[instr.second_register] - (long)registers[instr.first_register]) < (int)MIN_INT || ((long)registers[instr.second_register] - (long)registers[instr.first_register]) > (int)MAX_INT) { // overflow flag
        base_num = base_num + 2048;
    }
    registers[0] = base_num; //set %eflags
    break;
  case je: // 10
    if ((registers[0] & (int)64) == 64) {
        return program_counter + 4 + instr.immediate;
    }
    break;
  case jl: // 11
    if ((registers[0] & (int)128) == 128 ^ (registers[0] & (int)2048) == 2048) {
        return program_counter + 4 + instr.immediate;
    }
    break;
  case jle: // 12
    if (((registers[0] & (int)128) == 128 ^ (registers[0] & (int)2048) == 2048) || (registers[0] & (int)64) == 64) {
        return program_counter + 4 + instr.immediate;
    }
    break;
  case jge: // 13
    if (!((registers[0] & (int)128) == 128 ^ (registers[0] & (int)2048) == 2048)) {
        return program_counter + 4 + instr.immediate;
    }
    break;
  case jbe: // 14
    if (((registers[0] & (int)1) == 1) || (registers[0] & (int)64) == 64) {
        return program_counter + 4 + instr.immediate;
    }
    break;
  case jmp: // 15
    return program_counter + 4 + instr.immediate;
    break;
  case call: // 16
    registers[8] -= 4;
    ;int* var3 = (int*)(memory + registers[8]);
    *var3 = program_counter + 4;
    return program_counter + 4 + instr.immediate;
    break;
  case ret: // 17
    if (registers[8] == 1024) {
        exit(0);
    }
    else {
        ;int* var4 = (int*)(memory + registers[8]);
        program_counter = *var4;
        registers[8] += 4;
        return program_counter;
    }
    break;
  case pushl: // 18
    registers[8] -= 4;
    ;int* var5 = (int*)(memory + registers[8]);
    *var5 = registers[instr.first_register];
    break;
  case popl: // 19
    ;int* var6 = (int*)(memory + registers[8]);
    registers[instr.first_register] = *var6;
    registers[8] += 4;
    break;

  // TODO: Implement remaining instructions

  }

  // TODO: Don't always return program_counter + 4
  //       Some instructions will jump elsewhere

  // program_counter + 4 represents the subsequent instruction
  return program_counter + 4;
}


/***********************************************/
/**** Begin helper functions. Do not modify ****/
/***********************************************/

/*
 * Returns the file size in bytes of the file referred to by the given descriptor
*/
unsigned int get_file_size(int file_descriptor)
{
  struct stat file_stat;
  fstat(file_descriptor, &file_stat);
  return file_stat.st_size;
}

/*
 * Loads the raw bytes of a file into an array of 4-byte units
*/
unsigned int* load_file(int file_descriptor, unsigned int size)
{
  unsigned int* raw_instruction_bytes = (unsigned int*)malloc(size);
  if(raw_instruction_bytes == NULL)
    error_exit("unable to allocate memory for instruction bytes (something went really wrong)");

  int num_read = read(file_descriptor, raw_instruction_bytes, size);

  if(num_read != size)
    error_exit("unable to read file (something went really wrong)");

  return raw_instruction_bytes;
}

/*
 * Prints the opcode, register IDs, and immediate of every instruction, 
 * assuming they have been decoded into the instructions array
*/
void print_instructions(instruction_t* instructions, unsigned int num_instructions)
{
  printf("instructions: \n");
  unsigned int i;
  for(i = 0; i < num_instructions; i++)
  {
    printf("op: %d, reg1: %d, reg2: %d, imm: %d\n", 
	   instructions[i].opcode,
	   instructions[i].first_register,
	   instructions[i].second_register,
	   instructions[i].immediate);
  }
  printf("--------------\n");
}


/*
 * Prints an error and then exits the program with status 1
*/
void error_exit(const char* message)
{
  printf("Error: %s\n", message);
  exit(1);
}
