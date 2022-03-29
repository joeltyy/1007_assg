#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 256

FILE *fptr;
char line_buffer[MAX_LENGTH], *str_token;

//  Process definition
typedef struct
{
    /*
        define Process with the following variables
        1. ID
        2. Arrival Time
        3. Burst Time
        4. Backup variable for Burst Time to use in future calculations of Waiting Time
        5. Waiting Time (Turnaround Time - Burst Time)
        6. Process End Time
        7. Turnaround Time (Process End Time - Process Arrival Time)
    */
    int id, arrival_time, original_burst_time, burst_time, waiting_time, end_time, turnaround_time, run_count;
} Process;

// Process Array definition
typedef struct
{
    /*
        In defining the Process Array, the following variables should be added:
        1. Total Number of Processes
        2. Maximum Capacity in Array
        3. Total Burst Time - calculate Average Burst Time
        4. Average Burst Time - calculate initial time quantum to be used
        5. Pointer for Process struct of undefined size - used to point to actual array which stores the data
    */
    int total_processes, process_capacity;
    float total_burst_time, avg_burst_time;
    Process *process_list;
} Process_Array;

// Process array functions
Process_Array *create_process_array()
{
    // function to initialise process array with a max capacity of 20 processes
    Process_Array *tmp = malloc(sizeof(Process_Array));
    memset(tmp, 0, sizeof(*tmp));

    tmp->total_processes = 0;
    tmp->process_capacity = 20;
    tmp->process_list = malloc(sizeof(Process) * tmp->process_capacity);

    return tmp;
}

void destroy_process_array(Process_Array *proc_array)
{
    // function to clear memory after use
    free(proc_array);
}

// Process functions
Process *add_proc(Process_Array *proc_array, const int proc_id, const int proc_arrival_time, const int proc_burst_time)
{
    // function to add process into process array with variables: id, arrival time, burst time
    if (proc_array->total_processes == proc_array->process_capacity)
    { // check if process array is reaching max capacity
        printf("Not enough memory, adding somemore...\n");
        proc_array->process_capacity += 20; // number can be changed as you like
        printf("Max number of processes is now: %d\n\n", proc_array->process_capacity);
        proc_array->process_list = realloc(proc_array->process_list, sizeof(Process) * proc_array->process_capacity); // allocate enough space for 20 more processes
    }
    Process *process = (proc_array->process_list + proc_array->total_processes); // point to latest location pointer and allocate process at that location

    /*
        When adding process to array, initialise:
        1. End Time to be the same as the Arrival Time
        2. Burst Time
        3. Set Waiting and Turnaround time to 0
    */
    process->id = proc_id;
    process->arrival_time = process->end_time = proc_arrival_time;
    process->burst_time = process->original_burst_time = proc_burst_time;
    process->waiting_time = process->turnaround_time = process->run_count = 0;

    /*
        Then add Process Burst Time to Total Burst Time of Array, increase the total number of processes and calculate Average Burst Time
    */
    proc_array->total_burst_time += proc_burst_time;
    proc_array->total_processes++;
    proc_array->avg_burst_time = proc_array->total_burst_time / proc_array->total_processes;

    return process;
}

void *run_proc(Process_Array *proc_array, int process_number, int time_quantum, int *time_counter)
{
    /*
        function to check:
        1. if burst time of current process is less than time quantum
            a. run process fully
        2. if burst time of current process is more than time quantum
            a. run process for period of time quantum
            b. then check if remaining burst time of process is less than half of time quantum
            c. run process fully if so

        Time Counter is a common variable used to calculate the end time for each process, since:
        1. The run times of each process would affect the end times of subsequent processes
            eg. TQ is 3, when P1 runs for the TQ duration, P2 needs to factor in the run time of P1 to its own end time, before it can run.
    */
    Process *current_proc;

    current_proc = &proc_array->process_list[process_number]; // point to address of currently selected process in process list

    printf("%d\t",
           current_proc->burst_time);

    if (current_proc->burst_time > time_quantum) // check if burst time of process is more than TQ
    {
        // run process for period of TQ, then subtract TQ duration from burst time of current process
        *time_counter += time_quantum;
        current_proc->burst_time -= time_quantum;

        // if remaining burst time is less than half of TQ
        if (current_proc->burst_time <= (time_quantum / 2))
        {
            // run process remaining burst time, and set remaining burst time to 0
            *time_counter += current_proc->burst_time;
            current_proc->burst_time = 0;
        }
    }
    else // burst time of process <= TQ
    {
        // run process remaining burst time, and set remaining burst time to 0
        *time_counter += current_proc->burst_time;
        current_proc->burst_time = 0;
    }

    // set end time for current process to be the time counter
    // end time of current process would keep changing if process has to be run in multiple times
    current_proc->end_time = *time_counter;

    // if process has finished running
    if (current_proc->burst_time == 0)
    {
        /*
            set process waiting and turnaround time using the following formulae
            1. Turnaround Time = End Time - Arrival Time
            2. Waiting Time = Turnaround Time - Burst Time
        */
        current_proc->turnaround_time = current_proc->end_time - current_proc->arrival_time;
        current_proc->waiting_time = current_proc->turnaround_time - current_proc->original_burst_time;
    }
    printf("%d\t%d\t%d\t%d",
           current_proc->burst_time,
           current_proc->waiting_time,
           current_proc->end_time,
           current_proc->turnaround_time);
    if (current_proc->burst_time == 0)
    {
        printf("\tFinished.\n");
    }
    else
    {
        printf("\n");
    }
    current_proc->run_count++;
}

