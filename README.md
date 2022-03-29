# 1007_assg
Assignment for 1007

Algorithm: round robin variation

process vars:
1. id
2. arrival time
3. burst time
4. waiting time
5. end time
6. turnaround time

process array vars:
1. total number of processes (total_processes)
2. max number of processes that can be stored in the array (process_capacity)
3. total and average burst time of all processes in process list
4. pointer that points to start of list for processes
- size of list of processes is undefined

loop vars: 
1. Process Counter (i) = -1
2. Time Counter = Arrival Time of first process in the Process Array
    a. common variable among all processes, and is used to calculate the End Time for each process
3. Current Process Pointer
4. Next Process Pointer
5. Previous Process Pointer

Program pseudo code:
1. Initialize empty Process Array
2. Read file and add each line into Process Array as a Process 
    a. Allocate more space if required
3. Sort Processes in Process Array based on Process Arrival Time, such that the Process with the lowest Arrival Time will be run first
4. Assign value of Time Quantum (TQ), based on Average Burst Time of all processes within the Process Array
5. For each Process in the Process Array:
    a. Increment Process Counter
    b. if Process Counter is equals to total_processes:
        i. Increment Time Quantum
        ii. Set Process Counter back to 0
    c. Define where the pointers of Current Process, Next Process and Previous Process should point to
        i.   Current Process pointer = Process in Process Array at index of Process Counter
        ii.  Next Process pointer = Process in Process Array at index of (Process Counter + 1) % total_processes
        iii. Previous Process pointer = Process in Process Array at index of (Process Counter - 1)
            • if Process counter = total_processes - 1:
                • Previous Process pointer = Process in Process Array at index of (total_processes - 1)
        iv.  If Previous Process has finished running AND the end time of Previous Process is earlier than Arrival Time of Current Process
            • Time Counter = Arrival Time of Current Process
    d. Check if Remaining Burst Time of Current Process is 0
        i. If so, continue to Next Process
        ii. else Run Current Process
            • If Remaining Burst Time of Current Process is more than TQ duration
                • Time Counter += TQ duration
                • Current Process Burst Time -= TQ duration
                • If Remaining Burst Time of Current Process is less than or equals to (TQ duration / 2)
                    • Time Counter += Remaining Burst Time of Process
                    • Current Process Burst Time = 0
            • Else
                • Time Counter += Remaining Burst Time of Process
                • Current Process Burst Time = 0
            • Set End Time of Current Process to be the same as the Time Counter
            • If Remaining Burst Time of Current Process is 0
                • Calculate Waiting and Turnaround Time for Current Process
            • If End Time of Current Process is earlier than Arrival Time of Next Process
                • Set Process Counter to be the index of the first unfinished Process in the Process Array
                • Jump to Process pointed to by Process Counter
    e. If all processes have finished running:
        i. End loop
6. Clear Memory allocated to Process Array
    
