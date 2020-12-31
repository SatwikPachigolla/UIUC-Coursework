# Angrave's 2020 Acme CS 241 Exam Prep		
## A.K.A. Preparing for the Final Exam & Beyond CS 241... 
# partner: jeb5
Some of the questions require research (wikibook; websearch; wikipedia). 
It is accepted to work together and discuss answers, but please make an honest attempt first! 
Be ready to discuss and clear confusions & misconceptions in your last discussion section.
The final will also include pthreads, fork-exec-wait questions and virtual memory address translation. 
Be awesome. Angrave.

## 1. C 


1.	What are the differences between a library call and a system call? Include an example of each.

Library functions are an ordinary function that is placed in the collection of functions ie the library. System calls are a request to the OS for service. The difference between the two is that system calls are implemented in the kernel whereas library calls are implemented in user space. To make a system call, the app has to execute a hardware and system dependent instruction ( usually an interrupt or special SYSENTER call) instruction, which triggers context switch and forwards control to the kernel. Library calls on the other hand are no different from regular function calls, except that they do not reside in the main executable, and rather in shared libraries.

Examples: fopen is a library call, open is a system call.


2.	What is the `*` operator in C? What is the `&` operator? Give an example of each.

Those are the indirection and address-of operators, respectively. They are used to get the value of a variable at a certain location and memory and to get the address where a certain variable is stored, respectively.


3.	When is `strlen(s)` != `1+strlen(s+1)` ?


4.	How are C strings represented in memory? What is the wrong with `malloc(strlen(s))` when copying strings?

They are respresented as an array of characters with a terminating byte. strlen() does not count the terminating byte in the length so there will not be enough memory allocated for the terminating byte after the copy is completed.


5.	Implement a truncation function `void trunc(char*s,size_t max)` to ensure strings are not too long with the following edge cases.
```
if (length < max)
    strcmp(trunc(s, max), s) == 0
else if (s is NULL)
    trunc(s, max) == NULL
else
    strlen(trunc(s, max)) <= max
    // i.e. char s[]="abcdefgh; trunc(s,3); s == "abc". 
```


6.	Complete the following function to create a deep-copy on the heap of the argv array. Set the result pointer to point to your array. The only library calls you may use are malloc and memcpy. You may not use strdup.

    `void duplicate(char **argv, char ***result);` 

7.	Write a program that reads a series of lines from `stdin` and prints them to `stdout` using `fgets` or `getline`. Your program should stop if a read error or end of file occurs. The last text line may not have a newline char.

## 2. Memory 

1.	Explain how a virtual address is converted into a physical address using a multi-level page table. You may use a concrete example e.g. a 64bit machine with 4KB pages.

Handled by the MMU
MMU algorithm for a single-level page table:
	Check for invalid address
	If TLB contains the address:
		Get frame from TLB
		Perform read/write
	Else if page exists in memory:
		If process has necessary permissions:
			Convert address
			Add to TLB
		Else trigger hardware interrupt
	Else trigger hardware interrupt

A 64-bit machine with 4KB (2^12 bits), the bottom 12 bits will be used as an offset. The remaining 52 bits (64 -12)  will be divided up depending on the number of levels i.e. 2-level page table will use 26-bit-chunks of the virtual address as their indexes.


2.	Explain Knuth's and the Buddy allocation scheme. Discuss internal & external Fragmentation.

3.	What is the difference between the MMU and TLB? What is the purpose of each?

4.	Assuming 4KB page tables what is the page number and offset for virtual address 0x12345678  ?

5.	What is a page fault? When is it an error? When is it not an error?

A page fault occurs when a program attempts to access a block of memory that is not stored in the physical memory, or RAM. A page fault may be an error when a program tries to access a memory address that does not exist. Page faults actually will usually take place as background processes (without the user even noticing), so most of the time it is not an error.

6.	What is Spatial and Temporal Locality? Swapping? Swap file? Demand Paging?

## 3. Processes and Threads 

1.	What resources are shared between threads in the same process?

Everything but the stack because every thread has its own stack segment.
For example: 
Text segment (instructions)
Data segment (static and global data)
uninitialized data



2.	Explain the operating system actions required to perform a process context switch

3.	Explain the actions required to perform a thread context switch to a thread in the same process

4.	How can a process be orphaned? What does the process do about it?

5.	How do you create a process zombie?

6.	Under what conditions will a multi-threaded process exit? (List at least 4)

 exit
 return from the main thread
 pthread_cancel
 pthread_exit

## 4. Scheduling 
1.	Define arrival time, pre-emption, turnaround time, waiting time and response time in the context of scheduling algorithms. What is starvation?  Which scheduling policies have the possibility of resulting in starvation?

Source: Coursebook Scheduling section

