#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "Job.h"

void scheduler_not_preemptive(Job *jobs, int jobCount, char no_preempt_output[]);
//void sort (Job *readyJob, int size);
int sort (const void *s1, const void *s2);
void sort1 (Job *readyJob, int size);
void* core (void* parameters);
void clean_job(Job *jobs, int pos, int JobCount);
int rand_lim(int limit);

/**
0 = READY
1 = RUNNING
2 = DONE
**/

int core_status[2];
int jobDone = 0;
int mut = 4;

pthread_mutex_t lock[4];
/*I JOB NON VENGONO ORDINATI PER LUNGHEZZA, LA FUNZIONE NON E CORRETTA*/
/**shortest job next**/
void scheduler_not_preemptive(Job *origJobs, int jobCount, char no_preempt_output[]){
	FILE *fp;
	fp = freopen(no_preempt_output, "w+", stdout);

    long long clock1 = 0;
    long long clock2 = 0;
    pthread_t thread[2];
    args thread_args[2];

    Job jobs[jobCount];
    for(int ii =0; ii< jobCount; ii++){
        jobs[ii] = origJobs[ii];
    }

    /**inizializza il MUTEX **/
    for(int iii=0; iii<mut; iii++){
        if(pthread_mutex_init(&lock[iii], NULL) != 0){
            fprintf(stderr," mutex init failed\n");
        }
    }

    core_status[0]=0;
    core_status[1]=0;

    for(int c = 0; c<jobCount; c++){
            jobs[c].pState = READY;
    }
    /*for(int i1=0; i1<jobCount; i1++){
        fprintf(stderr,"job numero: %d con numero istruzioni %d, pState %d, arrivalTime %d e lunghezza %d totalLeght %d\n", jobs[i1].id, jobs[i1].numbOfInstr, jobs[i1].pState, jobs[i1].arrival_time, jobs[i1].totalLeght,jobs[i1].totalLeght);
        for(int i2 = 0; i2 < jobs[i1].numbOfInstr; i2++ ){
             fprintf(stderr,"istruzione numero: %d con type_flag %d, lunghezza %d e io_max %d \n", i2, jobs[i1].instr[i2].type_flag,jobs[i1].instr[i2].lenght,jobs[i1].instr[i2].io_max);
        }
    }*/
    //sort1(jobs, jobCount);
    //qsort(jobs, jobCount, sizeof(Job), sort);
	
    while(1){
		
        pthread_mutex_lock(&lock[2]);
        
        if(jobDone == jobCount){
            break;
        }
        pthread_mutex_unlock(&lock[2]);
        //fprintf(stderr,"JOBDONE %d", jobDone );

        for (int i = 0; i < jobCount; i++){

            pthread_mutex_lock(&lock[1]);
            if(core_status[0] == 0  && jobs[i].pState == READY && clock1 > jobs[i].arrival_time){


                /**metto argomenti**/

                core_status[0] = 1;

                thread_args[0].job = jobs[i];
                thread_args[0].clock = clock1;
                thread_args[0].core = 0;
                jobs[i].pState = RUNNING;
				//fprintf(stderr,"core%d,%llu,%d,RUNNING\n", thread_args[0].core,clock1,thread_args[0].job.id);
                printf("core%d,%llu,%d,RUNNING\n", thread_args[0].core,clock1,thread_args[0].job.id);
                //printf( "Numero %d VA verso il core %d (%d e numbOfInstr %d)\n", thread_args[0].job.id, thread_args[0].core, thread_args[0].job.pState, thread_args[0].job.numbOfInstr);
                //printf("Job %d ha pState %d\n", i ,jobs[i].pState);
                pthread_create (&thread[0], NULL, &core, &thread_args[0]);

            }else if(core_status[1] == 0  && jobs[i].pState == READY && clock2 > jobs[i].arrival_time){

                /**metto argomenti**/

                core_status[1] = 1;

                thread_args[1].job = jobs[i];
                thread_args[1].clock = clock2;
                thread_args[1].core = 1;
                jobs[i].pState = RUNNING;

                pthread_mutex_lock(&lock[0]);
                printf("core%d,%llu,%d,RUNNING\n", thread_args[1].core, clock2, thread_args[1].job.id);
                //fprintf(stderr,"core%d,%llu,%d,RUNNING\n", thread_args[1].core, clock2, thread_args[1].job.id);
                //printf("Numero %d VA verso il core %d (pState %d e numbOfInstr %d)\n", thread_args[1].job.id, thread_args[1].core, thread_args[0].job.pState, thread_args[0].job.numbOfInstr);
                pthread_mutex_unlock(&lock[0]);
                //printf("Job %d ha pState %d\n", i ,jobs[i].pState);
                pthread_create (&thread[1], NULL, &core, &thread_args[1]);
            }else{
                //TODO perchè finisce qua?
                //fprintf(stderr,"Clock %llu %llu\n", clock1, clock2 );
            }
            pthread_mutex_unlock(&lock[1]);
            /**devo far si che i thread abbiano delle variabili run, ready, done**/

        }
        pthread_mutex_lock(&lock[1]);
        if(core_status[0] == 2){

            pthread_join (thread[0], NULL);
            core_status[0] = 0;
            clock1 = thread_args[0].clock;
            jobs[thread_args[0].job.id] = thread_args[0].job;
            //printf("CORE 0 STATUS: %d\n",core_status[0]);

            //printf("CORE 0 ha lasciato il JOB %d ha pState %d e istrDone %d\n", thread_args[0].job.id, jobs[thread_args[0].job.id].pState, jobs[thread_args[0].job.id].instrDone);
            if(thread_args[0].job.pState == BLOCKED ){
                //printf("job %d messo in READY \n", thread_args[0].job.id);
                printf("core%d,%llu,%d,READY\n", thread_args[0].core, clock1, thread_args[0].job.id);
                //fprintf(stderr,"core%d,%llu,%d,READY\n", thread_args[0].core, clock1, thread_args[0].job.id);
                jobs[thread_args[0].job.id].pState = READY;
                //thread_args[0].job.pState = READY;
            }
        }

         if(core_status[1] == 2){

            pthread_join (thread[1], NULL);
            core_status[1] = 0;
            clock2 = thread_args[1].clock;
            jobs[thread_args[1].job.id] = thread_args[1].job;
            //printf("CORE 1 STATUS: %d\n",core_status[1]);

            //printf("CORE 1 ha lasciato il JOB %d ha pState %d e istrDone %d\n", jobs[thread_args[1].job.id].id, jobs[thread_args[1].job.id].pState, jobs[thread_args[1].job.id].instrDone);
             if(thread_args[1].job.pState == BLOCKED ){
                printf("core%d,%llu,%d,READY\n", thread_args[1].core, clock2, thread_args[1].job.id);
                //fprintf(stderr,"core%d,%llu,%d,READY\n", thread_args[1].core, clock2, thread_args[1].job.id);
                //printf("job %d messo in READY\n", thread_args[1].job.id);
                //thread_args[1].job.pState = READY;
                jobs[thread_args[1].job.id].pState = READY;
            }

        }
        if(core_status[0] == 0){
			clock1++;
		}
        if(core_status[1] == 0){
			clock2++;
		}
        
        pthread_mutex_unlock(&lock[1]);
    }
    
	fclose(fp);
	
}