void readFile(char *filename, Process_Array *proc_array)
{
    // function to read file via file name, and then add each process into process array

    // open file based on filename in read mode
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    { // if file cannot be found
        printf("Error!");
        exit(1);
    }
    else
    { // read file line by line
        int line_count = 0;
        int id, arr_time, burst_time;
        printf("Adding processes...\n\n");
        while (fgets(line_buffer, MAX_LENGTH, fptr))
        {
            id = line_count + 1;
            str_token = strtok(line_buffer, " "); // split line by " " and set as arrival time
            arr_time = atoi(str_token);
            if (str_token != NULL)
            {
                str_token = strtok(NULL, "\n"); // split remaining token by /n since end of line, then set as burst time
                burst_time = atoi(str_token);
            }
            Process *proc = add_proc(proc_array, id, arr_time, burst_time); // add process to process array and store newly created process in proc variable
            line_count++;
        }
    }
}

// compare function
int compare_process(const void *p1, const void *p2)
{
    /*
        Function to compare arrival times of 2 processes
        if negative number is returned, p1 is placed before p2
        else if positive number is returned, p1 is placed after p2
        otherwise, positioning of p1 and p2 does not matter since they arrive at the same time
    */
    int l = ((Process *)p1)->arrival_time;
    int r = ((Process *)p2)->arrival_time;
    return (l - r);
}

int check_all_finished(Process_Array *proc_array)
{
    int remaining_bt[proc_array->total_processes], tmp_counter = 0;
    for (int i = 0; i < proc_array->total_processes; i++)
    {
        remaining_bt[i] = proc_array->process_list[i].burst_time;
    }

    while (tmp_counter != proc_array->total_processes){
        if (remaining_bt[tmp_counter] == 0){
            if (tmp_counter == proc_array->total_processes -1){
                return 1;
                break;
            }
        }
        else{
            return 0;
        }
        tmp_counter++;
    }
}

int calculate_max_wt(Process_Array *proc_array){
    int max_wt = proc_array->process_list[0].waiting_time;
    Process * current_proc;
    for (int i = 1; i<proc_array->total_processes; i++){
        current_proc = &proc_array->process_list[i];
        if (max_wt < current_proc->waiting_time){
            max_wt = current_proc->waiting_time;
        }
    }
    return max_wt;
}

int calculate_max_tt(Process_Array *proc_array){
    int max_tt = proc_array->process_list[0].turnaround_time;
    Process * current_proc;
    for (int i = 1; i<proc_array->total_processes; i++){
        current_proc = &proc_array->process_list[i];
        if (max_tt < current_proc->turnaround_time){
            max_tt = current_proc->turnaround_time;
        }
    }
    return max_tt;
}