Arrival time: “time at which a process first arrives at the ready queue, and is ready to start executing. If a CPU is idle, the arrival time would also be the starting time of execution.”
Preemption: “without preemption, processes will run until they are unable to utilize the CPU any further. ...With preemption, the existing processes may be removed immediately if a more preferred process is added to the ready queue.” 
Not using preemption can cause starvation because earlier processes may never be scheduled to run, ie assigned a CPU!
Turnaround time: “total time from when the process arrives to when it ends.”
Formula: turnaround = end_time - arrival_time
Waiting time: “the total wait time or the total time that a process is on the ready queue.” Waiting time is not only the initial waiting time in the ready queue -- it’s also the other times a job is waiting (eg in round robin, a job can be run and then evicted; the time in between being evicted and being run again would be included in waiting time)
Formula: wait time = end_time - arrival_time - run_time
Response time: time it takes for a process to start running after arriving
Formula: response time = (start time) - (arrival time)

Starvation: a process ready to run for CPU may never be scheduled to run
In general, not using preemption can result in starvation
First come first serve - e.g. first process might be super long
Shortest job first may suffer from starvation b/c lots of short jobs might come in, and longer jobs will never be run
Priority may suffer from starvation because jobs with low priorities may never be run
Round Robin does NOT suffer from starvation because it will cycle through all the processes at fixed intervals (a long-running process can not starve all other processes from running)


2.	Which scheduling algorithm results the smallest average wait time?

SJF (preemptive)

3.	What scheduling algorithm has the longest average response time? shortest total wait time?

4.	Describe Round-Robin scheduling and its performance advantages and disadvantages.

5.	Describe the First Come First Serve (FCFS) scheduling algorithm. Explain how it leads to the convoy effect. 

6.	Describe the Pre-emptive and Non-preemptive SJF scheduling algorithms. 

7.	How does the length of the time quantum affect Round-Robin scheduling? What is the problem if the quantum is too small? In the limit of large time slices Round Robin is identical to _____?

8.	What reasons might cause a scheduler switch a process from the running to the ready state?

## 5. Synchronization and Deadlock

1.	Define circular wait, mutual exclusion, hold and wait, and no-preemption. How are these related to deadlock?

Define circular wait, mutual exclusion, hold and wait, and no-preemption. How are these related to deadlock?

Circular wait: when each process is waiting for a resource held by another process which in turn is waiting for the first process to release its resource. This is considered a condition for deadlock.
Mutual exclusion: or mutex is a program object that disallows simultaneous access to a shared resource. If all processes mutually exclude resources at the same time then that is considered a condition for deadlock.
Hold and wait: when a process is holding at least a resource and is trying to request additional resources that are held by other processes
Related to deadlock: If a process is holding a resource that another needs but is waiting for another resource held by another who also needs another held resource, it will create a deadlock for both processes
No pre-emption: a process can only be released voluntarily when the process has completed its task
Related to deadlock: If a process needs a resource held by another process, it cannot force the other process to release the needed resource until the other process has finished their task which may result in deadlock
All of these conditions are required for deadlock.


2.	What problem does the Banker's Algorithm solve?

3.	What is the difference between Deadlock Prevention, Deadlock Detection and Deadlock Avoidance?

Deadlock Detection: Identifying if the state of a part of system is at a point where no state changes are possible due to processes waiting on each other in a circular manner.
Deadlock avoidance: Circumventing decisions which would put the system in an unsafe state.
Deadlock prevention: Designing a system such that deadlocks are not possible.

4.	Sketch how to use condition-variable based barrier to ensure your main game loop does not start until the audio and graphic threads have initialized the hardware and are ready.

5.	Implement a producer-consumer fixed sized array using condition variables and mutex lock.

6.	Create an incorrect solution to the CSP for 2 processes that breaks: i) Mutual exclusion. ii) Bounded wait.

7.	Create a reader-writer implementation that suffers from a subtle problem. Explain your subtle bug.

## 6. IPC and signals

1.	Write brief code to redirect future standard output to a file.

2.	Write a brief code example that uses dup2 and fork to redirect a child process output to a pipe

3.	Give an example of kernel generated signal. List 2 calls that can a process can use to generate a SIGUSR1.

4.	What signals can be caught or ignored?

Signals like SIGINT, SIGTERM, SIG_DFL etc.. can be caught
You cannot ignore the SIGKILL or SIGSTOP signals.

SIGINT & 2 & Terminate (Can be caught) & Stop a process nicely
SIGQUIT & 3 & Terminate (Can be caught) & Stop a process harshly
SIGSTOP & N/A & Stop Process (Cannot be caught) & Suspends a process
SIGKILL & 9 & Terminate Process (Cannot be caught) & You want the process gone


5.	What signals cannot be caught? What is signal disposition?

As explained above, SIGSTOP and SIGKILL both cannot be caught. Signal disposition is the action a process takes when a signal is delivered. Each signal has a disposition.

6.	Write code that uses sigaction and a signal set to create a SIGALRM handler.

7.	Why is it unsafe to call printf, and malloc inside a signal handler?

## 7. Networking 

1.	Explain the purpose of `socket`, `bind`, `listen`, and `accept` functions

Socket - creates an endpoint for communication, returns a file descriptor that refers to that endpoint
Bind - Assigns a address to the socket 
Listen - specifies how many unaccepted new connections should be allowed, allows a backlog of connections
Accept - returns a file descriptor of a newly connected client

