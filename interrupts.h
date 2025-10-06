#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdio.h>


// Function declarations

int find_vector_index(int device_code);
unsigned int get_isr_address(int index);

// Looks up a vector table entry by interrupt number
struct VectorTableEntry* lookup_vector(int interruptNumber);

// Handles CPU execution with 5 actions
void handle_cpu(int value, int *current_time, FILE *output_file);

// Handles SYSCALL event using the vector table
void handle_syscall(int vector_index, int value, int *current_time, FILE *output_file);

// Handles END_IO event using the vector table
void handle_end_io(int vector_index, int value, int *current_time, FILE *output_file);

// Processes the trace file and writes to the output file
void process_trace_file(const char *input_filename, const char *output_filename);

void load_device_table(const char *filename);
void load_vector_table(const char *filename);

#endif // INTERRUPT_H