int main()
{
    int time_quantum, max_waiting_time, max_turnaround_time;
    float avg_waiting_time, avg_turnaround_time;
    avg_waiting_time = avg_turnaround_time = max_turnaround_time = max_waiting_time = 0;

    Process_Array *proc_array;
    proc_array = create_process_array(); // initialise empty process array

    // read file and add each process to process array
    // readFile("proc_time-same_arrival_time.txt", proc_array); // file for processes that all arrive at the same time
    // readFile("process_timings.txt", proc_array); // file for processes that arrive at different times

    // test cases for different arrival times
    Process *proc_1 = add_proc(proc_array, 1, 9, 4);
    Process *proc_2 = add_proc(proc_array, 2, 100, 6);
    Process *proc_3 = add_proc(proc_array, 3, 10, 8);
    Process *proc_4 = add_proc(proc_array, 4, 1, 5);
    Process *proc_5 = add_proc(proc_array, 5, 103, 4);
    Process *proc_6 = add_proc(proc_array, 6, 2, 9);

    // test cases for same arrival times
    // Process *proc_1 = add_proc(proc_array, 1, 0, 4);
    // Process *proc_2 = add_proc(proc_array, 2, 0, 6);
    // Process *proc_3 = add_proc(proc_array, 3, 0, 8);
    // Process *proc_4 = add_proc(proc_array, 4, 0, 5);
    // Process *proc_5 = add_proc(proc_array, 5, 0, 12);

    Process *current_proc, *next_proc, *prev_proc;

    // set initial time quantum to the nearest int rounded down from avg burst time * 25%
    // number can be changed as u wish
    time_quantum = ((int)proc_array->avg_burst_time * 0.5);

    // sort processes based on arrival time
    qsort(proc_array->process_list, proc_array->total_processes, sizeof(Process), compare_process);

    printf("Sorted Processes: \n");
    printf("Process\tArrival Time\tBurst Time\n");
    for (int i = 0; i < proc_array->total_processes; i++)
    {
        printf("#%d\t%d\t\t%d\n",
               proc_array->process_list[i].id,
               proc_array->process_list[i].arrival_time,
               proc_array->process_list[i].burst_time);
    }
    printf("Average Burst Time: %.2f\n\n", proc_array->avg_burst_time);

    printf("\nTQ: %d\n", time_quantum);
    printf("Calculating...\n\n");

    // i = process counter to see what process system is currently at
    // bt counter to track burst time, bt ptr to point to bt counter
    int i = -1, time_counter = proc_array->process_list[0].arrival_time;
    int *time_ptr;
    time_ptr = &time_counter;

    printf("Process\tAT\tBT\tRBT\tWT\tFT\tTT\n");

    while (1)
    {
        i++; // increment process counter
        if (i == proc_array->total_processes)
        {
            /*
                check if current process counter = total number of processes in process list
                reset process counter, and increase time quantum
            */
            time_quantum++;
            i = 0;
        }

        /*
            set pointer for current process to be process at index of process counter in process_list
            and next process to be the result of (process counter + 1) % number of total processes
            eg if process counter (i) currently is at the last process in the list, next process index would be 0 since i+1 == total num of processes
        */
        current_proc = &proc_array->process_list[i];
        next_proc = &proc_array->process_list[(i + 1) % proc_array->total_processes];

        // set pointer for previous process
        if (i > 0)
        {
            prev_proc = &proc_array->process_list[i - 1];

            /*
                check if previous process has finished running and end time is earlier than arrival time of current process
                if so, set common burst time counter to be Arrival Time for current process
                however, common burst time counter should only be changed once, and that is the before the firts
            */
            if (prev_proc->burst_time == 0 && prev_proc->end_time < current_proc->arrival_time && current_proc->run_count == 0)
            {
                time_counter = current_proc->arrival_time;
            }
        }
        else
        {
            int total_num = proc_array->total_processes;
            prev_proc = &proc_array->process_list[total_num - 1];
        }

        if (current_proc->burst_time == 0)
        {
            // move to next process
            continue;
        }
        else
        {
            // run current process
            // if end time of current process is earlier than arrival time of next process, jump to first unfinished process in the loop
            printf("#%d\t%d\t",
                   current_proc->id,
                   current_proc->arrival_time);

            run_proc(proc_array, i, time_quantum, time_ptr);

            if (current_proc->end_time < next_proc->arrival_time)
            {
                for (int j = 0; j < proc_array->total_processes; j++)
                {
                    if (proc_array->process_list[j].burst_time > 0)
                    {
                        i = j - 1;
                        break;
                    }
                }
            }
        }

        // break while loop when all processes in process array have finished running
        if (check_all_finished(proc_array))
        {
            break;
        }
    }

    printf("\nFinal Result:\n");
    printf("Process\tArrival Time\tEnd Time\tBurst Time\tWaiting Time\tTurnaround Time\n");

    for (int i = 0; i < proc_array->total_processes; i++)
    {
        // calculate max waiting and turnaround time, and avg waiting and turnaround time
        // something

        avg_waiting_time += proc_array->process_list[i].waiting_time;
        avg_turnaround_time += proc_array->process_list[i].turnaround_time;
        printf("#%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\n",
               proc_array->process_list[i].id,
               proc_array->process_list[i].arrival_time,
               proc_array->process_list[i].end_time,
               proc_array->process_list[i].original_burst_time,
               proc_array->process_list[i].waiting_time,
               proc_array->process_list[i].turnaround_time);
    }

    max_waiting_time = calculate_max_wt(proc_array);
    max_turnaround_time = calculate_max_tt(proc_array);

    printf("\nAvg Waiting time: %.2f\nAvg Turnaround Time: %.2f\n",
           avg_waiting_time / proc_array->total_processes,
           avg_turnaround_time / proc_array->total_processes);
    printf("Max Waiting time: %d\nMax Turnaround Time: %d\n",
           max_waiting_time,
           max_turnaround_time);

    destroy_process_array(proc_array); // clear memory used
    printf("\nMemory cleared\n");
}
