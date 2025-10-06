#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_DEVICES 100

int device_table[MAX_DEVICES];
unsigned int vector_table[MAX_DEVICES];
int num_devices = 0;

int find_delay(int device_code) {
     if (device_code >= 0 && device_code < num_devices) {
        return device_table[device_code];
    }
    return 0;
}

unsigned int get_isr_address(int index) {
    if (index >= 0 && index < num_devices) {
        return vector_table[index];
    }
    return 0;
}

// Handles CPU Bursts
void handle_cpu(int value, int *current_time, FILE *output_file) {
    int random_time;

    //CPU Burst
    fprintf(output_file, "%d, %d, CPU Burst\n", *current_time, value);
    *current_time += value;
}

// Function to handle SYSCALL event using vector table
void handle_syscall(int vector_index, int *current_time, FILE *output_file) {
        if (vector_index < 0) {
        fprintf(output_file, "Invalid vector index: %d\n", vector_index);
        return;
    }
    unsigned int delay = find_delay(vector_index);
    unsigned int isr_address = get_isr_address(vector_index);

    int random_time, remaining_time;

        fprintf(output_file, "%d, 1, switch to kernel mode\n", *current_time);
        *current_time += 1;

        fprintf(output_file, "%d, %d, context saved\n", *current_time, 10);
        *current_time += 10;


        // Action 3: Find vector in memory position
        short vector[20];
        fprintf(output_file, "%d, 1, find vector %d in memory position 0x%X\n",
                *current_time, vector_index, vector_index * 2);
        *current_time += 1;

        // Action 4: Load address into the PC
        fprintf(output_file, "%d, 1, load address 0x%X into the PC\n",
                *current_time, isr_address);
        *current_time += 1;

        // Now, after loading the address, we handle SYSCALL actions
       
        // Calculate remaining time for SYSCALL:
        remaining_time = delay - (1 + random_time + 1 + 1);  // Subtract the time already spent

        if (remaining_time > 0) {
            // Randomly distribute the remaining time across 3 actions
            int run_isr_time = (rand() % remaining_time);
            int transfer_data_time = (rand() % (remaining_time - run_isr_time));
            int check_errors_time = remaining_time - run_isr_time - transfer_data_time;

            // Action 5: SYSCALL: run the ISR
            fprintf(output_file, "%d, %d, SYSCALL: run the ISR\n", *current_time, run_isr_time);
            *current_time += run_isr_time;

            // Action 6: Transfer data
            fprintf(output_file, "%d, %d, transfer data\n", *current_time, transfer_data_time);
            *current_time += transfer_data_time;

            // Action 7: Check for errors
            fprintf(output_file, "%d, %d, check for errors\n", *current_time, check_errors_time);
            *current_time += check_errors_time;
        } else {
            fprintf(output_file, "Warning: Not enough time for SYSCALL sub-actions.\n");
        }

        // Action 8: IRET (always takes 1 time unit)
        fprintf(output_file, "%d, 1, IRET\n", *current_time);
        *current_time += 1;
}

// Function to handle END_IO event using vector table
void handle_end_io(int vector_index, int *current_time, FILE *output_file) {

        if (vector_index < 0) {
        fprintf(output_file, "Invalid vector index: %d\n", vector_index);
        return;
    }
        int random_time;
        unsigned int isr_address = get_isr_address(vector_index);
        unsigned int delay = find_delay(vector_index);

        // Action 1: Checks the priority of the interrupt (always takes 1 time unit)
        fprintf(output_file, "%d, 1, check priority of interrupt\n", *current_time);
        *current_time += 1;
       
        // Action 2: Checks if the interrupt is masked (always takes 1 time unit)
        fprintf(output_file, "%d, 1, check if masked\n", *current_time);
        *current_time += 1;

        // Action 3: Switch to kernel mode (always takes 1 time unit)
        fprintf(output_file, "%d, 1, switch to kernel mode\n", *current_time);
        *current_time += 1;

        // Action 4: Context saved (takes random time between 1 and 3)
        random_time = (rand() % 3) + 1;
        fprintf(output_file, "%d, %d, context saved\n", *current_time, random_time);
        *current_time += random_time;

        // Action 5: Find vector in memory position
        fprintf(output_file, "%d, 1, find vector %d in memory position 0x%X\n",
                *current_time, vector_index, vector_index * 2);
        *current_time += 1;

        // Action 6: Load address into the PC
        fprintf(output_file, "%d, 1, load address 0x%X into the PC\n",
                *current_time, isr_address);
        *current_time += 1;

        // Action 7: END_IO with given value as the duration
        fprintf(output_file, "%d, %d, END_IO\n", *current_time, delay);
        *current_time += delay;

        // Action 8: IRET (always takes 1 time unit)
        fprintf(output_file, "%d, 1, IRET\n", *current_time);
        *current_time += 1;
}

void load_device_table(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening vector table");
        exit(1);
    }

    
    num_devices = 0;
    while (fscanf(file, "%d", &device_table[num_devices]) == 1) {
        num_devices++;
        if (num_devices >= MAX_DEVICES) break;
    }

    fclose(file);
    printf("Loaded %d device entries.\n", num_devices);

}

void load_vector_table(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening vector table");
        exit(1);
    }

    int i = 0;
    while (fscanf(file, "%x", &vector_table[i]) == 1) {
        i++;
        if (i >= MAX_DEVICES) break;
    }

    fclose(file);

    if (i != num_devices) {
        fprintf(stderr, "Warning: vector table size (%d) != device table size (%d)\n", i, num_devices);
    }

    printf("Loaded %d ISR addresses.\n", i);
}


void process_trace_file(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        return;
    }
    


    char line[100];
    int current_time = 0;  // Track the current time

    srand(time(NULL));  // Seed for random number generator

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Remove newline character at the end
        line[strcspn(line, "\n")] = 0;

        // Tokenize based on commas and spaces
        char *event_type = strtok(line, ", ");
        char *param1 = strtok(NULL, ", ");
       
        if (strcmp(event_type, "CPU") == 0) {
            int value = atoi(param1);
            handle_cpu(value, &current_time, output_file);
        } else if (strcmp(event_type, "SYSCALL") == 0) {
            int device_code = atoi(param1);
            
            handle_syscall(device_code, &current_time, output_file);
        } else if (strcmp(event_type, "END_IO") == 0) {
            int device_code = atoi(param1);

            handle_end_io(device_code, &current_time, output_file);
        } else {
            fprintf(output_file, "Unknown event: %s\n", event_type);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

int main() {
    //Load Tables
    load_device_table("device_table.txt");
    load_vector_table("vector_table.txt");
    
    // Process the trace file and write to execution.txt
    process_trace_file("trace.txt", "execution.txt");
   
    return 0;
}
