# ICT 1007 Assignment

Algorithm: Variation of Round Robin

## Vars used for PROCESS struct:
1. ID
2. Arrival Time
3. Burst Time
4. Waiting Time
5. End Time
6. Turnaround Time
7. Run Count

## Vars used for Process Array Struct
1. Total Number of Processes (total_processes)
2. Max number of Processes that can be stored in the array (process_capacity)
3. Total and Average Burst Time of all Processes in array
4. Pointer to point to list of Processes

## Vars used for looping through Processes
1. Process Counter (PC)
2. Time Quantum (TQ)
3. Common Burst Time Counter (time_counter)

## Pseudo Code
1. Initialize empty Process Array
2. Open file for reading, and add each line as a Process to the Process Array 
3. Initialize TQ as Average Burst Time of Process Array / 2
4. For each Process in Process Array
  1. Check if PC == total_processes, if so, reset PC to 0
  2. Assign Pointers for current, next and previous process (current_proc, next_proc, prev_proc)
    1. If prev_proc has finished running AND End Time for prev_proc is earlier than Arrival Time of current_proc AND Run Count of current_proc is 0,
      1. Set time_counter to be Arrival Time of current_proc
  3. Check if Burst Time of current_proc is 0, move to next Process if so
  4. Else, run Process
    1. If Burst Time of current_proc is more than TQ,
      1. Run Process for TQ duration
        1. Add TQ duration to time_counter
        2. Subtract TQ duration from Burst Time of current_proc
      2. Check if Remaining Burst Time of current_proc is less than 1/2 of TQ, run Process fully if so
        1. Add Remaining Burst Time of current_proc to time_counter
        2. Set Burst Time of current_proc to be 0
  5. Set End Time of current_proc to be the same as time_counter
  6. Check if Burst Time of current_proc is 0, if so:
    1. Calculate Waiting and Turnaround Time for current_proc
  7. Increase Run Count of current_proc
  8. Check if End Time of current_proc is earlier than Arrival Time of next_proc
    1. Go to first unfinished Process in the Array
  9. Check if all Processes have finished running, exit loop if so
5. Print out results
6. Clear memory used by Process Array