Remember: used in order socket, bind, listen, accept


2.	Write brief (single-threaded) code using `getaddrinfo` to create a UDP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

3.	Write brief (single-threaded) code using `getaddrinfo` to create a TCP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

4.	Explain the main differences between using `select` and `epoll`. What are edge- and level-triggered epoll modes?

5.	Describe the services provided by TCP but not UDP. 

Attempts Recovery
Gaurantees Delivery

6.	How does TCP connection establishment work? And how does it affect latency in HTTP1.0 vs HTTP1.1?

7.	Wrap a version of read in a loop to read up to 16KB into a buffer from a pipe or socket. Handle restarts (`EINTR`), and socket-closed events (return 0).

8.	How is Domain Name System (DNS) related to IP and UDP? When does host resolution not cause traffic?

9.	What is NAT and where and why is it used? 

## 8. Files 

1.	Write code that uses `fseek`, `ftell`, `read` and `write` to copy the second half of the contents of a file to a `pipe`.

2.	Write code that uses `open`, `fstat`, `mmap` to print in reverse the contents of a file to `stderr`.

3.	Write brief code to create a symbolic link and hard link to the file /etc/password

4.	"Creating a symlink in my home directory to the file /secret.txt succeeds but creating a hard link fails" Why? 

5.	Briefly explain permission bits (including sticky and setuid bits) for files and directories.

int chmod(const char *path, mode_t mode);

Sticky bits were a bit that could be set on an executable file that would allow a program’s text segment to remain in swap even after the end of the program’s execution.

When a directory’s sticky bit is set only the file’s owner, the directory’s owner, and the root user can rename or delete the file. 

Setuid bit indicates that when run, the program will set the uid of the user to that of the owner of the file. 
Read (value of 4)- able to read
Write (value of 2)- able to write
Execute (value of 1)- able to execute (used for programs), “navigation bit”

Permission bits can be set for the individual owner of the file, the group the file belongs to, and everyone else. 

Example: 754 (read (4), write (2), execute for owner (1); read (4) and execute (1) for the group; read (4) for everyone else) 

Sticky - limits deletion of files/renaming files to users that have permissions in that file or dir
Setuid - Whether or not, when executed, the file should run *as its owner*, so if the file is owned by root, and a rando runs it, if the setuid bit is set, it runs as root.

6.	Write brief code to create a function that returns true (1) only if a path is a directory.

struct stat my_stat;
memset(my_stat, 0, sizeof(struct stat));
stat(path, &my_stat);
return S_ISDIR(my_stat.st_mode);

7.	Write brief code to recursive search user's home directory and sub-directories (use `getenv`) for a file named "xkcd-functional.png' If the file is found, print the full path to stdout.

8.	The file 'installmeplz' can't be run (it's owned by root and is not executable). Explain how to use sudo, chown and chmod shell commands, to change the ownership to you and ensure that it is executable.

## 9. File system 
Assume 10 direct blocks, a pointer to an indirect block, double-indirect, and triple indirect block, and block size 4KB.

1.	A file uses 10 direct blocks, a completely full indirect block and one double-indirect block. The latter has just one entry to a half-full indirect block. How many disk blocks does the file use, including its content, and all indirect, double-indirect blocks, but not the inode itself? A sketch would be useful.

2.	How many i-node reads are required to fetch the file access time at /var/log/dmesg ? Assume the inode of (/) is cached in memory. Would your answer change if the file was created as a symbolic link? Hard link?

3.	What information is stored in an i-node?  What file system information is not? 

Inode is the key abstraction. An inode is a file.  The inode does not store the filename, that is stored inside of directories.
Information stored in inode:
Device ID
File serial numbers
File mode
Link Count
User ID
Group ID
Size
Timestamps (accessed, modified and changed)
Disk block locations


4.	Using a version of stat, write code to determine a file's size and return -1 if the file does not exist, return -2 if the file is a directory or -3 if it is a symbolic link.

5.	If an i-node based file uses 10 direct and n single-indirect blocks (1 <= n <= 1024), what is the smallest and largest that the file contents can be in bytes? You can leave your answer as an expression.

6.	When would `fstat(open(path,O_RDONLY),&s)` return different information in s than `lstat(path,&s)`?

They would return different information when the named file is a symbolic link. In this case, lstat() returns information about the link, but fstat() returns information about the file the link references.

## 10. "I know the answer to one exam question because I helped write it"

Create a hard but fair 'spot the lie/mistake' multiple choice or short-answer question. Ideally, 50% can get it correct.


Spot the error in the following code to return a string that is reversed result of the concatenation of two passed in strings? How does the functions behavior differ from the desired behavior? How can this be fixed?

char* concatReverse(char* str1, char* str2) {
    int size = strlen(str1) + strlen(str2) + 1;
    char* result = malloc(size);
    strcpy(result, str1);
    strcat(result, str2);
    int i;
    for (i = 0; i<=size/2; i++){
        char temp = result[i];
        result[i] = result[size-i];
        result[size-i] = temp;
    }
    return result;   
}
