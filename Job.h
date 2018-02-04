#ifndef JOB_H
#define JOB_H


typedef enum {NEW, READY, RUNNING, BLOCKED, EXIT} state;

typedef struct{
    int type_flag;
    int lenght;
    int io_max;
}instruction;

typedef struct {
    int id;
    int arrival_time;
    state pState;
    int  totalLeght;
    int numbOfInstr;
    int instrDone; //l'ultima istruzione completata (per salvare quelle bloccanti)
    instruction *instr;
}Job;

typedef struct {
    Job job;
	long clock;
	int core;
}args;

void scheduler_preemptive(Job* jobs, int quantum, int jobCount, char preempt_output[]);
void scheduler_not_preemptive( int jobCount, char no_preempt_output[]);
void synch_signal (int sig);
#endif /* JOB_H*/


