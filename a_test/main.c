// main.c
// #define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
// #include <asm/types.h>

struct prcs_info {
	int prio; 			// priority
	unsigned int state;			// -1 unrunnable, 0 runnable, >0 stopped
	int nscount; 		// namespace count
	unsigned long nvcsw; 		// context switch count
	unsigned long long start_time; 	// start time in ns
};

int esoncall_warpper(struct prcs_info *pif, pid_t pid)
{
	int err = 0;

	asm("MOV x8, #451;"                           // x8 holds syscall no
		"SVC #0;"                                 // supervisor call (syscall in arm64)
		"MOV %[result], x0" : [result] "=r" (err) // copy return code to err
		);
	return err;
}

int main(int argc, char *argv[]) {
    pid_t tid;
    struct prcs_info Data; 
    printf("start programs ======\n");
    tid = getpid();
    int err = esoncall_warpper(&Data, tid); // syscall!
    if (err != 0) {
        printf("Got Error(error code): %d\n", err);
    }
    printf("pid: %d\tprio: %d\tcontext switch: %ld\tstart_time: %lld\tnscount: %d\n", 
            tid, Data.prio, Data.nvcsw, Data.start_time, Data.nscount);
    
    if (argc != 2 ) {
        printf("GoodBye\n");
        return 0;
    }

    printf("read commandline param\n");
    sscanf(argv[1], "%d", &tid );
    printf("Geting pid = %d\n",tid );
    err = esoncall_warpper(&Data, tid);//read
    if (err != 0) {
        printf("Got Error(error code): %d\n", err);
    }
    printf("pid: %d\tprio: %d\tcontext switch: %ld\tstart_time: %lld\tnscount: %d\n", 
            tid, Data.prio, Data.nvcsw, Data.start_time, Data.nscount);
    printf("GoodBye\n"); 
    return 0;
}