void* core (void* parameters){



    args* th_args = (args*) parameters;
    long long clock = (*th_args).clock;
    int coreNumb = (*th_args).core;

    int count = 0; //per poter riprendere un job se è stato interrotto
    
    if((*th_args).job.instrDone != 0){
        count = (*th_args).job.instrDone;
    }
    //printf("COUNT 1 del Job %d: %d\n",(*th_args).job.id, count);
    for (; count < (*th_args).job.numbOfInstr; count++){


        if((*th_args).job.instr[count].type_flag == 0){ //se l'istruzione è non bloccante


            int lenght = (*th_args).job.instr[count].lenght;
             
            //fprintf(stderr,"core %d ha eseguito un istruzione n. %d del job: %d\n",coreNumb, count, (*th_args).job.id);
            (*th_args).job.instrDone++;
            for (int j=0; j<lenght; j++){
                clock++;
            }

        }else if((*th_args).job.instr[count].type_flag == 1){

            clock++;
            int max = (*th_args).job.instr[count].io_max;
            (*th_args).job.instrDone = (count+1);
            /*espediente per fare da 1 a io_max*/
            int rnd = rand_lim(max);
			 pthread_mutex_lock(&lock[0]);
            (*th_args).job.pState = BLOCKED;
            (*th_args).job.arrival_time = clock + rnd;
            pthread_mutex_unlock(&lock[0]);
            /*se è bloccato metto lo stato BLOCKED e l'arrival_time lo setto al clock attuale
            aggiungendo il numero di cicli per cui deve restare bloccato (rnd) ANCHE SE  POTREBBE ESSERE SBAGLIATO
            ESSENDOCI DUE CICLI DI CLOCK DIVERSI*/
            printf("core%d,%llu,%d,BLOCKED\n", coreNumb, clock , (*th_args).job.id);
            //fprintf(stderr,"core%d,%llu,%d,BLOCKED\n", coreNumb, clock , (*th_args).job.id);
            //printf("core %d ha eseguito l' istruzione n. %d del job: %d ma è stato bloccato\n",coreNumb, count, (*th_args).job.id);
            break;

        }else{
            fprintf(stderr, "Errore nella lettura del type_flag");
            (*th_args).job.pState = BLOCKED;
        }
    }

    if((*th_args).job.numbOfInstr == count){
        /*completa procedura con job concluso*/
        pthread_mutex_lock(&lock[2]);
        jobDone++;
        pthread_mutex_unlock(&lock[2]);
        printf("core%d,%llu,%d,EXIT\n", coreNumb, clock , (*th_args).job.id);
        //fprintf(stderr,"core%d,%llu,%d,EXIT\n", coreNumb, clock , (*th_args).job.id);
        //fprintf(stderr,"CORE %d ha FINITO il job: %d\n",coreNumb, (*th_args).job.id);
        pthread_mutex_lock(&lock[0]);
        (*th_args).job.pState = EXIT;
        pthread_mutex_unlock(&lock[0]);
    }

    (*th_args).clock = clock;

    /**Completa job con mutex e status del core**/
    pthread_mutex_lock(&lock[1]);
    core_status[coreNumb] = 2;
    pthread_mutex_unlock(&lock[1]);
    return NULL;

}

void sort1 (Job *readyJob, int size){

    int i,j;
    Job temp;
    for(i=0; i<size; i++){

        for(j=i+1; j<size; j++){

            if(readyJob[i].totalLeght > readyJob[j].totalLeght){

                temp = readyJob[i];
                readyJob[i] = readyJob[j];
                readyJob[j] = temp;
            }
        }
    }
}
int sort (const void *s1, const void *s2){

    Job *j1 = (Job *)s1;
    Job *j2 = (Job *)s2;
    //int leghtcompare = strcmp(j1->totalLeght, j2->totalLeght);
    int leghtcompare=0;
    if(j1->totalLeght < j2->totalLeght){
        leghtcompare = -1;
    }else if(j1->totalLeght > j2->totalLeght){
        leghtcompare = 1;
    }else if (j1->totalLeght == j2->totalLeght){
        return j1->id - j2->id;
    }

    return leghtcompare;
}

int rand_lim(int limit) {

    limit -=1;
    int divisor = RAND_MAX/(limit+1);
    int retval;
	
    do {
        retval = rand() / divisor;
    } while (retval > limit);
    retval++;
    
    return retval;
}
