#include "./types.h"
#include "./multitasking.h"
#include "./io.h"

// An array to hold all of the processes we create
proc_t processes[MAX_PROCS];

// Keep track of the next index to place a newly created process in the process array
uint8 process_index = 0;

proc_t *prev;    // The previously ran user process
proc_t *running; // The currently running process, can be either kernel or user process
proc_t *next;    // The next process to run
proc_t *kernel;  // The kernel process

// Select the next user process (proc_t *next) to run
// Selection must be made from the processes array (proc_t processes[])
int schedule()
{
    // Use prev to determine where to start searching for the next process
    // If prev is NULL, start from -1 so we begin searching from process 0
    int start_pid = (prev) ? prev->pid : -1;

    for (int offset = 1; offset <= MAX_PROCS; offset++)
    {
        int i = (start_pid + offset) % MAX_PROCS;

        if (processes[i].type == PROC_TYPE_USER && processes[i].status == PROC_STATUS_READY)
        {
            next = &processes[i];
            return 1;
        }
    }

    return 0;
}

int ready_process_count()
{
    int count = 0;

    for (int i = 0; i < MAX_PROCS; i++)
    {
        proc_t *current = &processes[i];

        if (current->type == PROC_TYPE_USER && current->status == PROC_STATUS_READY)
        {
            count++;
        }
    }

    return count;
}

// Create a new user process
// When the process is eventually ran, start executing from the function provided (void *func)
// Initialize the stack top and base at location (void *stack)
// If we have hit the limit for maximum processes, return -1
// Store the newly created process inside the processes array (proc_t processes[])
int createproc(void *func, char *stack)
{
    if (process_index >= MAX_PROCS)
        return -1;

    proc_t *p = &processes[process_index];
    p->type = PROC_TYPE_USER;
    p->status = PROC_STATUS_READY;
    p->pid = process_index;
    p->esp = (void *)stack;
    p->ebp = (void *)stack;
    p->eip = func;

    process_index++;
    return 0;
}

// Create a new kernel process
// The kernel process is ran immediately, executing from the function provided (void *func)
// Stack does not to be initialized because it was already initialized when main() was called
// If we have hit the limit for maximum processes, return -1
// Store the newly created process inside the processes array (proc_t processes[])
int startkernel(void func())
{
    if (process_index >= MAX_PROCS)
    {
        return -1;
    }
    proc_t kernproc;
    kernproc.status = PROC_STATUS_RUNNING;
    kernproc.type = PROC_TYPE_KERNEL;
    kernproc.pid = process_index;
    processes[process_index] = kernproc;
    kernel = &processes[process_index];
    process_index++;
    running = kernel;
    func();

    return 0;
}
void exit()
{
    running->status = PROC_STATUS_TERMINATED;

    if (running->type == PROC_TYPE_USER)
    {
        next = kernel;
        contextswitch();
    }
    else if (running->type == PROC_TYPE_KERNEL)
    {
        return;
    }
}

void yield()
{
    if (running->type == PROC_TYPE_USER)
    {
        running->status = PROC_STATUS_READY;
        next = kernel;
        contextswitch();
    }
    else if (running->type == PROC_TYPE_KERNEL)
    {
        if (schedule() == 1)
        {
            next->status = PROC_STATUS_RUNNING;
            prev = next;  // Update prev to point to the process we just scheduled
            contextswitch();
        }
    }
}

// Performs a context switch, switching from "running" to "next"
void contextswitch()
{
    // In order to perform a context switch, we need perform a system call
    // The system call takes inputs via registers, in this case eax, ebx, and ecx
    // eax = system call code (0x01 for context switch)
    // ebx = the address of the process control block for the currently running process
    // ecx = the address of the process control block for the process we want to run next

    // Save registers for later and load registers with arguments
    asm volatile("push %eax");
    asm volatile("push %ebx");
    asm volatile("push %ecx");
    asm volatile("mov %0, %%ebx" : : "r"(&running));
    asm volatile("mov %0, %%ecx" : : "r"(&next));
    asm volatile("mov $1, %eax");

    // Call the system call
    asm volatile("int $0x80");

    // Pop the previously pushed registers when we return eventually
    asm volatile("pop %ecx");
    asm volatile("pop %ebx");
    asm volatile("pop %eax");
}
